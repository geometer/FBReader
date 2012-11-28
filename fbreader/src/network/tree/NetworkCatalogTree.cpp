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

#include <algorithm>

#include <ZLDialogManager.h>

#include "../authentication/NetworkAuthenticationManager.h"
#include "NetworkCatalogUtil.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "../NetworkErrors.h"
#include "NetworkTreeFactory.h"
#include "../../networkActions/AuthenticationDialogManager.h"
#include "NetworkTreeNodes.h"

const ZLTypeId NetworkCatalogTree::TYPE_ID(NetworkTree::TYPE_ID);

const ZLTypeId &NetworkCatalogTree::typeId() const {
	return TYPE_ID;
}

NetworkCatalogTree::NetworkCatalogTree(RootTree *parent, shared_ptr<NetworkItem> item, std::size_t position) :
	NetworkTree(parent, position), myItem(item) {
}

NetworkCatalogTree::NetworkCatalogTree(NetworkCatalogTree *parent, shared_ptr<NetworkItem> item, std::size_t position) :
	NetworkTree(parent, position), myItem(item) {
	init();
}

void NetworkCatalogTree::init() {
//	if (!item().URLByType[NetworkItem::URL_CATALOG].empty()) {
//		registerAction(new ExpandCatalogAction(*this));
//	}
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
		if (ZLTreeTitledNode* node = zlobject_cast<ZLTreeTitledNode*>(parent())) {
			return node->image();
		}
	}
	if (myImage.isNull()) {
		myImage = NetworkCatalogUtil::getAndDownloadImageByUrl(url, this);
	}
	return (!myImage.isNull() && myImage->good()) ? myImage : FBTree::defaultCoverImage("booktree-catalog.png");
}

class AsyncLoadSubCatalogRunnable : public ZLNetworkRequest::Listener {

public:
	AsyncLoadSubCatalogRunnable(NetworkCatalogTree *tree, bool resumeMode) :
		myTree(tree), myResumeMode(resumeMode) {
		myTree->notifyDownloadStarted();
		if (myResumeMode) {
			myTree->item().resumeLoading(myChildrens, this);
		} else {
			myTree->item().loadChildren(myChildrens, this);
		}
	}

	void finished(const std::string &error) {
		myTree->notifyDownloadStopped();
		myTree->onChildrenReceived(myChildrens, error);
	}

	void setUIStatus(bool enabled) {
		if (enabled) {
			myTree->notifyDownloadStarted();
		} else {
			myTree->notifyDownloadStopped();
		}
	}

private:
	NetworkCatalogTree *myTree;
	NetworkItem::List myChildrens;
	bool myResumeMode;
};

//class NetworkCatalogTreeAuthScope : public ZLUserData {
//public:
//	shared_ptr<ZLNetworkRequest::Listener> listener;
//};

class NetworkCatalogTreeAuthListener : public ZLNetworkRequest::Listener {
public:
	NetworkCatalogTreeAuthListener(NetworkCatalogTree *tree) : myTree(tree) {
	}

	void finished(const std::string &error) {
		myTree->onAuthCheck(error);
	}

	void setUIStatus(bool enabled) {
		if (enabled) {
			myTree->notifyDownloadStarted();
		} else {
			myTree->notifyDownloadStopped();
		}
	}

private:
	NetworkCatalogTree *myTree;
};

void NetworkCatalogTree::requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener) {
	myListeners.push_back(listener);
	if (myListeners.size() > 1) {
		return;
	}

	const NetworkLink &link = item().Link;
	shared_ptr<NetworkAuthenticationManager> manager = link.authenticationManager();

	if (item().getVisibility() == B3_UNDEFINED && !manager.isNull()) {
		AuthenticationDialogManager::authAndInitAsync(*manager, new NetworkCatalogTreeAuthListener(this));
		return;
	}

	if (!manager.isNull()) {
		AuthenticationDialogManager::athoriseIfCan(*manager, new NetworkCatalogTreeAuthListener(this));
		return;
	}
	onAuthCheck(std::string());
}

