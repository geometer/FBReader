/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

class BookCollection {

public:
	ZLStringOption PathOption;
	ZLBooleanOption ScanSubdirsOption;

public:
	BookCollection();
	~BookCollection();

	const std::vector<AuthorPtr> &authors() const;
	const Books &books() const;
	bool isBookExternal(BookDescriptionPtr description) const;

	void rebuild(bool strong);
	bool synchronize() const;

	void collectSeriesNames(AuthorPtr author, std::set<std::string> &list) const;
	void removeTag(const std::string &tag, bool includeSubTags);
	void renameTag(const std::string &from, const std::string &to, bool includeSubTags);
	void cloneTag(const std::string &from, const std::string &to, bool includeSubTags);
	void addTagToAllBooks(const std::string &to);
	void addTagToBooksWithNoTags(const std::string &to);
	bool hasBooks(const std::string &tag) const;
	bool hasSubtags(const std::string &tag) const;
	
private:
	void collectDirNames(std::set<std::string> &names) const;
	void collectBookFileNames(std::set<std::string> &bookFileNames) const;

	void addDescription(BookDescriptionPtr description) const;

private:
	mutable Books myBooks;
	mutable std::vector<AuthorPtr> myAuthors;
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

inline const Books &BookCollection::books() const {
	synchronize();
	return myBooks;
}

inline bool BookCollection::isBookExternal(BookDescriptionPtr description) const {
	synchronize();
	return myExternalBooks.find(description) != myExternalBooks.end();
}

inline const Books &LastOpenedBooks::books() const { return myBooks; }

#endif /* __BOOKCOLLECTION_H__ */
