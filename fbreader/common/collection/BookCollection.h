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

#include "../description/BookDescription.h"

typedef std::vector<BookDescriptionPtr > Books;

class DescriptionComparator {

public:
	DescriptionComparator() MODEL_SECTION;
	~DescriptionComparator() MODEL_SECTION;
	bool operator() (const BookDescriptionPtr d1, const BookDescriptionPtr d2) MODEL_SECTION;
};

class BookCollection {

public:
	static ZLStringOption PathOption;
	static ZLBooleanOption ScanSubdirsOption;

public:
	BookCollection() MODEL_SECTION;
	~BookCollection() MODEL_SECTION;

	const std::vector<const Author*> &authors() const MODEL_SECTION;
	const Books &books(const Author *author) MODEL_SECTION;

	bool isActual() const MODEL_SECTION;
	
private:
	void collectDirNames(std::set<std::string> &names) MODEL_SECTION;
	void addDescription(BookDescriptionPtr description) MODEL_SECTION;

private:
	std::vector<const Author*> myAuthors;
	std::map<const Author*,Books> myCollection;

	std::string myPath;
	bool myScanSubdirs;
};

class LastOpenedBooks {

public:
	static ZLIntegerOption MaxListSizeOption;

public:
	LastOpenedBooks() MODEL_SECTION;
	~LastOpenedBooks() MODEL_SECTION;
	void addBook(const std::string &fileName) MODEL_SECTION;
	const Books &books() const MODEL_SECTION;

private:
	Books myBooks;
};

inline DescriptionComparator::DescriptionComparator() {}
inline DescriptionComparator::~DescriptionComparator() {}

inline const std::vector<const Author*> &BookCollection::authors() const { return myAuthors; }
inline const Books &BookCollection::books(const Author *author) { return (*myCollection.find(author)).second; }

inline const Books &LastOpenedBooks::books() const { return myBooks; }

#endif /* __BOOKCOLLECTION_H__ */
