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

#include <ZLResource.h>
#include <ZLImage.h>

#include "NetworkNodes.h"
#include "NetworkView.h"

const ZLTypeId NetworkSeriesNode::TYPE_ID(NetworkContainerNode::TYPE_ID);

const ZLTypeId &NetworkSeriesNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkSeriesNode::resource() const {
	return ZLResource::resource("networkView")["seriesNode"];
}

NetworkSeriesNode::NetworkSeriesNode(NetworkContainerNode *parent, const std::string &seriesTitle, SummaryType summaryType) : 
	mySeriesTitle(seriesTitle), mySummaryType(summaryType) {
	parent->append(this);
}

void NetworkSeriesNode::init() {
//	registerExpandTreeAction();
}

std::string NetworkSeriesNode::title() const {
	return mySeriesTitle;
}

std::string NetworkSeriesNode::subtitle() const {
	if (mySummary.empty()) {
		if (mySummaryType == BOOKS) {
			mySummary = NetworkContainerNode::subtitle();
		} else {
			std::set<NetworkBookItem::AuthorData> authorSet;
			const std::vector<ZLTreeNode*> &books = children();
			for (std::vector<ZLTreeNode*>::const_iterator it = books.begin(); it != books.end(); ++it) {
				const NetworkBookItem &book = ((NetworkBookNode*)*it)->book();
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

shared_ptr<ZLImage> NetworkSeriesNode::image() const {
	const std::vector<ZLTreeNode*> &books = children();
	for (std::vector<ZLTreeNode*>::const_iterator it = books.begin(); it != books.end(); ++it) {
		shared_ptr<ZLImage> bookCover = ((ZLTreeTitledNode*)*it)->image();
		if (!bookCover.isNull()) {
			return bookCover;
		}
	}
	return FBNode::defaultCoverImage("booktree-book.png");
}

std::string NetworkSeriesNode::imageUrl() const {
//	const std::vector<ZLTreeNode*> &books = children();
//	for (std::vector<ZLTreeNode*>::const_iterator it = books.begin(); it != books.end(); ++it) {
//		shared_ptr<ZLImage> bookCover = ((FBReaderNode*)*it)->image();
//		if (!bookCover.isNull()) {
//			return bookCover;
//		}
//	}
	return FBNode::defaultImageUrl("booktree-book.png");
}
