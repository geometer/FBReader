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

#ifndef __DBRUNNABLES_H__
#define __DBRUNNABLES_H__

#include <deque>
#include <vector>

#include "../../fbreader/ReadingState.h"

#include "../sqldb/DBConnection.h"
#include "../sqldb/DBCommand.h"
#include "../sqldb/DBRunnable.h"

#include "BooksDBQuery.h"

#include "../../library/Lists.h"

class FindFileIdRunnable;
class LoadFileEntriesRunnable;
class DeleteFileEntriesRunnable;

/*
 * Save Runnables
 */
 

class InitBooksDBRunnable : public DBRunnable {

public:
	InitBooksDBRunnable(DBConnection &connection);
	bool run();

private:
	DBConnection &myConnection;
};

class ClearBooksDBRunnable : public DBRunnable {

public:
	ClearBooksDBRunnable(DBConnection &connection);
	bool run();

private:
	DBConnection &myConnection;
};

class SaveTableBookRunnable : public DBRunnable {

public:
	SaveTableBookRunnable(DBConnection &connection);
	bool run();
	void setBook(shared_ptr<Book> book);

private:
	bool addTableBook(const shared_ptr<Book> book, int fileId);
	bool updateTableBook(const shared_ptr<Book> book);

private:
	shared_ptr<Book> myBook;

	shared_ptr<DBCommand> myFindBookId;

	shared_ptr<DBCommand> myAddBook;
	shared_ptr<DBCommand> myUpdateBook;

	shared_ptr<FindFileIdRunnable> myFindFileId;
};

class SaveAuthorsRunnable : public DBRunnable {

public:
	SaveAuthorsRunnable(DBConnection &connection);
	bool run();
	void setBook(shared_ptr<Book> book);

private:
	shared_ptr<Book> myBook;

	shared_ptr<DBCommand> mySetBookAuthor;
	shared_ptr<DBCommand> myTrimBookAuthors;

	shared_ptr<DBCommand> myFindAuthorId;
	shared_ptr<DBCommand> myAddAuthor;
};

class SaveTagsRunnable : public DBRunnable {

public:
	SaveTagsRunnable(DBConnection &connection);
	bool run();
	void setBook(shared_ptr<Book> book);

private:
	int findTagId(shared_ptr<Tag> tag);

private:
	shared_ptr<Book> myBook;

	shared_ptr<DBCommand> myAddBookTag;
	shared_ptr<DBCommand> myDeleteBookTag;

	shared_ptr<DBCommand> myFindTagId;
	shared_ptr<DBCommand> myAddTag;

	shared_ptr<DBCommand> myLoadBookTags;
};


class SaveSeriesRunnable : public DBRunnable {

public:
	SaveSeriesRunnable(DBConnection &connection);
	bool run();
	void setBook(shared_ptr<Book> book);

private:
	shared_ptr<Book> myBook;

	shared_ptr<DBCommand> mySetBookSeries;
	shared_ptr<DBCommand> myDeleteBookSeries;

	shared_ptr<DBCommand> myFindSeriesId;
	shared_ptr<DBCommand> myAddSeries;
};

class SaveBookRunnable : public DBRunnable {
public:
	SaveBookRunnable(SaveTableBookRunnable &saveTableBook, SaveAuthorsRunnable &saveAuthors, 
		SaveSeriesRunnable &saveSeries, SaveTagsRunnable &saveTags);

	bool run();
	void setBook(shared_ptr<Book> book);

private:
	SaveTableBookRunnable &mySaveTableBook;
	SaveAuthorsRunnable &mySaveAuthors;
	SaveSeriesRunnable &mySaveSeries;
	SaveTagsRunnable &mySaveTags;
};

class SaveFileEntriesRunnable : public DBRunnable {

public:
	SaveFileEntriesRunnable(DBConnection &connection);
	bool run();
	void setEntries(const std::string &fileName, const std::vector<std::string> &entries);

private:
	std::string myFileName;
	std::vector<std::string> myEntries;

