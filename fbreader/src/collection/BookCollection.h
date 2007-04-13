/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __BOOKCOLLECTION_H__
#define __BOOKCOLLECTION_H__

#include <string>
#include <vector>
#include <set>
#include <map>

#include <ZLOptions.h>

#include "../description/BookDescription.h"

typedef std::vector<BookDescriptionPtr> Books;

class DescriptionComparator {

public:
	DescriptionComparator();
	~DescriptionComparator();
	bool operator() (const BookDescriptionPtr d1, const BookDescriptionPtr d2);
};

class BookCollection {

public:
	ZLStringOption PathOption;
	ZLBooleanOption ScanSubdirsOption;

public:
	BookCollection();
	~BookCollection();

	const std::vector<AuthorPtr> &authors() const;
	const Books &books(AuthorPtr author) const;
	bool isBookExternal(BookDescriptionPtr description) const;

	void rebuild(bool strong);
	bool synchronize() const;
	
private:

	void collectDirNames(std::set<std::string> &names) const;
	void collectBookFileNames(std::set<std::string> &bookFileNames) const;

	void addDescription(BookDescriptionPtr description) const;

private:
	mutable std::vector<AuthorPtr> myAuthors;
	mutable std::map<AuthorPtr,Books> myCollection;
	mutable std::set<BookDescriptionPtr> myExternalBooks;

	mutable std::string myPath;
	mutable bool myScanSubdirs;
	mutable bool myDoStrongRebuild;
	mutable bool myDoWeakRebuild;
};

class LastOpenedBooks {

public:
	ZLIntegerRangeOption MaxListSizeOption;

public:
	LastOpenedBooks();
	~LastOpenedBooks();
	void addBook(const std::string &fileName);
	const Books &books() const;

private:
	Books myBooks;
};

inline DescriptionComparator::DescriptionComparator() {}
inline DescriptionComparator::~DescriptionComparator() {}

inline const std::vector<AuthorPtr > &BookCollection::authors() const {
	synchronize();
	return myAuthors;
}

inline const Books &BookCollection::books(AuthorPtr author) const {
	synchronize();
	return (*myCollection.find(author)).second;
}

inline bool BookCollection::isBookExternal(BookDescriptionPtr description) const {
	synchronize();
	return myExternalBooks.find(description) != myExternalBooks.end();
}

inline const Books &LastOpenedBooks::books() const { return myBooks; }

#endif /* __BOOKCOLLECTION_H__ */
