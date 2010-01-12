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

#include <iostream>
#include <iomanip>
#include <ZLTime.h>

#include <vector>
#include <algorithm>

#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "BookInfo.h"
#include "Migration.h"
#include "../options/FBCategoryKey.h"

#include "../formats/FormatPlugin.h"

#include "../database/booksdb/BooksDBUtil.h"
#include "../database/booksdb/BooksDB.h"
#include "../library/Book.h"
#include "../library/Tag.h"

static const std::string BOOK_LIST_GROUP = "BookList";
static const std::string BOOK_LIST_SIZE = "Size";
static const std::string BOOK_LIST_PREFIX = "Book";

static const std::string CURRENT_STATE_GROUP = "State";

static const std::string RECENT_BOOKS_GROUP = "LastOpenedBooks";
static const std::string BOOK = "Book";
static const size_t MaxXmlListSize = 10;

static const std::string PARAGRAPH_OPTION_NAME = "Paragraph";
static const std::string WORD_OPTION_NAME = "Word";
static const std::string CHAR_OPTION_NAME = "Char";
static const std::string POSITION_IN_BUFFER = "PositionInBuffer";
static const std::string BUFFER_SIZE = "UndoBufferSize";
static const char * const BUFFER_PARAGRAPH_PREFIX = "Paragraph_";
static const char * const BUFFER_WORD_PREFIX = "Word_";
static const int MaxXmlStackSize = 20;

static const std::string SIZE = "Size";
static const std::string ENTRIES_NUMBER = "EntriesNumber";
static const std::string PALM_TYPE = "PalmType";

static const std::string NET_FILES_GROUP = "Files";

class Migration_0_11_0_Runnable : public DBRunnable {

public:
	bool run();

private:
	bool migrateBooks();
	bool migrateBookList();
	bool migrateState();
	bool migrateNetwork();

	bool migrateBook(const std::string &fileName);

	std::string tags2string(const TagList &tags);

	bool stringEquals(const std::string &tags1, const std::string &tags2);

	bool migrateRecentBooks();

	bool migrateBooksState();

	bool migrateBookStateStack(const std::string &fileName, const Book &book);
	bool migrateBookLastState(const std::string &fileName, const Book &book);

	bool shouldReadDisk(const std::string &fileName);

	bool clearBooksOptions();

	static void moveOption(const ZLCategoryKey &category, const std::string &oldGroup, const std::string &newGroup, const std::string &name, const std::string &defaultValue);
	static void moveOption(const ZLCategoryKey &category, const std::string &oldGroup, const std::string &newGroup, const std::string &name, int defaultValue);
	static void movePercentGroups(std::vector<std::string> &optionGroups);
	static void moveBookGroup(const std::string &oldgroup, const std::string &newgroup);

private:
	std::map<std::string, shared_ptr<Book> > myBooks;
};


inline bool Migration_0_11_0_Runnable::shouldReadDisk(const std::string &fileName) {
	const std::string ext = ZLFile(fileName).extension();
	return ext == "fb2" || ext == "epub" || ext == "mobi" || ext == "oebzip" || ext == "opf";
	//return ext == "fb2";
	//return true;
}


void Migration_0_11_0_Runnable::moveOption(const ZLCategoryKey &category, const std::string &oldGroup, const std::string &newGroup, const std::string &name, const std::string &defaultValue) {
	ZLStringOption newOption(category, newGroup, name, defaultValue);
	ZLStringOption oldOption(category, oldGroup, name, defaultValue);
	newOption.setValue(oldOption.value());
	oldOption.setValue(defaultValue);
}

void Migration_0_11_0_Runnable::moveOption(const ZLCategoryKey &category, const std::string &oldGroup, const std::string &newGroup, const std::string &name, int defaultValue) {
	ZLIntegerOption newOption(category, newGroup, name, defaultValue);
	ZLIntegerOption oldOption(category, oldGroup, name, defaultValue);
	newOption.setValue(oldOption.value());
	oldOption.setValue(defaultValue);
}

static bool percentPathPredicate(const std::string &path) {
	static const std::string _start = "%APPLICATION_PATH%";
	return ZLStringUtil::stringStartsWith(path, _start);
}

