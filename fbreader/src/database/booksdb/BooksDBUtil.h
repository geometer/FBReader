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

#ifndef __BOOKSDBUTIL_H__
#define __BOOKSDBUTIL_H__

#include <map>

#include <shared_ptr.h>

#include "../../library/Lists.h"

class Book;
class ZLFile;

class BooksDBUtil {

public:
	static shared_ptr<Book> getBook(const std::string &fileName, bool checkFile = true);

	static bool getBooks(std::map<std::string, shared_ptr<Book> > &booksmap, bool checkFile = true);

	static bool getRecentBooks(BookList &books);

public:
	static void addTag(shared_ptr<Book> book, shared_ptr<Tag> tag);
	static void renameTag(shared_ptr<Book> book, shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags);
	static void cloneTag(shared_ptr<Book> book, shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags);
	static void removeAllTags(shared_ptr<Book> book);

	static bool checkInfo(const ZLFile &file);
	static void saveInfo(const ZLFile &file);

	static void listZipEntries(const ZLFile &zipFile, std::vector<std::string> &entries);
	static void resetZipInfo(const ZLFile &zipFile);

	static bool isBookFull(const Book &book);

	static bool canRemoveFile(const std::string &fileName);

private:
	static shared_ptr<Book> loadFromDB(const std::string &fileName);
};

#endif /* __BOOKSDBUTIL_H__ */
