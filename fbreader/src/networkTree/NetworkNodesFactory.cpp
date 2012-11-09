/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

FBReaderNode *NetworkNodesFactory::createNetworkNode(NetworkCatalogNode *parent, shared_ptr<NetworkItem> item, std::size_t atPosition) {
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
	NetworkAuthorNode *authorNode = 0;

	int flags = NetworkCatalogItem::FLAGS_DEFAULT;
	if (NetworkCatalogNode* catalogNode = zlobject_cast<NetworkCatalogNode*>(parent)) {
		flags = catalogNode->item().getFlags();
	}
	NetworkSeriesNode::SummaryType seriesSummaryType = NetworkSeriesNode::AUTHORS;
	NetworkBookNode::SummaryType booksSummaryType = NetworkBookNode::AUTHORS;
	if ((parent->isInstanceOf(NetworkCatalogNode::TYPE_ID) &&
			(flags & NetworkCatalogItem::FLAG_SHOW_AUTHOR) == 0) ||
			 parent->isInstanceOf(NetworkAuthorNode::TYPE_ID)) {
		seriesSummaryType = NetworkSeriesNode::BOOKS;
		booksSummaryType = NetworkBookNode::NONE;
	}

	for (NetworkItem::List::const_iterator it = books.begin(); it != books.end(); ++it) {
		if (!(*it)->isInstanceOf(NetworkBookItem::TYPE_ID)) {
			continue;
		}
		const NetworkBookItem &book = (const NetworkBookItem &) **it;

		//TODO split this method on smaller parts
		switch (flags & NetworkCatalogItem::FLAGS_GROUP) {
			case NetworkCatalogItem::FLAG_GROUP_BY_SERIES:
				if (book.SeriesTitle.empty()) {
					new NetworkBookNode(parent, *it, booksSummaryType);
				} else {
					if (seriesNode == 0 || seriesNode->title() != book.SeriesTitle) {
						seriesNode = new NetworkSeriesNode(parent, book.SeriesTitle, seriesSummaryType);
					}
					new NetworkBookNode(seriesNode, *it, booksSummaryType);
				}
				break;
			case NetworkCatalogItem::FLAG_GROUP_MORE_THAN_1_BOOK_BY_SERIES:
				{
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
				break;
			case NetworkCatalogItem::FLAG_GROUP_BY_AUTHOR:
				if (book.Authors.empty()) {
					new NetworkBookNode(parent, *it, booksSummaryType);
				} else {
					const NetworkBookItem::AuthorData &author = book.Authors.front();
					if (authorNode == 0 || authorNode->author() != author) {
						authorNode = new NetworkAuthorNode(parent, author);
					}
					new NetworkBookNode(authorNode, *it, booksSummaryType);
				}
				break;
			default:
				new NetworkBookNode(parent, *it, booksSummaryType);
				break;
		}


	}
}
