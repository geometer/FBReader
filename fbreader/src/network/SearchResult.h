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

#ifndef __SEARCHRESULT_H__
#define __SEARCHRESULT_H__

#include "NetworkBookCollection.h"

class SearchResult {

private:
	static SearchResult ourLastSearchResult;

private:
	SearchResult();

public:
	static const SearchResult &lastSearchResult();
	static void setLastSearchResult(const std::string &summary, shared_ptr<NetworkBookCollection> bookCollection);

public:
	const std::string &summary() const;
	shared_ptr<NetworkBookCollection> collection() const;

private:
	std::string mySummary;
	shared_ptr<NetworkBookCollection> myBookCollection;
};

inline const std::string &SearchResult::summary() const { return mySummary; }
inline shared_ptr<NetworkBookCollection> SearchResult::collection() const { return myBookCollection; }

#endif /* __SEARCHRESULT_H__ */
