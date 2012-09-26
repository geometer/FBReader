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

#include "NetworkTreeFactory.h"

#include "NetworkTreeNodes.h"

#include "../NetworkBookCollection.h"

NetworkTreeFactory::NetworkTreeFactory() {
}

FBTree *NetworkTreeFactory::createNetworkTree(NetworkCatalogTree *parent, shared_ptr<NetworkItem> item, size_t position) {
	if (item->isInstanceOf(NetworkCatalogItem::TYPE_ID)) {
		NetworkCatalogTree *ptr = new NetworkCatalogTree(parent, item, position);
		ptr->item().onDisplayItem();
		return ptr;
	} else if (item->isInstanceOf(NetworkBookItem::TYPE_ID)) {
		return new NetworkBookTree(parent, item, NetworkBookTree::AUTHORS);
	}
	return 0;
}

void NetworkTreeFactory::fillAuthorTree(NetworkTree *parent, const NetworkItem::List &books) {
	NetworkSeriesTree *seriesTree = 0;
	NetworkAuthorTree *authorTree = 0;

	int flags = NetworkCatalogItem::FLAGS_DEFAULT;
	if (NetworkCatalogTree* catalogTree = zlobject_cast<NetworkCatalogTree*>(parent)) {
		flags = catalogTree->item().getFlags();
	}
	NetworkSeriesTree::SummaryType seriesSummaryType = NetworkSeriesTree::AUTHORS;
	NetworkBookTree::SummaryType booksSummaryType = NetworkBookTree::AUTHORS;
	if ((parent->isInstanceOf(NetworkCatalogTree::TYPE_ID) &&
			(flags & NetworkCatalogItem::FLAG_SHOW_AUTHOR) == 0) ||
			 parent->isInstanceOf(NetworkAuthorTree::TYPE_ID)) {
		seriesSummaryType = NetworkSeriesTree::BOOKS;
		booksSummaryType = NetworkBookTree::NONE;
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
					new NetworkBookTree(parent, *it, booksSummaryType);
				} else {
					if (seriesTree == 0 || seriesTree->title() != book.SeriesTitle) {
						seriesTree = new NetworkSeriesTree(parent, book.SeriesTitle, seriesSummaryType);
					}
					new NetworkBookTree(seriesTree, *it, booksSummaryType);
				}
				break;
			case NetworkCatalogItem::FLAG_GROUP_MORE_THAN_1_BOOK_BY_SERIES:
				{
					std::string seriesTitle = book.SeriesTitle;
					if (!seriesTitle.empty() && (seriesTree == 0 || seriesTree->title() != seriesTitle)) {
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
						seriesTree = 0;
						new NetworkBookTree(parent, *it, booksSummaryType);
					} else {
						if (seriesTree == 0 || seriesTree->title() != seriesTitle) {
							seriesTree = new NetworkSeriesTree(parent, seriesTitle, seriesSummaryType);
						}
						new NetworkBookTree(seriesTree, *it, booksSummaryType);
					}
				}
				break;
			case NetworkCatalogItem::FLAG_GROUP_BY_AUTHOR:
				if (book.Authors.empty()) {
					new NetworkBookTree(parent, *it, booksSummaryType);
				} else {
					const NetworkBookItem::AuthorData &author = book.Authors.front();
					if (authorTree == 0 || authorTree->author() != author) {
						authorTree = new NetworkAuthorTree(parent, author);
					}
					new NetworkBookTree(authorTree, *it, booksSummaryType);
				}
				break;
			default:
				new NetworkBookTree(parent, *it, booksSummaryType);
				break;
		}


	}
}
