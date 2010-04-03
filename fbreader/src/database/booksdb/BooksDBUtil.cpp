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

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLStringUtil.h>

#include "BooksDBUtil.h"
#include "BooksDB.h"

#include "../../library/Book.h"
#include "../../library/Tag.h"
#include "../../library/Author.h"

shared_ptr<Book> BooksDBUtil::getBook(const std::string &filePath, bool checkFile) {
	const std::string physicalFilePath = ZLFile(filePath).physicalFilePath();

	ZLFile file(physicalFilePath);
	if (checkFile && !file.exists()) {
		return 0;
	}

	if (!checkFile || checkInfo(file)) {
		shared_ptr<Book> book = loadFromDB(filePath);
		if (!book.isNull() && isBookFull(*book)) {
			return book;
		}
	} else {
		if (physicalFilePath != filePath) {
			resetZipInfo(file);
		}
		saveInfo(file);
	}

	shared_ptr<Book> book = Book::loadFromFile(filePath);
	if (book.isNull()) {
		return 0;
	}
	BooksDB::Instance().saveBook(book);
	return book;
}

bool BooksDBUtil::getRecentBooks(BookList &books) {
	std::vector<std::string> fileNames;
	if (!BooksDB::Instance().loadRecentBooks(fileNames)) {
		return false;
	}
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		shared_ptr<Book> book = getBook(*it /*, true OR false ? */); // TODO: check file ???
		if (!book.isNull()) {
			books.push_back(book);
		}
	}
	return true;
}

bool BooksDBUtil::getBooks(std::map<std::string, shared_ptr<Book> > &booksmap, bool checkFile) {
	BookList books;
	if (!BooksDB::Instance().loadBooks(books)) {
		return false;
	}
	for (BookList::iterator it = books.begin(); it != books.end(); ++it) {
		Book &book = **it;
		const std::string physicalFilePath = book.file().physicalFilePath();
		ZLFile file(physicalFilePath);
		if (!checkFile || file.exists()) {
			if (!checkFile || checkInfo(file)) {
				if (isBookFull(book)) {
					booksmap.insert(std::make_pair(book.file().path(), *it));
					continue;
				}
			} else {
				if (physicalFilePath != book.file().path()) {
					resetZipInfo(file);
				}
				saveInfo(file);
			}
			shared_ptr<Book> bookptr = Book::loadFromFile(book.file().path());
			if (!bookptr.isNull()) {
				BooksDB::Instance().saveBook(bookptr);
				booksmap.insert(std::make_pair(book.file().path(), bookptr));
			}
		}
	}
	return true;
}

bool BooksDBUtil::isBookFull(const Book &book) {
	return
		!book.title().empty() &&
		!book.encoding().empty();
}

shared_ptr<Book> BooksDBUtil::loadFromDB(const std::string &filePath) {
	if (filePath.empty()) {
		return 0;
	}
	return BooksDB::Instance().loadBook(filePath);
}

bool BooksDBUtil::checkInfo(const ZLFile &file) {
	return BooksDB::Instance().getFileSize(file.path()) == (int) file.size();
}

void BooksDBUtil::saveInfo(const ZLFile &file) {
	BooksDB::Instance().setFileSize(file.path(), file.size());
}

void BooksDBUtil::listZipEntries(const ZLFile &zipFile, std::vector<std::string> &entries) {
	entries.clear();
	BooksDB::Instance().loadFileEntries(zipFile.path(), entries);
	if (entries.empty()) {
		resetZipInfo(zipFile);
		BooksDB::Instance().loadFileEntries(zipFile.path(), entries);
	}
}

void BooksDBUtil::resetZipInfo(const ZLFile &zipFile) {
	shared_ptr<ZLDir> zipDir = zipFile.directory();
	if (!zipDir.isNull()) {
		std::vector<std::string> entries;
		zipDir->collectFiles(entries, false);
		BooksDB::Instance().saveFileEntries(zipFile.path(), entries);
	}
}

bool BooksDBUtil::canRemoveFile(const std::string &filePath) {
	ZLFile bookFile(filePath);
	std::string physicalPath = bookFile.physicalFilePath();
	if (filePath != physicalPath) {
		ZLFile zipFile(physicalPath);
		shared_ptr<ZLDir> zipDir = zipFile.directory();
		if (zipDir.isNull()) {
			return false;
		}
		std::vector<std::string> entries;
		zipDir->collectFiles(entries, false); // TODO: replace with BooksDB call???
		if (entries.size() != 1) {
			return false;
		}
		if (zipDir->itemPath(entries[0]) != filePath) {
			return false;
		}
	}
	return ZLFile(physicalPath).canRemove();
}

void BooksDBUtil::addTag(shared_ptr<Book> book, shared_ptr<Tag> tag) {
	if (book->addTag(tag)) {
		BooksDB::Instance().saveTags(book);
	}
}

void BooksDBUtil::renameTag(shared_ptr<Book> book, shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags) {
	if (book->renameTag(from, to, includeSubTags)) {
		BooksDB::Instance().saveTags(book);
	}
}

void BooksDBUtil::cloneTag(shared_ptr<Book> book, shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags) {
	if (book->cloneTag(from, to, includeSubTags)) {
		BooksDB::Instance().saveTags(book);
	}
}

void BooksDBUtil::removeAllTags(shared_ptr<Book> book) {
	book->removeAllTags();
}
