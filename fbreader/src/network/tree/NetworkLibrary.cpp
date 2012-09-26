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

#include "../NetworkLinkCollection.h"

//#include "NetworkNodesFactory.h"

#include "../NetworkItems.h"
#include "../NetworkLinkCollection.h"
#include "../NetworkLink.h"
#include "../SearchResult.h"
#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkActions/NetworkOperationRunnable.h"

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
	myDialog = ZLDialogManager::Instance().createTreeDialog(ZLResource::resource("networkView"));
}

void NetworkLibrary::showDialog() {
	makeUpToDate();
	myDialog->run(&myRootTree);
}

void NetworkLibrary::makeUpToDate() {
	//TODO rewrite this method
	NetworkLinkCollection::Instance().initialize();
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();
	for (size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
		new NetworkCatalogRootTree(&myRootTree, link, i);
	}
}
