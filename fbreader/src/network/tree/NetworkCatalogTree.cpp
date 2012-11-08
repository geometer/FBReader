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
#include "../NetworkErrors.h"
#include "NetworkTreeFactory.h"
#include "../../networkActions/AuthenticationDialog.h"
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

void NetworkCatalogTree::requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener) {
	myListeners.push_back(listener);
	if (myListeners.size() > 1) {
		return;
	}

	myChildrenItems.clear();

	const NetworkLink &link = item().Link;
	shared_ptr<NetworkAuthenticationManager> manager = link.authenticationManager();

	if (item().getVisibility() == B3_UNDEFINED && !manager.isNull()) {
		bool result = AuthenticationDialog::run(*manager);
		if (!result) {
			notifyListeners(std::string());
			return;
		}
	}

	notifyDownloadStarted();
	shared_ptr<ZLUserDataHolder> scopeData = new ZLUserDataHolder;
	if (!manager.isNull()) {
//		NetworkCatalogTreeAuthScope *scope = new NetworkCatalogTreeAuthScope;
//		scope->listener = listener;
		manager->isAuthorised(ZLExecutionUtil::createListener(scopeData, this, &NetworkCatalogTree::onAuthCheck));
		return;
	}

	onInitialization(*scopeData, std::string());
}

void NetworkCatalogTree::onAuthCheck(ZLUserDataHolder &data, const std::string &error) {
	if (error.empty()) {
		const NetworkLink &link = item().Link;
		shared_ptr<NetworkAuthenticationManager> manager = link.authenticationManager();
		if (!manager.isNull() && manager->needsInitialization()) {
			manager->initialize(ZLExecutionUtil::createListener(new ZLUserDataHolder(data), this, &NetworkCatalogTree::onInitialization));
			return;
		}
	}
	onInitialization(data, error);
}

void NetworkCatalogTree::onInitialization(ZLUserDataHolder &data, const std::string &error) {
	if (!error.empty()) {
		const NetworkLink &link = item().Link;
		shared_ptr<NetworkAuthenticationManager> manager = link.authenticationManager();
		if (!manager.isNull()) {
			manager->logOut();
		}
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
}

void NetworkCatalogTree::notifyListeners(const std::string &error) {
	for (size_t i = 0; i < myListeners.size(); ++i) {
		if (!myListeners.at(i).isNull())
			myListeners.at(i)->finished(error);
	}
	myListeners.clear();
}

NetworkCatalogItem &NetworkCatalogTree::item() {
	return (NetworkCatalogItem&)*myItem;
}

const ZLResource &NetworkCatalogTree::resource() const {
	return ZLResource::resource("networkView")["libraryItemNode"];
}
