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

#ifndef __BOOKSDB_H__
#define __BOOKSDB_H__

#include <set>
#include <map>
#include <deque>

#include "../sqldb/implsqlite/SQLiteDataBase.h"
#include "DBRunnables.h"

#include "../../fbreader/ReadingState.h"

class Book;

class BooksDB : public SQLiteDataBase {

public:
	static const std::string DATABASE_NAME;
	static const std::string STATE_DATABASE_NAME;
	static const std::string NET_DATABASE_NAME;

	static BooksDB &Instance();

private:
	static shared_ptr<BooksDB> ourInstance;

	BooksDB(const std::string &path);

public:
	virtual ~BooksDB();

public:
	bool initDatabase();
	bool isInitialized() const;
	bool clearDatabase();

	shared_ptr<Book> loadBook(const std::string &fileName);
	bool saveBook(const shared_ptr<Book> book);
	bool saveAuthors(const shared_ptr<Book> book);
	bool saveSeries(const shared_ptr<Book> book);
	bool saveTags(const shared_ptr<Book> book);

	int getFileSize(const std::string fileName);
	bool setFileSize(const std::string fileName, int size);

	bool setEncoding(const Book &book, const std::string &encoding);

	bool loadFileEntries(const std::string &fileName, std::vector<std::string> &entries);
	bool saveFileEntries(const std::string &fileName, const std::vector<std::string> &entries);

	bool loadRecentBooks(std::vector<std::string> &fileNames);
	bool saveRecentBooks(const BookList &books);

	bool loadBooks(BookList &books);

	bool loadBookStateStack(const Book &book, std::deque<ReadingState> &stack);
	bool saveBookStateStack(const Book &book, const std::deque<ReadingState> &stack);

	bool removeBook(const Book &book);

	std::string getPalmType(const std::string &fileName);
	bool setPalmType(const std::string &fileName, const std::string &type);

	std::string getNetFile(const std::string &url);
	bool setNetFile(const std::string &url, const std::string &fileName);
	bool unsetNetFile(const std::string &url);

	bool loadBookState(const Book &book, ReadingState &state);
	bool setBookState(const Book &book, const ReadingState &state);

	int loadStackPos(const Book &book);
	bool setStackPos(const Book &book, int stackPos);
	
	bool insertIntoBookList(const Book &book);
	bool deleteFromBookList(const Book &book);
	bool checkBookList(const Book &book);

private:
public:
	shared_ptr<Tag> getTagById(int id) const;
	void loadAllTagsById() const;

private:
	void loadSeries(Book &book);
	void loadSeries(const std::map<int,shared_ptr<Book> > &books);
	void loadAuthors(Book &book);
	void loadAuthors(const std::map<int,shared_ptr<Book> > &books);
	void loadTags(Book &book);
	void loadTags(const std::map<int,shared_ptr<Book> > &books);

	std::string getFileName(int fileId);

private:
	void initCommands();

private:
	bool myInitialized;

	shared_ptr<SaveTableBookRunnable> mySaveTableBook;
	shared_ptr<SaveAuthorsRunnable> mySaveAuthors;
	shared_ptr<SaveSeriesRunnable> mySaveSeries;
	shared_ptr<SaveTagsRunnable> mySaveTags;
	shared_ptr<SaveBookRunnable> mySaveBook;
	shared_ptr<SaveFileEntriesRunnable> mySaveFileEntries;

	shared_ptr<FindFileIdRunnable> myFindFileId;

	shared_ptr<LoadFileEntriesRunnable> myLoadFileEntries;

	shared_ptr<LoadRecentBooksRunnable> myLoadRecentBooks;
	shared_ptr<SaveRecentBooksRunnable> mySaveRecentBooks;

	shared_ptr<SaveBookStateStackRunnable> mySaveBookStateStack;
	
	shared_ptr<DeleteBookRunnable> myDeleteBook;

	shared_ptr<DBCommand> myLoadBook;

	shared_ptr<DBCommand> myGetFileSize;
	shared_ptr<DBCommand> mySetFileSize;

	shared_ptr<DBCommand> myFindFileName;

	shared_ptr<DBCommand> myFindAuthorId;

	shared_ptr<DBCommand> myLoadBooks;

	shared_ptr<DBCommand> myLoadBookStateStack;

	shared_ptr<DBCommand> myGetPalmType;
	shared_ptr<DBCommand> mySetPalmType;

	shared_ptr<DBCommand> myGetNetFile;
	shared_ptr<DBCommand> mySetNetFile;

	shared_ptr<DBCommand> myLoadStackPos;
	shared_ptr<DBCommand> mySetStackPos;
	shared_ptr<DBCommand> myLoadBookState;
	shared_ptr<DBCommand> mySetBookState;

	shared_ptr<DBCommand> myInsertBookList;
	shared_ptr<DBCommand> myDeleteBookList;
	shared_ptr<DBCommand> myCheckBookList;

private: // disable copying
	BooksDB(const BooksDB &);
	const BooksDB &operator = (const BooksDB &);
};


inline bool BooksDB::isInitialized() const { return myInitialized; }

#endif /* __BOOKSDB_H__ */
