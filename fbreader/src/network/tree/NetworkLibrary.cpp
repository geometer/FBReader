/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include "../NetworkLinkCollection.h"

//#include "NetworkNodesFactory.h"

#include "../NetworkItems.h"
#include "../NetworkLinkCollection.h"
#include "../NetworkLink.h"
#include "../SearchResult.h"
#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "NetworkSearcher.h"
#include "NetworkTreeNodes.h"

#include "NetworkLibrary.h"

NetworkLibrary *NetworkLibrary::ourInstance = 0;

NetworkLibrary &NetworkLibrary::Instance() {
	if (ourInstance == 0) {
		ourInstance = new NetworkLibrary();
	}
	return *ourInstance;
}

NetworkLibrary::NetworkLibrary() {
	//TODO maybe it should be created in showDialog method?
	myDialog = ZLDialogManager::Instance().createTreeDialog("networkLibrary", ZLResource::resource("networkView"));
	myRootTree.setDialog(myDialog);
	myFakeRootTree.setDialog(myDialog);
	myUpdateVisibility = false;
	myChildrenAreInvalid = true;
}

void NetworkLibrary::showDialog() {
	synchronize();
	myDialog->run(&myRootTree);
	myDialog->setSearcher(new NetworkSearcher);
}

void NetworkLibrary::hideDialog() {
    myDialog->onCloseRequest();
}

void NetworkLibrary::refresh() {
	myDialog->onRefresh();
}

void NetworkLibrary::makeUpToDate() {
	//TODO rewrite this method
	NetworkLinkCollection::Instance().initialize();
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();
	for (std::size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
        new NetworkCatalogRootTree(&myRootTree, link, i);
	}
}

void NetworkLibrary::updateVisibility() {
	for (std::size_t i = 0; i < myRootTree.children().size(); ++i) {
		ZLTreeNode* tree = myRootTree.children().at(i);
		if (NetworkCatalogTree* catalogTree = zlobject_cast<NetworkCatalogTree*>(tree)) {
			catalogTree->updateVisibility();
		}
	}
}

RootTree &NetworkLibrary::getFakeCatalogTree() {
	return myFakeRootTree;
}

void NetworkLibrary::synchronize() {
	if (myChildrenAreInvalid) {
		myChildrenAreInvalid = false;
		makeUpToDate();
	}
	if (myUpdateVisibility) {
		myUpdateVisibility = false;
		updateVisibility();
	}
}

void NetworkLibrary::invalidateVisibility() {
	myUpdateVisibility = true;
}
