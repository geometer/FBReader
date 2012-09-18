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

#include <algorithm>

#include <ZLExecutionData.h>

#include "LitResBookshelfItem.h"
#include "LitResAuthenticationManager.h"

#include "../../NetworkLink.h"
#include "../../NetworkComparators.h"
#include "../../NetworkErrors.h"

SortedCatalogItem::SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource,
									 const NetworkItem::List &children, CatalogType type)
	: NetworkCatalogItem(parent.Link, resource.value(), resource["summary"].value(), parent.URLByType, Always, type) {
	addChildren(children);
}

bool SortedCatalogItem::accepts(NetworkItem* item) const {
	return zlobject_cast<NetworkBookItem*>(item) != 0;
}

void SortedCatalogItem::addChildren(const NetworkItem::List &children) {
	for (size_t i = 0; i < children.size(); ++i) {
		shared_ptr<NetworkItem> child = children.at(i);
		if (accepts(&(*child))) {
			myChildren.push_back(child);
		}
	}
}

std::string SortedCatalogItem::loadChildren(NetworkItem::List &children) {
	children.assign(myChildren.begin(), myChildren.end());
	return std::string();
}

bool SortedCatalogItem::isEmpty() const {
	return myChildren.empty();
}

const ZLResource &SortedCatalogItem::resource(const std::string &resourceKey) {
	return ZLResource::resource("networkView")[resourceKey];
}

class BySeriesFilter {

public:
	bool accepts(NetworkItem* item) const {
		NetworkBookItem* bookItem = zlobject_cast<NetworkBookItem*>(item);
		return bookItem != 0 && !bookItem->SeriesTitle.empty();
	}
};

LitResBookshelfItem::LitResBookshelfItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const std::map<URLType,std::string> &urlByType,
	VisibilityType visibility
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	visibility
) {
	myForceReload = false;
}

void LitResBookshelfItem::onDisplayItem() {
	myForceReload = false;
}

std::string LitResBookshelfItem::loadChildren(NetworkItem::List &children) {
	LitResAuthenticationManager &mgr =
		(LitResAuthenticationManager&)*Link.authenticationManager();
	if (mgr.isAuthorised().Status == B3_FALSE) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	std::string error;
	if (myForceReload) {
		error = mgr.reloadPurchasedBooks();
	}
	myForceReload = true;

	NetworkItem::List tmpChildren;
	mgr.collectPurchasedBooks(tmpChildren);

	if (tmpChildren.size() <= 5) {
		children.assign(tmpChildren.begin(), tmpChildren.end());
		std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	} else {
		children.push_back(new SortedCatalogItem(*this, SortedCatalogItem::resource("byDate"), tmpChildren));
		children.push_back(new SortedCatalogItem(*this, SortedCatalogItem::resource("byAuthor"), tmpChildren,
												 NetworkBookItemComparator()));
		children.push_back(new SortedCatalogItem(*this, SortedCatalogItem::resource("byTitle"), tmpChildren,
												 NetworkBookItemByTitleComparator()));
		SortedCatalogItem* bySeries = new SortedCatalogItem(*this, SortedCatalogItem::resource("bySeries"), tmpChildren,
															NetworkBookItemBySeriesComparator(), OTHER, BySeriesFilter());
		if (!bySeries->isEmpty()) {
			children.push_back(bySeries);
		} else {
			delete bySeries;
		}
	}
	return error;
}
