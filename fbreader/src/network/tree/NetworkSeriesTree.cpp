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

#include <set>
#include <algorithm>

#include <ZLResource.h>
#include <ZLImage.h>
#include <ZLStringUtil.h>

#include "NetworkCatalogUtil.h"
#include "NetworkTreeNodes.h"

const ZLTypeId NetworkSeriesTree::TYPE_ID(NetworkTree::TYPE_ID);

const ZLTypeId &NetworkSeriesTree::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkSeriesTree::resource() const {
	return ZLResource::resource("networkView")["seriesNode"];
}

NetworkSeriesTree::NetworkSeriesTree(NetworkTree *parent, const std::string &seriesTitle) :
	NetworkTree(parent), mySeriesTitle(seriesTitle) {
	init();
}

void NetworkSeriesTree::init() {
	//registerExpandTreeAction();
}

std::string NetworkSeriesTree::title() const {
	return mySeriesTitle;
}

std::string NetworkSeriesTree::subtitle() const {
	return ZLStringUtil::printf(resource()["booksCount"].value((int)children().size()), (unsigned int)children().size());
}

static const size_t MAX_BATCH_SIZE = 6;

shared_ptr<const ZLImage> NetworkSeriesTree::image() const {
	if (myImages.empty()) {
		for (size_t i = 0; i < std::min(children().size(), MAX_BATCH_SIZE); ++i) {
			NetworkBookTree *bookTree = zlobject_cast<NetworkBookTree*>(children().at(i));
			if (!bookTree) {
				continue;
			}
			NetworkItem::UrlInfoCollection urlByType = bookTree->book().URLByType;
			std::string url = urlByType[NetworkItem::URL_COVER];
			myImages.push_back(NetworkCatalogUtil::getAndDownloadImageByUrl(url, this));
		}
	}
	return ZLImageManager::Instance().makeBatchImage(myImages, FBTree::defaultCoverImage("booktree-book.png"));
}
