/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __SORTEDCATALOGITEM_H__
#define __SORTEDCATALOGITEM_H__

#include <algorithm>

#include <ZLResource.h>

#include "../NetworkComparators.h"
#include "../NetworkItems.h"

class SortedCatalogItem : public NetworkCatalogItem {

public:
	class BookItemFilter {
	public:
		bool accepts(NetworkItem* item) const;
	};

	class BySeriesFilter {
	public:
		bool accepts(NetworkItem* item) const;
	};

	//TODO maybe refactor (using templates is too complex for this simple case
	//(templates were used because in C++ virtual methods can't be called from constructor)
	template <class T, class F>
	static SortedCatalogItem* create(const NetworkCatalogItem &parent, const std::string &resourceKey,
					   const NetworkItem::List &children, int flags, T comparator, F filter);
	template <class T>
	static SortedCatalogItem* create(const NetworkCatalogItem &parent, const std::string &resourceKey,
					   const NetworkItem::List &children, int flags, T comparator);
	static SortedCatalogItem* create(const NetworkCatalogItem &parent, const std::string &resourceKey,
					   const NetworkItem::List &children, int flags);

public:
	SortedCatalogItem(const NetworkCatalogItem &parent, const ZLResource &resource, const NetworkItem::List &children, int flags);

public:
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener);
	bool isEmpty() const;
	 //TODO following method should be in class NetworkLibrary or smth like that
	static const ZLResource &resource(const std::string &resourceKey);

protected:
	NetworkItem::List myChildren;
};

template <class T, class F>
SortedCatalogItem* SortedCatalogItem::create(const NetworkCatalogItem &parent, const std::string &resourceKey,
				   const NetworkItem::List &children, int flags, T comparator, F filter) {
	NetworkItem::List tmpChildren;
	for (std::size_t i = 0; i < children.size(); ++i) {
		shared_ptr<NetworkItem> child = children.at(i);
		if (filter.accepts(&(*child))) {
			tmpChildren.push_back(child);
		}
	}
	std::sort(tmpChildren.begin(), tmpChildren.end(), comparator);
	return new SortedCatalogItem(parent, resource(resourceKey), tmpChildren, flags);
}

template <class T>
SortedCatalogItem* SortedCatalogItem::create(const NetworkCatalogItem &parent, const std::string &resourceKey,
				   const NetworkItem::List &children, int flags, T comparator) {
	return create(parent, resourceKey, children, flags, comparator, BookItemFilter());
}

inline SortedCatalogItem* SortedCatalogItem::create(const NetworkCatalogItem &parent, const std::string &resourceKey,
				   const NetworkItem::List &children, int flags) {
	BookItemFilter filter;
	NetworkItem::List tmpChildren;
	for (std::size_t i = 0; i < children.size(); ++i) {
		shared_ptr<NetworkItem> child = children.at(i);
		if (filter.accepts(&(*child))) {
			tmpChildren.push_back(child);
		}
	}
	return new SortedCatalogItem(parent, resource(resourceKey), tmpChildren, flags);
}

#endif /* __SORTEDCATALOGITEM_H__ */
