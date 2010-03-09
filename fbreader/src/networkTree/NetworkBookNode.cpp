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
#include "NetworkCatalogUtil.h"

#include "../networkActions/NetworkActions.h"

const ZLTypeId NetworkBookNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &NetworkBookNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkBookNode::resource() const {
	return ZLResource::resource("networkView")["bookNode"];
}

NetworkBookNode::NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book) : FBReaderNode(parent), myBook(book) {
	const NetworkBookItem &item = this->book();
	myReadAction = new NetworkBookReadAction(item, false);
	myDownloadAction = new NetworkBookDownloadAction(item, false);
	myReadDemoAction = new NetworkBookReadAction(item, true);
	myDownloadDemoAction = new NetworkBookDownloadAction(item, true);
	myBuyAction = new NetworkBookBuyAction(item);
	myDeleteAction = new NetworkBookDeleteAction(item);
}

std::string NetworkBookNode::title() const {
	return myBook->Title;
}

std::string NetworkBookNode::summary() const {
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = book().Authors;
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if (!authorsString.empty()) {
			authorsString += ", ";
		}
		authorsString += it->DisplayName;
	}
	FBReaderNode *parent = (FBReaderNode*)this->parent();
	if (parent->title() == authorsString) {
		return std::string();
	}
	return authorsString;
}

void NetworkBookNode::paint(ZLPaintContext &context, int vOffset) {
	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);

	drawTitle(context, vOffset);
	drawSummary(context, vOffset);

	int left = 0;
	drawHyperlink(context, left, vOffset, myReadAction);
	drawHyperlink(context, left, vOffset, myDeleteAction);
	drawHyperlink(context, left, vOffset, myDownloadAction);
	drawHyperlink(context, left, vOffset, myReadDemoAction);
	drawHyperlink(context, left, vOffset, myDownloadDemoAction);
	drawHyperlink(context, left, vOffset, myBuyAction);
}

shared_ptr<ZLImage> NetworkBookNode::extractCoverImage() const {
	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_COVER]);
	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
}

const NetworkBookItem &NetworkBookNode::book() const {
	return (const NetworkBookItem&)*myBook;
}
