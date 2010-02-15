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

#ifndef __NETWORKBOOKCOLLECTION_H__
#define __NETWORKBOOKCOLLECTION_H__

#include "NetworkItems.h"

#include "NetworkComparators.h"


typedef std::map<NetworkBookItem::AuthorData, NetworkItem::List, NetworkAuthorComparator> NetworkAuthorBooksMap;


class NetworkBookCollection {

public:
	NetworkBookCollection();

public:
	void addBook(shared_ptr<NetworkItem> bookPtr);

	const NetworkItem::List &books() const;
	bool empty() const;

	const NetworkAuthorBooksMap &authorBooksMap();

private:
	NetworkItem::List myBookList;
	std::map<NetworkBookItem::AuthorData, unsigned int> myAuthorRates;
	shared_ptr<NetworkAuthorComparator> myAuthorComparator;
	shared_ptr<NetworkAuthorBooksMap> myAuthorBooksMap;

private: // disable copying
	NetworkBookCollection(const NetworkBookCollection &);
	const NetworkBookCollection &operator = (const NetworkBookCollection &);
};

inline const NetworkItem::List &NetworkBookCollection::books() const { return myBookList; }
inline bool NetworkBookCollection::empty() const { return myBookList.empty(); }

#endif /* __NETWORKBOOKCOLLECTION_H__ */
