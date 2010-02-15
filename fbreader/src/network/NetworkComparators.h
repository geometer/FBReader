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

#ifndef __NETWORKCOMPARATORS_H__
#define __NETWORKCOMPARATORS_H__


#include <map>

#include "NetworkItems.h"


class NetworkBookItemComparator {

public:
	bool operator () (const shared_ptr<NetworkItem> &bookPtr0, const shared_ptr<NetworkItem> &bookPtr1);
};


class NetworkAuthorComparator {

public:
	NetworkAuthorComparator(const std::map<NetworkBookItem::AuthorData, unsigned int> &rates);

	bool operator () (const NetworkBookItem::AuthorData &author0, const NetworkBookItem::AuthorData &author1);

private:
	const std::map<NetworkBookItem::AuthorData, unsigned int> &myRates;
};


#endif /* __NETWORKCOMPARATORS_H__ */