void Migration_0_11_0_Runnable::movePercentGroups(std::vector<std::string> &optionGroups) {
	std::vector<std::string>::iterator it = optionGroups.begin();
	while ((it = std::find_if(it, optionGroups.end(), percentPathPredicate)) != optionGroups.end()) {
		const std::string oldgroup = *it;
		const std::string newgroup = ZLFile(oldgroup).resolvedPath();
		if (std::find(optionGroups.begin(), optionGroups.end(), newgroup) == optionGroups.end()) {
			moveBookGroup(oldgroup, newgroup);
			*it++ = newgroup;
		} else {
			if (BookInfo(newgroup).TitleOption.value().empty()) {
				moveBookGroup(oldgroup, newgroup);
			}
			it = optionGroups.erase(it);
		}
		ZLOption::clearGroup(oldgroup);
	}
}

void Migration_0_11_0_Runnable::moveBookGroup(const std::string &oldgroup, const std::string &newgroup) {
	BookInfo oldbi(oldgroup);
	BookInfo newbi(newgroup);
	newbi = oldbi;
	oldbi.reset();

	moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, PARAGRAPH_OPTION_NAME, 0);
	moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, WORD_OPTION_NAME, 0);
	moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, CHAR_OPTION_NAME, 0);
	moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, POSITION_IN_BUFFER, 0);

	int stackSize = ZLIntegerOption(ZLCategoryKey::STATE, oldgroup, BUFFER_SIZE, 0).value();
	for (int i = 0; i < stackSize; ++i) {
		std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
		std::string bufferWord = BUFFER_WORD_PREFIX;
		ZLStringUtil::appendNumber(bufferParagraph, i);
		ZLStringUtil::appendNumber(bufferWord, i);
		moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, bufferParagraph, -1);
		moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, bufferWord, -1);
	}	
	moveOption(ZLCategoryKey::STATE, oldgroup, newgroup, BUFFER_SIZE, 0);
}



Migration_0_11_0::Migration_0_11_0() : Migration("0.11.0") {
}

void Migration_0_11_0::doMigrationInternal() {
	Migration_0_11_0_Runnable r;
	BooksDB::Instance().executeAsTransaction(r);
	//r.run();
}

bool Migration_0_11_0_Runnable::run() {
const ZLTime start;
	if (!migrateBooks()) {
		std::cerr << std::endl << "VERDICT: migrateBooks failed" << std::endl << std::endl;
	}
std::cerr << "migration total 0: " << ZLTime().millisecondsFrom(start) << "ms" << std::endl;
	if (!migrateBookList()) {
		std::cerr << std::endl << "VERDICT: migrateBookList failed" << std::endl << std::endl;
	}
std::cerr << "migration total 1: " << ZLTime().millisecondsFrom(start) << "ms" << std::endl;
	if (!migrateState()) {
		std::cerr << std::endl << "VERDICT: migrateState failed" << std::endl << std::endl;
	}
std::cerr << "migration total 2: " << ZLTime().millisecondsFrom(start) << "ms" << std::endl;
	if (!migrateNetwork()) {
		std::cerr << std::endl << "VERDICT: migrateNetwork failed" << std::endl << std::endl;
	}
std::cerr << "migration total 3: " << ZLTime().millisecondsFrom(start) << "ms" << std::endl;
	if (!clearBooksOptions()) {
		std::cerr << std::endl << "VERDICT: clearBooksOptions failed" << std::endl << std::endl;
	}
std::cerr << "migration total 4: " << ZLTime().millisecondsFrom(start) << "ms" << std::endl;
	return true;
}



