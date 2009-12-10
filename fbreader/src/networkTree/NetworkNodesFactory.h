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

#ifndef __NETWORKNODESFACTORY_H__
#define __NETWORKNODESFACTORY_H__

#include <ZLBlockTreeView.h>

#include "../network/NetworkLibraryItems.h"


class NetworkBookCollection;
class FBReaderNode;
class NetworkCatalogNode;
class SearchResultNode;
class NetworkAuthorNode;


class NetworkNodesFactory {

private:
	NetworkNodesFactory();

public:
	static FBReaderNode *createNetworkNode(NetworkCatalogNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition = -1);

	static void createSubnodes(SearchResultNode *parent, NetworkBookCollection &books);

private:
	static void fillAuthorNode(NetworkAuthorNode *authorNode, const NetworkLibraryItemList &books);
};

#endif /* __NETWORKNODESFACTORY_H__ */
