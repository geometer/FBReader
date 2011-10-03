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

const ZLTypeId NetworkBookNode::TYPE_ID(ZLTreeActionNode::TYPE_ID);

const ZLTypeId &NetworkBookNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkBookNode::resource() const {
	return ZLResource::resource("networkView")["bookNode"];
}

NetworkBookNode::NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book, SummaryType summaryType) : myBook(book), mySummaryType(summaryType) {
	init();
	parent->append(this);
}

void NetworkBookNode::init() {
	const NetworkBookItem &book = this->book();
	if (!book.reference(BookReference::DOWNLOAD_FULL).isNull() ||
			!book.reference(BookReference::DOWNLOAD_FULL_CONDITIONAL).isNull()) {
		registerAction(new NetworkBookReadAction(this, book, false));
		registerAction(new NetworkBookDownloadAction(this, book, false));
		registerAction(new NetworkBookDeleteAction(book));
	}
	if (!book.reference(BookReference::DOWNLOAD_DEMO).isNull()) {
		registerAction(new NetworkBookReadAction(this, book, true));
		registerAction(new NetworkBookDownloadAction(this, book, true, resource()["demo"].value()));
	}
	if (!book.reference(BookReference::BUY).isNull()) {
		registerAction(new NetworkBookBuyDirectlyAction(book));
	} else if (!book.reference(BookReference::BUY_IN_BROWSER).isNull()) {
		registerAction(new NetworkBookBuyInBrowserAction(book));
	}
}

std::string NetworkBookNode::title() const {
	return myBook->Title;
}

std::string NetworkBookNode::subtitle() const {
	int count = 0;
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = book().Authors;
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if (!authorsString.empty()) {
			authorsString += ", ";
		}
		authorsString += it->DisplayName;
		++count;
	}
	if (mySummaryType == NONE && count == 1) {
		return std::string();
	}
	return authorsString;
}

bool NetworkBookNode::activate() {
	return false;
}

shared_ptr<ZLImage> NetworkBookNode::image() const {
//	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_COVER]);
//	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
	return shared_ptr<ZLImage>();
}

std::string NetworkBookNode::imageUrl() const {
	std::string url = myBook->URLByType[NetworkItem::URL_COVER];
	if (url.empty())
		url = FBNode::defaultImageUrl("booktree-book.png");
	return url;
}

const NetworkBookItem &NetworkBookNode::book() const {
	return (const NetworkBookItem&)*myBook;
}
