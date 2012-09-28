/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLDialogManager.h>

#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkTree/NetworkCatalogUtil.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "NetworkTreeFactory.h"
#include "NetworkTreeNodes.h"

const ZLTypeId NetworkCatalogTree::TYPE_ID(NetworkTree::TYPE_ID);

const ZLTypeId &NetworkCatalogTree::typeId() const {
	return TYPE_ID;
}

NetworkCatalogTree::NetworkCatalogTree(RootTree *parent, shared_ptr<NetworkItem> item, size_t position) :
	NetworkTree(parent, position), myItem(item) {
}

NetworkCatalogTree::NetworkCatalogTree(NetworkCatalogTree *parent, shared_ptr<NetworkItem> item, size_t position) :
	NetworkTree(parent, position), myItem(item) {
	init();
}

void NetworkCatalogTree::init() {
	if (!item().URLByType[NetworkItem::URL_CATALOG].empty()) {
		registerAction(new ExpandCatalogAction(*this));
	}
//	const std::string htmlUrl =
//		item().URLByType[NetworkItem::URL_HTML_PAGE];
//	if (!htmlUrl.empty()) {
//		registerAction(new OpenInBrowserAction(htmlUrl));
//	}
	//registerAction(new ReloadAction(*this));
}

std::string NetworkCatalogTree::title() const {
	return myItem->Title;
}

std::string NetworkCatalogTree::subtitle() const {
	return ((const NetworkCatalogItem&)*myItem).Summary;
}


shared_ptr<const ZLImage> NetworkCatalogTree::image() const {
	const std::string &url = myItem->URLByType[NetworkItem::URL_COVER];

	if (url.empty()) {
		//return lastResortCoverImage();
		return 0;
	}

	shared_ptr<const ZLImage> image = NetworkCatalogUtil::getImageByUrl(url);
	if (!image.isNull()) {
		return image;
	}

	if (url.find(':') == std::string::npos) {
		return defaultCoverImage(url);
	}

	return 0;
}

std::string NetworkCatalogTree::imageUrl() const {
	const std::string &url = myItem->URLByType[NetworkItem::URL_COVER];
	if (url.empty()) {
		if (ZLTreeTitledNode *node = zlobject_cast<ZLTreeTitledNode*>(parent())) {
			return node->imageUrl();
		}
	} else if (url.find(':') == std::string::npos) {
		return FBTree::defaultImageUrl(url);
	}
	return url;
}

void NetworkCatalogTree::requestChildren(shared_ptr<ZLExecutionData::Listener> /*listener*/) {
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

	//TODO rewrite this method
	if (hasSubcatalogs) {
		for (NetworkItem::List::iterator it = myChildrenItems.begin(); it != myChildrenItems.end(); ++it) {
			NetworkTreeFactory::createNetworkTree(this, *it);
		}
	} else {
		NetworkTreeFactory::fillAuthorTree(this, myChildrenItems);
	}
}

NetworkCatalogItem &NetworkCatalogTree::item() {
	return (NetworkCatalogItem&)*myItem;
}

const ZLResource &NetworkCatalogTree::resource() const {
	return ZLResource::resource("networkView")["libraryItemNode"];
}

//shared_ptr<const ZLImage> NetworkCatalogTree::lastResortCoverImage() const {
//	return ((FBTree*)parent())->coverImage();
//}

NetworkCatalogTree::ExpandCatalogAction::ExpandCatalogAction(NetworkCatalogTree &tree) : myTree(tree) {
}

ZLResourceKey NetworkCatalogTree::ExpandCatalogAction::key() const {
	return ZLResourceKey(/*myNode.isOpen() ? "collapseTree" :*/ "expandTree");
}

void NetworkCatalogTree::ExpandCatalogAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	const NetworkLink &link = myTree.item().Link;
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

	if (myTree.myChildrenItems.empty()) {
		myTree.requestChildren(0); //who should request his children? dialog or node himself?
	}
	if (!myTree.children().empty()) {
		myTree.expand();
	}
}
