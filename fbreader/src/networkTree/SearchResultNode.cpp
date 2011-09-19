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

#include <ZLStringUtil.h>

#include <ZLResource.h>
#include <ZLImage.h>

#include "NetworkNodes.h"
#include "NetworkView.h"

#include "../network/NetworkBookCollection.h"


const ZLTypeId SearchResultNode::TYPE_ID(NetworkContainerNode::TYPE_ID);

SearchResultNode::SearchResultNode(ZLTreeListener::RootNode *parent, shared_ptr<NetworkBookCollection> searchResult, const std::string &summary, size_t atPosition) : 
	mySearchResult(searchResult), 
	mySummary(summary) {
	parent->insert(this, atPosition);
}

void SearchResultNode::init() {
//	registerExpandTreeAction();
}

const ZLTypeId &SearchResultNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &SearchResultNode::resource() const {
	return ZLResource::resource("networkView")["searchResultNode"];
}

std::string SearchResultNode::title() const {
	return resource()["title"].value();
}

std::string SearchResultNode::summary() const {
	return mySummary;
}

shared_ptr<ZLImage> SearchResultNode::image() const {
	return FBNode::defaultCoverImage("searchresult.png");
}

std::string SearchResultNode::imageUrl() const {
	return FBNode::defaultImageUrl("searchresult.png");
}

shared_ptr<NetworkBookCollection> SearchResultNode::searchResult() { 
	return mySearchResult;
}
