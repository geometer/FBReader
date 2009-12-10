/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#include <iostream>

#include <ZLResource.h>
#include <ZLImage.h>

#include <ZLDialogManager.h>

#include "NetworkNodes.h"
#include "NetworkView.h"
#include "NetworkNodesFactory.h"
#include "NetworkCatalogUtil.h"
#include "NetworkOperationRunnable.h"

#include "../fbreader/FBReader.h"

#include "../network/NetworkLibraryItems.h"
#include "../network/NetworkBookCollection.h"
#include "../network/NetworkLink.h"
#include "../network/NetworkAuthenticationManager.h"

class NetworkCatalogNode::ExpandCatalogAction : public ZLRunnable {

public:
	ExpandCatalogAction(NetworkCatalogNode &node);
	void run();

private:
	NetworkCatalogNode &myNode;
};

class NetworkCatalogNode::OpenInBrowserAction : public ZLRunnable {

public:
	OpenInBrowserAction(const std::string &url);
	void run();

private:
	const std::string myURL;
};

class NetworkCatalogNode::ReloadAction : public ZLRunnable {

public:
	ReloadAction(NetworkCatalogNode &node);
	void run();

private:
	NetworkCatalogNode &myNode;
};

const std::string NetworkCatalogNode::TYPE_ID = "LibraryItemNode";


NetworkCatalogNode::NetworkCatalogNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition) : 
	FBReaderNode(parent, atPosition), 
	myItem(item) {
}

NetworkCatalogNode::NetworkCatalogNode(NetworkCatalogNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition) : 
	FBReaderNode(parent, atPosition), 
	myItem(item) {
}

shared_ptr<ZLRunnable> NetworkCatalogNode::expandCatalogAction() {
	if (myExpandCatalogAction.isNull()) {
		myExpandCatalogAction = new ExpandCatalogAction(*this);
	}
	return myExpandCatalogAction;
}

shared_ptr<ZLRunnable> NetworkCatalogNode::openInBrowserAction() {
	if (myOpenInBrowserAction.isNull()) {
		myOpenInBrowserAction = new OpenInBrowserAction(item().htmlURL());
	}
	return myOpenInBrowserAction;
}

shared_ptr<ZLRunnable> NetworkCatalogNode::reloadAction() {
	if (myReloadAction.isNull()) {
		myReloadAction = new ReloadAction(*this);
	}
	return myReloadAction;
}

NetworkLibraryCatalogItem &NetworkCatalogNode::item() {
	return (NetworkLibraryCatalogItem&)*myItem;
}

const NetworkLibraryItemList &NetworkCatalogNode::childrenItems() {
	return myChildrenItems;
}

const std::string &NetworkCatalogNode::typeId() const {
	return TYPE_ID;
}


void NetworkCatalogNode::paint(ZLPaintContext &context, int vOffset) {
	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);
	drawTitle(context, vOffset, myItem->title());
	const std::string &summary = item().summary();
	if (!summary.empty()) {
		drawSummary(context, vOffset, summary);
	}

	paintHyperlinks(context, vOffset);
}

void NetworkCatalogNode::paintHyperlinks(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("networkView")["libraryItemNode"];

	int left = 0;
	if (!item().url().empty()) {
		drawHyperlink(context, left, vOffset,
			resource[isOpen() ? "collapseTree" : "expandTree"].value(),
			expandCatalogAction()
		);
	}
	if (!item().htmlURL().empty()) {
		drawHyperlink(context, left, vOffset,
			resource["openInBrowser"].value(),
			openInBrowserAction()
		);
	}
	if (isOpen()) {
		drawHyperlink(context, left, vOffset, resource["reload"].value(), reloadAction());
	}
}

shared_ptr<ZLImage> NetworkCatalogNode::extractCoverImage() const {
	const std::string &url = myItem->cover();

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

	if (!myChildrenItems.empty()) {
		for (NetworkLibraryItemList::iterator it = myChildrenItems.begin(); it != myChildrenItems.end(); ++it) {
			NetworkNodesFactory::createNetworkNode(this, *it);
		}
	}
	FBReader::Instance().invalidateAccountDependents();
}

NetworkCatalogNode::ExpandCatalogAction::ExpandCatalogAction(NetworkCatalogNode &node) : myNode(node) {
}

void NetworkCatalogNode::ExpandCatalogAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	NetworkLink &link = myNode.item().link();
	if (!link.authenticationManager().isNull()) {
		NetworkAuthenticationManager &mgr = *link.authenticationManager();
		ZLBoolean3 authState = mgr.isAuthorised();
		std::cerr << "authState == " << authState << std::endl;
		if (authState == B3_UNDEFINED) {
			// TODO: show information message???
			return;
		}
		if (authState == B3_TRUE && mgr.needsInitialization()) {
			InitializeAuthenticationManagerRunnable initializer(mgr);
			initializer.executeWithUI();
			if (initializer.hasErrors()) {
				mgr.logOut();
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

void NetworkCatalogNode::OpenInBrowserAction::run() {
	FBReader::Instance().openLinkInBrowser(myURL);
}

NetworkCatalogNode::ReloadAction::ReloadAction(NetworkCatalogNode &node) : myNode(node) {
}

void NetworkCatalogNode::ReloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	bool openState = myNode.isOpen();
	myNode.updateChildren();
	if (openState) {
		myNode.expandOrCollapseSubtree();
	}
	FBReader::Instance().refreshWindow();
}

