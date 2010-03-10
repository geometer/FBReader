/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <algorithm>

#include <ZLResource.h>
#include <ZLImage.h>

#include <ZLDialogManager.h>

#include "NetworkNodes.h"
#include "NetworkView.h"
#include "NetworkNodesFactory.h"
#include "NetworkCatalogUtil.h"

#include "../fbreader/FBReader.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkBookCollection.h"
#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../networkActions/NetworkOperationRunnable.h"

class NetworkCatalogNode::OpenInBrowserAction : public ZLRunnableWithKey {

public:
	OpenInBrowserAction(const std::string &url);
	ZLResourceKey key() const;
	void run();

private:
	const std::string myURL;
};

const ZLTypeId NetworkCatalogNode::TYPE_ID(NetworkContainerNode::TYPE_ID);

NetworkCatalogNode::NetworkCatalogNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkItem> item, size_t atPosition) : 
	NetworkContainerNode(parent, atPosition), 
	myItem(item) {
}

NetworkCatalogNode::NetworkCatalogNode(NetworkCatalogNode *parent, shared_ptr<NetworkItem> item, size_t atPosition) : 
	NetworkContainerNode(parent, atPosition), 
	myItem(item) {
}

void NetworkCatalogNode::init() {
	if (!item().URLByType[NetworkItem::URL_CATALOG].empty()) {
		registerAction(new ExpandCatalogAction(*this));
	}
	const std::string htmlUrl =
		item().URLByType[NetworkItem::URL_HTML_PAGE];
	if (!htmlUrl.empty()) {
		registerAction(new OpenInBrowserAction(htmlUrl));
	}
	registerAction(new ReloadAction(*this));
}

NetworkCatalogItem &NetworkCatalogNode::item() {
	return (NetworkCatalogItem&)*myItem;
}

const NetworkItem::List &NetworkCatalogNode::childrenItems() {
	return myChildrenItems;
}

const ZLTypeId &NetworkCatalogNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkCatalogNode::resource() const {
	return ZLResource::resource("networkView")["libraryItemNode"];
}

std::string NetworkCatalogNode::title() const {
	return myItem->Title;
}

std::string NetworkCatalogNode::summary() const {
	return ((const NetworkCatalogItem&)*myItem).Summary;
}

shared_ptr<ZLImage> NetworkCatalogNode::extractCoverImage() const {
	const std::string &url = myItem->URLByType[NetworkItem::URL_COVER];

	if (url.empty()) {
		return lastResortCoverImage();
	}

	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(url);
	if (!image.isNull()) {
		return image;
	}

	if (url.find(':') == std::string::npos) {
		return defaultCoverImage(url);
	}

	return lastResortCoverImage();
}

shared_ptr<ZLImage> NetworkCatalogNode::lastResortCoverImage() const {
	return ((FBReaderNode*)parent())->coverImage();
}

void NetworkCatalogNode::updateChildren() {
	if (isOpen()) {
		open(false);
	}
	clear();

	myChildrenItems.clear();
	LoadSubCatalogRunnable loader(item(), myChildrenItems);
	loader.executeWithUI();

	if (loader.hasErrors()) {
		loader.showErrorMessage();
	} else if (myChildrenItems.empty()) {
		ZLDialogManager::Instance().informationBox(ZLResourceKey("emptyCatalogBox"));
	}

	bool hasSubcatalogs = false;
	for (NetworkItem::List::iterator it = myChildrenItems.begin(); it != myChildrenItems.end(); ++it) {
		if ((*it)->typeId() == NetworkCatalogItem::TYPE_ID) {
			hasSubcatalogs = true;
			break;
		}
	}

	if (hasSubcatalogs) {
		for (NetworkItem::List::iterator it = myChildrenItems.begin(); it != myChildrenItems.end(); ++it) {
			NetworkNodesFactory::createNetworkNode(this, *it);
		}
	} else {
		NetworkNodesFactory::fillAuthorNode(this, myChildrenItems);
	}
	FBReader::Instance().invalidateAccountDependents();
}

NetworkCatalogNode::ExpandCatalogAction::ExpandCatalogAction(NetworkCatalogNode &node) : myNode(node) {
}

ZLResourceKey NetworkCatalogNode::ExpandCatalogAction::key() const {
	return ZLResourceKey(myNode.isOpen() ? "collapseTree" : "expandTree");
}

void NetworkCatalogNode::ExpandCatalogAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	const NetworkLink &link = myNode.item().Link;
	if (!link.authenticationManager().isNull()) {
		NetworkAuthenticationManager &mgr = *link.authenticationManager();
		IsAuthorisedRunnable checker(mgr);
		checker.executeWithUI();
		if (checker.hasErrors()) {
			checker.showErrorMessage();
			return;
		}
		if (checker.result() == B3_TRUE && mgr.needsInitialization()) {
			InitializeAuthenticationManagerRunnable initializer(mgr);
			initializer.executeWithUI();
			if (initializer.hasErrors()) {
				LogOutRunnable logout(mgr);
				logout.executeWithUI();
			}
		}
	}

	if (myNode.myChildrenItems.empty()) {
		myNode.updateChildren();
	}
	myNode.expandOrCollapseSubtree();
	FBReader::Instance().refreshWindow();
}

NetworkCatalogNode::OpenInBrowserAction::OpenInBrowserAction(const std::string &url) : myURL(url) {
}

ZLResourceKey NetworkCatalogNode::OpenInBrowserAction::key() const {
	return ZLResourceKey("openInBrowser");
}

void NetworkCatalogNode::OpenInBrowserAction::run() {
	FBReader::Instance().openLinkInBrowser(myURL);
}

NetworkCatalogNode::ReloadAction::ReloadAction(NetworkCatalogNode &node) : myNode(node) {
}

ZLResourceKey NetworkCatalogNode::ReloadAction::key() const {
	return ZLResourceKey("reload");
}

bool NetworkCatalogNode::ReloadAction::makesSense() const {
	return myNode.isOpen();
}

void NetworkCatalogNode::ReloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	myNode.updateChildren();
	myNode.expandOrCollapseSubtree();
	FBReader::Instance().refreshWindow();
}
