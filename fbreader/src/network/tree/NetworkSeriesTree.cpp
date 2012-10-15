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

#include <ZLResource.h>
#include <ZLImage.h>

#include "NetworkTreeNodes.h"

const ZLTypeId NetworkSeriesTree::TYPE_ID(NetworkTree::TYPE_ID);

const ZLTypeId &NetworkSeriesTree::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkSeriesTree::resource() const {
	return ZLResource::resource("networkView")["seriesNode"];
}

NetworkSeriesTree::NetworkSeriesTree(NetworkTree *parent, const std::string &seriesTitle, SummaryType summaryType) :
	NetworkTree(parent), mySeriesTitle(seriesTitle), mySummaryType(summaryType) {
	init();
}

void NetworkSeriesTree::init() {
	//registerExpandTreeAction();
}

std::string NetworkSeriesTree::title() const {
	return mySeriesTitle;
}

std::string NetworkSeriesTree::subtitle() const {
	if (mySummary.empty()) {
		if (mySummaryType == BOOKS) {
			mySummary = FBTree::subtitle();
		} else {
			std::set<NetworkBookItem::AuthorData> authorSet;
			const ZLTreeNode::List &books = children();
			for (ZLTreeNode::List::const_iterator it = books.begin(); it != books.end(); ++it) {
				const NetworkBookItem &book = ((NetworkBookTree*)*it)->book();
				const std::vector<NetworkBookItem::AuthorData> &authors = book.Authors;
				for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
					if (authorSet.find(*it) == authorSet.end()) {
						authorSet.insert(*it);
						if (!mySummary.empty()) {
							mySummary += ", ";
						}
						mySummary += it->DisplayName;
					}
				}
			}
		}
	}

	return mySummary;
}

shared_ptr<const ZLImage> NetworkSeriesTree::image() const {
	//TODO reimplement
//	const ZLTreeNode::List &books = children();
//	for (const ZLTreeNode::List::const_iterator it = books.begin(); it != books.end(); ++it) {
//		shared_ptr<const ZLImage> bookCover = ((FBTree*)*it)->coverImage();
//		if (!bookCover.isNull()) {
//			return bookCover;
//		}
//	}
	return defaultCoverImage("booktree-book.png");
}