void NetworkCatalogTree::onAuthCheck(const std::string &error) {
	if (!error.empty()) {
		return;
	}

	if (!myChildrenItems.empty()) {
		notifyDownloadStopped();
		notifyListeners(std::string()); //TODO maybe not be silent about auth error here
		return;
	}
	new AsyncLoadSubCatalogRunnable(this, false);

}

void NetworkCatalogTree::requestMoreChildren(shared_ptr<ZLNetworkRequest::Listener> listener) {
	//TODO does double requesting is processed correct?
	if (!item().supportsResumeLoading()) {
		listener->finished();
		return;
	}
	myListeners.push_back(listener);
	if (myListeners.size() == 1) {
		new AsyncLoadSubCatalogRunnable(this, true);
	}
}

void NetworkCatalogTree::onChildrenReceived(NetworkItem::List &childrens, const std::string &error) {
	if (!error.empty()) {
		//special case for authenticationFailed after 'cancel' button pressed in AuthDialog?
		//TODO maybe it'll be work wrong at some cases? maybe we should have another error this case?
		if (error != NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			NetworkErrors::showErrorMessage(error);
		}
		notifyListeners(error);
		return;
	}

	myChildrenItems.insert(myChildrenItems.end(), childrens.begin(), childrens.end());

	if (myChildrenItems.empty()) {
		ZLDialogManager::Instance().informationBox(ZLResourceKey("emptyCatalogBox"));
		notifyListeners(error);
		return;
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
		for (NetworkItem::List::iterator it = childrens.begin(); it != childrens.end(); ++it) {
			NetworkTreeFactory::createNetworkTree(this, *it);
		}
	} else {
		NetworkTreeFactory::fillAuthorTree(this, childrens);
	}
	notifyListeners(error);

	this->updated();
}

void NetworkCatalogTree::notifyListeners(const std::string &error) {
	for (std::size_t i = 0; i < myListeners.size(); ++i) {
		if (!myListeners.at(i).isNull())
			myListeners.at(i)->finished(error);
	}
	myListeners.clear();
}

NetworkCatalogItem &NetworkCatalogTree::item() {
	return (NetworkCatalogItem&)*myItem;
}

void NetworkCatalogTree::updateVisibility() {
	//adding to remove list and clearing all existed nodes
	List toRemove;
	NetworkItem::List itemsWithNodes; //used in loop for creating new nodes (for these items new nodes won't be created)
	for (size_t i = 0; i < children().size(); ++i) {
		ZLTreeNode* tree = children().at(i);
		if (!tree->isInstanceOf(NetworkCatalogTree::TYPE_ID)) {
			continue;
		}
		NetworkCatalogTree *child = (NetworkCatalogTree*)tree;
		itemsWithNodes.push_back(child->myItem);
		switch (child->item().getVisibility()) {
			case B3_TRUE:
				child->updateVisibility();
				break;
			case B3_FALSE:
				toRemove.push_back(child);
				break;
			case B3_UNDEFINED:
				child->clearCatalog();
				break;
		}
	}

	//creating new nodes (if necessary)
	for (size_t i = 0; i < myChildrenItems.size(); ++i) {
		shared_ptr<NetworkItem> item = myChildrenItems.at(i);
		if (!item->isInstanceOf(NetworkCatalogItem::TYPE_ID)) {
			continue;
		}
		if (std::find(itemsWithNodes.begin(), itemsWithNodes.end(), item) != itemsWithNodes.end()) {
			continue;
		}
		NetworkCatalogItem *catalogItem = (NetworkCatalogItem*)(&*item);
		if (catalogItem->getVisibility() != B3_FALSE) {
			NetworkTreeFactory::createNetworkTree(this, item, i);
		}
	}

	for (size_t i = 0; i < toRemove.size(); ++i) {
		ZLTreeNode* tree = toRemove.at(i);
		remove(tree);
	}

}

void NetworkCatalogTree::clearCatalog() {
	myChildrenItems.clear();
	clear();
}

const ZLResource &NetworkCatalogTree::resource() const {
	return ZLResource::resource("networkView")["libraryItemNode"];
}