bool Migration_0_11_0_Runnable::migrateBooks() {
	/*std::map<std::string, unsigned long> ext2time;
	std::map<std::string, unsigned long> ext2num;
	unsigned long totalTime = 0, totalNum = 0;*/

	PluginCollection &collection = PluginCollection::Instance();
	std::vector<std::string> optionGroups;
	ZLOption::listOptionGroups(optionGroups);

	movePercentGroups(optionGroups);

	bool res = true;
	for (std::vector<std::string>::const_iterator it = optionGroups.begin(); it != optionGroups.end(); ++it) {
		const std::string &name = *it;
		if (Migration::isLikeToFileName(name)) {
			/* TODO: check correctness of migration order: 
			 *   1) palmType
			 *   2) size
			 *   3) book (depends on palmType and size)
			 */
			const std::string palmType = ZLStringOption(FBCategoryKey::BOOKS, name, PALM_TYPE, "").value();
			if (!palmType.empty()) {
				BooksDB::Instance().setPalmType(name, palmType);
			}
			ZLStringOption(FBCategoryKey::BOOKS, name, PALM_TYPE, "").setValue(""); // clean books.xml
			ZLFile file(name);
			if (file.physicalFilePath() == name) {
				int size = ZLIntegerOption(FBCategoryKey::BOOKS, name, SIZE, -1).value();
				if (size != -1) {
					BooksDB::Instance().setFileSize(name, size);
				}
			}
			if (collection.plugin(file, false) != 0) {
				if (!BookInfo(name).TitleOption.value().empty()) {
					//ZLTime start;
					if (!migrateBook(name)) {
						std::cerr << "ERROR(2): migrateBook failed" << std::endl;
						res = false;
					}
					/*ZLTime end;
					{
						unsigned time = end.millisecondsFrom(start);
						std::string ext = ZLFile(name).extension();
						totalTime += time;
						totalNum += 1;
						ext2time[ext] += time;
						ext2num[ext] += 1;
					}*/
				}
				BookInfo(name).reset(); // clean books.xml
			} else {
				ZLOption::clearGroup(name); // clean books.xml
			}
			ZLIntegerOption(FBCategoryKey::BOOKS, name, SIZE, -1).setValue(-1);  // clean books.xml
			if (!ZLStringOption(FBCategoryKey::BOOKS, name, ENTRIES_NUMBER, "").value().empty()) {
				ZLOption::clearGroup(name); // clean books.xml
			}
		}
	}

	/*std::cerr << "     ext" << "   time,ms" << "   time/total,%" << "   number of books" <<  "    time for 1 book,ms" << std::endl;
	std::cerr << "---------------------------------------------------------------------------" << std::endl;
	for (std::map<std::string, unsigned long>::const_iterator it = ext2time.begin(); it != ext2time.end(); ++it) {
		const std::string &ext = it->first;
		unsigned long time = it->second;
		unsigned long num = ext2num[ext];
		std::cerr << std::setw(8) << ext << std::setw(10) << time << std::setw(15) << ((float) time) / totalTime * 100.0 
			<< std::setw(18) << num << std::setw(22) << ((float) time) / num << std::endl;
	}
	std::cerr << "---------------------------------------------------------------------------" << std::endl;
	std::cerr << "total:" << std::endl;
	std::cerr << std::setw(8) << "" << std::setw(10) << totalTime << std::setw(15) << "" 
		<< std::setw(20) << totalNum << std::setw(20) << "" << std::endl;*/

	return res;
}


bool Migration_0_11_0_Runnable::migrateBook(const std::string &fileName) {
	shared_ptr<Book> infoBook = Book::loadFromBookInfo(fileName);
	if (infoBook.isNull()) {
		std::cerr << "ERROR: loading book from BookInfo failed: " << fileName << std::endl;
		return false;
	}
	if (shouldReadDisk(fileName) && BooksDBUtil::isBookFull(*infoBook)) {
		shared_ptr<Book> fileBook = Book::loadFromFile(fileName);
		//shared_ptr<Book> fileBook = infoBook;
		//shared_ptr<Book> fileBook;
		if (!fileBook.isNull()) {
			std::string tagList1 = tags2string(infoBook->tags());
			std::string tagList2 = tags2string(fileBook->tags());
			if (stringEquals(tagList1, tagList2)) {
				infoBook->removeAllTags();
				const TagList &tList = fileBook->tags();
				for (TagList::const_iterator it = tList.begin(); it != tList.end(); ++it) {
					infoBook->addTag(*it);
				}
			}
		}
	}
	myBooks.insert(std::make_pair(fileName, infoBook));
	const bool code = BooksDB::Instance().saveBook(infoBook);
	if (!code) {
		std::cerr << "ERROR: saving book to database failed: " << fileName << std::endl;
	}
	return code;
}

std::string Migration_0_11_0_Runnable::tags2string(const TagList &tags) {
	std::string tagList;
	TagList::const_iterator it = tags.begin();
	if (it != tags.end()) {
		tagList += (*it++)->fullName();
		while (it != tags.end()) {
			tagList += ',';
			tagList += (*it++)->fullName();
		}
	}
	return tagList;
}

