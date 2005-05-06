/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __BOOKCOLLECTION_H__
#define __BOOKCOLLECTION_H__

#include <string>
#include <vector>
#include <set>
#include <map>

#include <abstract/ZLOptions.h>

class Author;
class BookDescription;

class DescriptionComparator {

public:
	DescriptionComparator() MODEL_SECTION;
	~DescriptionComparator() MODEL_SECTION;
	bool operator() (const BookDescription *d1, const BookDescription *d2) MODEL_SECTION;
};

class BookCollection {

public:
	static ZLStringOption PathOption;
	static ZLBooleanOption ScanSubdirsOption;

public:
	BookCollection() MODEL_SECTION;
	~BookCollection() MODEL_SECTION;

	const std::vector<const Author*> &authors() const MODEL_SECTION;
	const std::vector<BookDescription*> &books(const Author *author) MODEL_SECTION;

	bool isActual() const MODEL_SECTION;

	void forget(BookDescription *description) MODEL_SECTION;
	
private:
	void collectDirNames(std::set<std::string> &names) MODEL_SECTION;
	void addDescription(BookDescription *description) MODEL_SECTION;

private:
	typedef std::vector<BookDescription*> Books;

	std::vector<const Author*> myAuthors;
	std::map<const Author*,Books> myCollection;

	std::string myPath;
	bool myScanSubdirs;
	BookDescription *myForgottenBook;
};

inline DescriptionComparator::DescriptionComparator() {}
inline DescriptionComparator::~DescriptionComparator() {}

inline const std::vector<const Author*> &BookCollection::authors() const { return myAuthors; }
inline const std::vector<BookDescription*> &BookCollection::books(const Author *author) { return (*myCollection.find(author)).second; }
inline void BookCollection::forget(BookDescription *description) { myForgottenBook = description; }

#endif /* __BOOKCOLLECTION_H__ */
