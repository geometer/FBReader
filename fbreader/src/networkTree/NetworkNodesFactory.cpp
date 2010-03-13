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

#include "NetworkNodesFactory.h"

#include "NetworkNodes.h"

#include "../network/NetworkBookCollection.h"

FBReaderNode *NetworkNodesFactory::createNetworkNode(NetworkCatalogNode *parent, shared_ptr<NetworkItem> item, size_t atPosition) {
	if (item->isInstanceOf(NetworkCatalogItem::TYPE_ID)) {
		NetworkCatalogNode *ptr = new NetworkCatalogNode(parent, item, atPosition);
		ptr->item().onDisplayItem();
		return ptr;
	} else if (item->isInstanceOf(NetworkBookItem::TYPE_ID)) {
		return new NetworkBookNode(parent, item, NetworkBookNode::AUTHORS);
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

void NetworkNodesFactory::fillAuthorNode(NetworkContainerNode *parent, const NetworkItem::List &books) {
	NetworkSeriesNode *seriesNode = 0;

	NetworkSeriesNode::SummaryType seriesSummaryType = NetworkSeriesNode::AUTHORS;
	NetworkBookNode::SummaryType booksSummaryType = NetworkBookNode::AUTHORS;
	if ((parent->isInstanceOf(NetworkCatalogNode::TYPE_ID) &&
			((NetworkCatalogNode*)parent)->item().Type == NetworkCatalogItem::BY_AUTHORS) ||
			 parent->isInstanceOf(NetworkAuthorNode::TYPE_ID)) {
		seriesSummaryType = NetworkSeriesNode::BOOKS;
		booksSummaryType = NetworkBookNode::NONE;
	}

	for (NetworkItem::List::const_iterator it = books.begin(); it != books.end(); ++it) {
		if (!(*it)->isInstanceOf(NetworkBookItem::TYPE_ID)) {
			continue;
		}
		const NetworkBookItem &book = (const NetworkBookItem &) **it;
		std::string seriesTitle = book.SeriesTitle;

		if (!seriesTitle.empty() && (seriesNode == 0 || seriesNode->title() != seriesTitle)) {
			NetworkItem::List::const_iterator jt = it + 1;
			while (jt != books.end() && !(*jt)->isInstanceOf(NetworkBookItem::TYPE_ID)) {
				++jt;
			}
			if (jt == books.end()) {
				seriesTitle.clear();
			} else {
				const NetworkBookItem &next = (const NetworkBookItem&)**jt;
				if (next.SeriesTitle != seriesTitle) {
					seriesTitle.clear();
				}
			}
		}

		if (seriesTitle.empty()) {
			seriesNode = 0;
			new NetworkBookNode(parent, *it, booksSummaryType);
		} else {
			if (seriesNode == 0 || seriesNode->title() != seriesTitle) {
				seriesNode = new NetworkSeriesNode(parent, seriesTitle, seriesSummaryType);
			}
			new NetworkBookNode(seriesNode, *it, booksSummaryType);
		}
	}	
}
