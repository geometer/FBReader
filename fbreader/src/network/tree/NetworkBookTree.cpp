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
#include "../../networkTree/NetworkCatalogUtil.h"

#include "../../networkActions/NetworkActions.h"

const ZLTypeId NetworkBookTree::TYPE_ID(FBTree::TYPE_ID);

const ZLTypeId &NetworkBookTree::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkBookTree::resource() const {
	return ZLResource::resource("networkView")["bookNode"];
}

NetworkBookTree::NetworkBookTree(NetworkTree *parent, shared_ptr<NetworkItem> book, SummaryType summaryType) : FBTree(parent), myBook(book), mySummaryType(summaryType) {
}

void NetworkBookTree::init() {
//	const NetworkBookItem &book = this->book();
//	if (!book.reference(BookReference::DOWNLOAD_FULL).isNull() ||
//			!book.reference(BookReference::DOWNLOAD_FULL_CONDITIONAL).isNull()) {
//		registerAction(new NetworkBookReadAction(book, false));
//		registerAction(new NetworkBookDownloadAction(book, false));
//		registerAction(new NetworkBookDeleteAction(book));
//	}
//	if (!book.reference(BookReference::DOWNLOAD_DEMO).isNull()) {
//		registerAction(new NetworkBookReadAction(book, true));
//		registerAction(new NetworkBookDownloadAction(book, true, resource()["demo"].value()));
//	}
//	if (!book.reference(BookReference::BUY).isNull()) {
//		registerAction(new NetworkBookBuyDirectlyAction(book));
//	} else if (!book.reference(BookReference::BUY_IN_BROWSER).isNull()) {
//		registerAction(new NetworkBookBuyInBrowserAction(book));
//	}
}

std::string NetworkBookTree::title() const {
	return myBook->Title;
}

std::string NetworkBookTree::subtitle() const {
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

shared_ptr<const ZLImage> NetworkBookTree::image() const {
	shared_ptr<const ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_COVER]);
	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
}

const NetworkBookItem &NetworkBookTree::book() const {
	return (const NetworkBookItem&)*myBook;
}