	shared_ptr<DBCommand> myAddFile;

	shared_ptr<FindFileIdRunnable> myFindFileId;
	shared_ptr<DeleteFileEntriesRunnable> myDeleteFileEntries;
};

class SaveRecentBooksRunnable : public DBRunnable {

public:
	SaveRecentBooksRunnable(DBConnection &connection);
	bool run();
	void setBooks(const BookList &books);

private:
	BookList myBooks;

	shared_ptr<DBCommand> myClearRecentBooks;
	shared_ptr<DBCommand> myInsertRecentBooks;
};

class SaveBookStateStackRunnable : public DBRunnable {

public:
	SaveBookStateStackRunnable(DBConnection &connection);
	bool run();
	void setState(int bookId, const std::deque<ReadingState > &stack);

private:
	int myBookId;
	std::deque<ReadingState > myStack;

	shared_ptr<DBCommand> myTrimBookStateStack;
	shared_ptr<DBCommand> mySetBookStateStack;
};

class DeleteFileEntriesRunnable : public DBRunnable {

public:
	DeleteFileEntriesRunnable(DBConnection &connection);
	bool run();
	void setFileId(int fileId);

private:
	bool doDelete(int fileId);

private:
	int myFileId;

	shared_ptr<DBCommand> myDeleteFileEntries;
	shared_ptr<DBCommand> myLoadFileEntryIds;
};

class DeleteBookRunnable : public DBRunnable {

public:
	DeleteBookRunnable(DBConnection &connection);
	bool run();
	void setFileName(const std::string &fileName);

private:
	std::string myFileName;

	shared_ptr<FindFileIdRunnable> myFindFileId;
	shared_ptr<DBCommand> myDeleteFile;
};


inline InitBooksDBRunnable::InitBooksDBRunnable(DBConnection &connection) : myConnection(connection) {}
inline ClearBooksDBRunnable::ClearBooksDBRunnable(DBConnection &connection) : myConnection(connection) {}

inline void SaveFileEntriesRunnable::setEntries(const std::string &fileName, const std::vector<std::string> &entries) {
	myFileName = fileName;
	myEntries  = entries; // copy vector
}

inline void SaveBookStateStackRunnable::setState(int bookId, const std::deque<ReadingState > &stack) {
	myBookId = bookId;
	myStack = stack; // copy deque
}

inline void DeleteFileEntriesRunnable::setFileId(int fileId) { myFileId = fileId; }

inline void DeleteBookRunnable::setFileName(const std::string &fileName) { myFileName = fileName; }

/*
 * Load & Modify Runnables
 */

class FindFileIdRunnable : public DBRunnable {

public:
	FindFileIdRunnable(DBConnection &connection);
	bool run();
	void setFileName(const std::string &fileName, bool add = false);

	int fileId() const;

private:
	std::string myFileName;
	bool myAdd;
	int myFileId;

	shared_ptr<DBCommand> myFindFileId;
	shared_ptr<DBCommand> myAddFile;
};


/*
 * Load Runnables
 */

class LoadFileEntriesRunnable : public DBRunnable {

public:
	LoadFileEntriesRunnable(DBConnection &connection);
	bool run();
	void setFileName(const std::string &fileName);
	void collectEntries(std::vector<std::string> &entries);

private:
	std::string myFileName;
	std::vector<std::string> myEntries;

	shared_ptr<FindFileIdRunnable> myFindFileId;

	shared_ptr<DBCommand> myLoadFileEntries;
};

class LoadRecentBooksRunnable : public DBRunnable {

public:
	LoadRecentBooksRunnable(DBConnection &connection);
	bool run();
	void collectFileIds(std::vector<int> &fileIds);

private:
	std::vector<int> myFileIds;

	shared_ptr<DBCommand> myLoadRecentBooks;
};

#endif /* __DBRUNNABLES_H__ */