bool Migration_0_11_0_Runnable::stringEquals(const std::string &tags1, const std::string &tags2) {
	size_t i1 = 0;
	size_t i2 = 0;
	while (i1 < tags1.size() && i2 < tags2.size()) {
		if (isspace(tags1[i1])) {
			++i1;
			continue;
		}
		if (isspace(tags2[i2])) {
			++i2;
			continue;
		}
		if (tags1[i1++] != tags2[i2++]) {
			return false;
		}
	}
	if (i1 == tags1.size() && i2 < tags2.size()) {
		while (i2 < tags2.size()) {
			if (!isspace(tags2[i2++])) {
				return false;
			}
		}
		return true;
	}
	if (i1 < tags1.size() && i2 == tags2.size()) {
		while (i1 < tags1.size()) {
			if (!isspace(tags1[i1++])) {
				return false;
			}
		}
		return true;
	}
	return true;
}

bool Migration_0_11_0_Runnable::migrateBookList() {
	bool res = true;
	int size = ZLIntegerOption(ZLCategoryKey::STATE, BOOK_LIST_GROUP, BOOK_LIST_SIZE, 0).value();
	for (int i = 0; i < size; ++i) {
		std::string optionName = BOOK_LIST_PREFIX;
		ZLStringUtil::appendNumber(optionName, i);
		const std::string &fileName = ZLStringOption(ZLCategoryKey::STATE, BOOK_LIST_GROUP, optionName, "").value();
		if (!fileName.empty()) {
			std::map<std::string, shared_ptr<Book> >::iterator it = myBooks.find(fileName);
			if (it != myBooks.end()) {
				shared_ptr<Book> book = it->second;
				if (!book.isNull() && book->bookId() != 0) {
					if (!BooksDB::Instance().insertIntoBookList(*book)) {
						std::cerr << "ERROR: insert into BookList failed: " << fileName << std::endl;
						res = false;
					}
				}
			}
		}
	}
	ZLOption::clearGroup(BOOK_LIST_GROUP); // clean state.xml
	return res;
}

bool Migration_0_11_0_Runnable::migrateState() {
	bool res = true;
	if (!migrateRecentBooks()) {
		std::cerr << "ERROR(2): migrateRecentBooks failed" << std::endl;
		res = false;
	}
	if (!migrateBooksState()) {
		std::cerr << "ERROR(2): migrateBooksState failed" << std::endl;
		res = false;
	}
	ZLOption::clearGroup(RECENT_BOOKS_GROUP); // clean state.xml
	ZLOption::clearGroup(CURRENT_STATE_GROUP); // clean state.xml
	return res;
}

bool Migration_0_11_0_Runnable::migrateRecentBooks() {
	BookList books;
	for (size_t i = 0; i < MaxXmlListSize; ++i) {
		std::string num = BOOK;
		ZLStringUtil::appendNumber(num, i);
		std::string name = ZLStringOption(ZLCategoryKey::STATE, RECENT_BOOKS_GROUP, num, "").value();
		if (!name.empty()) {
			//shared_ptr<Book> book = BooksDBUtil::getBook(name, false);
			std::map<std::string, shared_ptr<Book> >::const_iterator it = myBooks.find(name);
			if (it == myBooks.end()) {
				if ((it = myBooks.find(ZLFile(name).resolvedPath())) == myBooks.end()) {
					continue;
				}
			}
			shared_ptr<Book> book = it->second;
			if (!book.isNull() && book->bookId() != 0 && std::find(books.begin(), books.end(), book) == books.end()) {
				books.push_back(book);
			}
		}
	}
	bool res = BooksDB::Instance().saveRecentBooks(books);
	if (!res) {
		std::cerr << "ERROR: saving recent books list failed (" << books.size() << " item[s])" << std::endl;
	}
	return res;
}

bool Migration_0_11_0_Runnable::migrateBooksState() {
	bool res = true;

	for (std::map<std::string, shared_ptr<Book> >::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		const std::string &fileName = it->first;
		if (it->second.isNull()) {
			std::cerr << "ERROR: book in map is null: " << fileName << std::endl;
			res = false;
			continue;
		}
		const Book &book = *it->second;
		if (!migrateBookStateStack(fileName, book)) {
			res = false;
		}
		if (!migrateBookLastState(fileName, book)) {
			res = false;
		}
	}
	return res;
}


