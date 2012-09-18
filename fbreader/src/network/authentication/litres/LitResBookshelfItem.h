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

#ifndef __LITRESBOOKSHELFITEM_H__
#define __LITRESBOOKSHELFITEM_H__

#include <ZLResource.h>

#include "../../NetworkComparators.h"
#include "../../NetworkItems.h"

class NetworkLink;

class SortedCatalogItem : public NetworkCatalogItem {

public:
	template <class T>
	SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource, const NetworkItem::List &children,
					  T comparator, CatalogType type = OTHER);
	template <class T, class F>
	SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource, const NetworkItem::List &children,
					  T comparator, CatalogType type, F filter);
	SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource,  const NetworkItem::List &children,
					  CatalogType type = OTHER);

public:
	std::string loadChildren(NetworkItem::List &children);
	bool isEmpty() const;
	 //TODO following method should be in class NetworkLibrary or smth like that
	static const ZLResource &resource(const std::string &resourceKey);

protected:
	bool accepts(NetworkItem* item) const;

private:
	void addChildren(const NetworkItem::List &children);

protected:
	NetworkItem::List myChildren;
};

//TODO does it necessary to create so many templates?
template <class T>
SortedCatalogItem::SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource,
									 const NetworkItem::List &children, T comparator, CatalogType type)
	: NetworkCatalogItem(parent.Link, resource.value(), resource["summary"].value(), parent.URLByType, Always, type) {
	addChildren(children);
	std::sort(myChildren.begin(), myChildren.end(), comparator);
}

template <class T, class F>
SortedCatalogItem::SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource, const NetworkItem::List &children,
									 T comparator, CatalogType type, F filter)
	: NetworkCatalogItem(parent.Link, resource.value(), resource["summary"].value(), parent.URLByType, Always, type) {
	for (size_t i = 0; i < children.size(); ++i) {
		shared_ptr<NetworkItem> child = children.at(i);
		if (filter.accepts(&(*child))) {
			myChildren.push_back(child);
		}
	}
	std::sort(myChildren.begin(), myChildren.end(), comparator);
}

class LitResBookshelfItem : public NetworkCatalogItem {

public:
	LitResBookshelfItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility = Always
	);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkItem::List &children);

private:
	bool myForceReload;
};

#endif /* __LITRESBOOKSHELFITEM_H__ */
