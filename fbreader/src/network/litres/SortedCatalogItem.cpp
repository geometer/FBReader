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

#include "SortedCatalogItem.h"

bool SortedCatalogItem::BookItemFilter::accepts(NetworkItem* item) const {
	return zlobject_cast<NetworkBookItem*>(item) != 0;
}

bool SortedCatalogItem::BySeriesFilter::accepts(NetworkItem* item) const {
	NetworkBookItem* bookItem = zlobject_cast<NetworkBookItem*>(item);
	return bookItem != 0 && !bookItem->SeriesTitle.empty();
}

SortedCatalogItem::SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource,
									 const NetworkItem::List &children, int flags)
	: NetworkCatalogItem(parent.Link, resource.value(), resource["summary"].value(), parent.URLByType, AlWAYS, flags) {
	myChildren = children;
}

std::string SortedCatalogItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	children.assign(myChildren.begin(), myChildren.end());
	listener->finished();
	return std::string();
}

bool SortedCatalogItem::isEmpty() const {
	return myChildren.empty();
}

const ZLResource &SortedCatalogItem::resource(const std::string &resourceKey) {
	return ZLResource::resource("networkView")[resourceKey];
}