bool Migration_0_11_0_Runnable::migrateBookStateStack(const std::string &fileName, const Book &book) {
	std::deque<ReadingState> stack;
	bool res = true;
	int stackSize = ZLIntegerOption(ZLCategoryKey::STATE, fileName, BUFFER_SIZE, 0).value();
	if (stackSize > 0) {
		if (stackSize > MaxXmlStackSize) {
			stackSize = MaxXmlStackSize;
		}
		for (int i = 0; i < stackSize; ++i) {
			std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
			std::string bufferWord = BUFFER_WORD_PREFIX;
			ZLStringUtil::appendNumber(bufferParagraph, i);
			ZLStringUtil::appendNumber(bufferWord, i);
			ReadingState pos(
				ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferParagraph, -1).value(), 
				ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferWord, -1).value(), 
				0
			);
			stack.push_back(pos);
			ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferParagraph, -1).setValue(-1); // clean state.xml
			ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferWord, -1).setValue(-1); // clean state.xml
		}
		if (!BooksDB::Instance().saveBookStateStack(book, stack)) {
			std::cerr << "ERROR: saving book state stack failed: " << fileName << std::endl;
			res = false;
		}
		stack.clear();
	}
	ZLIntegerOption(ZLCategoryKey::STATE, fileName, BUFFER_SIZE, 0).setValue(0);  // clean state.xml
	return res;
}

bool Migration_0_11_0_Runnable::migrateBookLastState(const std::string &fileName, const Book &book) {
	const ReadingState state(
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, PARAGRAPH_OPTION_NAME, 0).value(), 
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, WORD_OPTION_NAME, 0).value(), 
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, CHAR_OPTION_NAME, 0).value()
	);
	const int stackPos  = ZLIntegerOption(ZLCategoryKey::STATE, fileName, POSITION_IN_BUFFER, 0).value();
	if (state.Paragraph == 0 && state.Word == 0 && state.Character == 0 && stackPos == 0) {
		return true;
	}
	ZLIntegerOption(ZLCategoryKey::STATE, fileName, PARAGRAPH_OPTION_NAME, 0).setValue(0);
	ZLIntegerOption(ZLCategoryKey::STATE, fileName, WORD_OPTION_NAME, 0).setValue(0);
	ZLIntegerOption(ZLCategoryKey::STATE, fileName, CHAR_OPTION_NAME, 0).setValue(0);
	ZLIntegerOption(ZLCategoryKey::STATE, fileName, POSITION_IN_BUFFER, 0).setValue(0);
	bool res1 = BooksDB::Instance().setBookState(book, state);
	bool res2 = BooksDB::Instance().setStackPos(book, stackPos);
	if (!res1) {
		std::cerr << "ERROR: saving book last state failed: " << fileName << std::endl;
	}
	if (!res2) {
		std::cerr << "ERROR: saving book state stack position failed: " << fileName << std::endl;
	}
	return res1 && res2;
}

bool Migration_0_11_0_Runnable::migrateNetwork() {
	bool res = true;
	std::vector<std::string> urls;
	ZLOption::listOptionNames(NET_FILES_GROUP, urls);
	for (std::vector<std::string>::const_iterator it = urls.begin(); it != urls.end(); ++it) {
		const std::string &url = *it;
		const std::string fileName = ZLStringOption(ZLCategoryKey::NETWORK, NET_FILES_GROUP, url, "").value();
		if (!BooksDB::Instance().setNetFile(url, fileName)) {
			std::cerr << "ERROR: saving file's URL failed: " << std::endl 
				<< "\tURL = " << url << std::endl
				<< "\tfileName = " << fileName << std::endl;
			res = false;
		}
	}
	ZLOption::clearGroup(NET_FILES_GROUP); // clean state.xml
	return res;
}

bool Migration_0_11_0_Runnable::clearBooksOptions() {
	bool res = true;
	for (std::map<std::string, shared_ptr<Book> >::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		const std::string &fileName = it->first;
		if (it->second.isNull()) {
			std::cerr << "ERROR: book in map is null in clearBooksOptions: " << fileName << std::endl;
			res = false;
			continue;
		}
		ZLOption::clearGroup(fileName); // clear books.xml & state.xml
	}
	return res;
}

