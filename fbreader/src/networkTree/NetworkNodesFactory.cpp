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

#include "NetworkNodesFactory.h"

#include "NetworkNodes.h"

#include "../network/NetworkBookCollection.h"

FBReaderNode *NetworkNodesFactory::createNetworkNode(NetworkCatalogNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition) {
	const std::string &itemType = item->typeId();

	if (itemType == NetworkLibraryCatalogItem::TYPE_ID) {
		return new NetworkCatalogNode(parent, item, atPosition);
	} else if (itemType == NetworkLibraryBookItem::TYPE_ID) {
		return new NetworkBookInfoNode(parent, item);
	}
	return 0;
}

void NetworkNodesFactory::createSubnodes(SearchResultNode *parent, NetworkBookCollection &books) {
	const NetworkAuthorBooksMap &map = books.authorBooksMap();
	for (NetworkAuthorBooksMap::const_iterator it = map.begin(); it != map.end(); ++it) {
		fillAuthorNode(
			new NetworkAuthorNode(parent, it->first), 
			it->second
		);
	}
}

void NetworkNodesFactory::fillAuthorNode(NetworkAuthorNode *authorNode, const NetworkLibraryItemList &books) {
	NetworkSeriesNode *seriesNode = 0;
	for (NetworkLibraryItemList::const_iterator it = books.begin(); it != books.end(); ++it) {
		if ((*it)->typeId() != NetworkLibraryBookItem::TYPE_ID) {
			continue;
		}
		const NetworkLibraryBookItem &book = (const NetworkLibraryBookItem &) **it;
		const std::string &series = book.series();
		if (series.empty()) {
			seriesNode = 0;
			new NetworkBookInfoNode(authorNode, *it);
		} else {
			if (seriesNode == 0 || seriesNode->series() != series) {
				seriesNode = new NetworkSeriesNode(authorNode, series);
			}
			new NetworkBookInfoNode(seriesNode, *it);
		}
	}	
}
