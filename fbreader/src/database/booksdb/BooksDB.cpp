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

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLLanguageUtil.h>

#include "BooksDB.h"
#include "BooksDBQuery.h"

#include "../../library/Book.h"
#include "../../library/Author.h"
#include "../../library/Tag.h"

#include "../sqldb/implsqlite/SQLiteFactory.h"

shared_ptr<BooksDB> BooksDB::ourInstance = 0;

const std::string BooksDB::DATABASE_NAME = "books.db";
const std::string BooksDB::STATE_DATABASE_NAME = "state.db";
const std::string BooksDB::NET_DATABASE_NAME = "network.db";

BooksDB &BooksDB::Instance() {
	if (ourInstance.isNull()) {
		ZLFile dir(databaseDirName());
		dir.directory(true);
		ZLFile file(databaseDirName() + ZLibrary::FileNameDelimiter + DATABASE_NAME);
		ourInstance = new BooksDB(file.physicalFilePath());
		ourInstance->initDatabase();
	}
	return *ourInstance;
}

BooksDB::BooksDB(const std::string &path) : SQLiteDataBase(path), myInitialized(false) {
	initCommands();
}

BooksDB::~BooksDB() {
}

bool BooksDB::initDatabase() {
	if (isInitialized()) {
		return true;
	}

	if (!open()) {
		return false;
	} 

	myInitialized = true;

	ZLFile stateFile(databaseDirName() + ZLibrary::FileNameDelimiter + STATE_DATABASE_NAME);
	ZLFile netFile(databaseDirName() + ZLibrary::FileNameDelimiter + NET_DATABASE_NAME);
	shared_ptr<DBCommand> cmd = SQLiteFactory::createCommand(BooksDBQuery::PREINIT_DATABASE, connection(), "@stateFile", DBValue::DBTEXT, "@netFile", DBValue::DBTEXT);
	((DBTextValue&)*cmd->parameter("@stateFile").value()) = stateFile.physicalFilePath();
	((DBTextValue&)*cmd->parameter("@netFile").value()) = netFile.physicalFilePath();
	if (!cmd->execute()) {
		myInitialized = false;
		close();
		return false;
	}

	shared_ptr<DBRunnable> runnable = new InitBooksDBRunnable(connection());
	if (!executeAsTransaction(*runnable)) {
		myInitialized = false;
		close();
		return false;
	}

	return true;
}

void BooksDB::initCommands() {
	myLoadBook     = SQLiteFactory::createCommand(BooksDBQuery::LOAD_BOOK, connection(), "@file_id", DBValue::DBINT);
	myGetFileSize  = SQLiteFactory::createCommand(BooksDBQuery::GET_FILE_SIZE, connection(), "@file_id", DBValue::DBINT);
	mySetFileSize  = SQLiteFactory::createCommand(BooksDBQuery::SET_FILE_SIZE, connection(), "@file_id", DBValue::DBINT, "@size", DBValue::DBINT);
	myFindFileName = SQLiteFactory::createCommand(BooksDBQuery::FIND_FILE_NAME, connection(), "@file_id", DBValue::DBINT);
	myFindAuthorId    = SQLiteFactory::createCommand(BooksDBQuery::FIND_AUTHOR_ID, connection(), "@name", DBValue::DBTEXT, "@sort_key", DBValue::DBTEXT);

	myLoadBooks = SQLiteFactory::createCommand(BooksDBQuery::LOAD_BOOKS, connection());

	myLoadBookStateStack = SQLiteFactory::createCommand(BooksDBQuery::LOAD_BOOK_STATE_STACK, connection(), "@book_id", DBValue::DBINT);

	myGetPalmType = SQLiteFactory::createCommand(BooksDBQuery::GET_PALM_TYPE, connection(), "@file_id", DBValue::DBINT);
	mySetPalmType = SQLiteFactory::createCommand(BooksDBQuery::SET_PALM_TYPE, connection(), "@file_id", DBValue::DBINT, "@type", DBValue::DBTEXT);

	myLoadStackPos = SQLiteFactory::createCommand(BooksDBQuery::LOAD_STACK_POS, connection(), "@book_id", DBValue::DBINT);
	mySetStackPos = SQLiteFactory::createCommand(BooksDBQuery::SET_STACK_POS, connection(), "@book_id", DBValue::DBINT, "@stack_pos", DBValue::DBINT);

	myLoadBookState = SQLiteFactory::createCommand(BooksDBQuery::LOAD_BOOK_STATE, connection(), "@book_id", DBValue::DBINT);
	mySetBookState = SQLiteFactory::createCommand(BooksDBQuery::SET_BOOK_STATE, connection(), "@book_id", DBValue::DBINT, "@paragraph", DBValue::DBINT, "@word", DBValue::DBINT, "@char", DBValue::DBINT);

	myInsertBookList = SQLiteFactory::createCommand(BooksDBQuery::INSERT_BOOK_LIST, connection(), "@book_id", DBValue::DBINT);
	myDeleteBookList = SQLiteFactory::createCommand(BooksDBQuery::DELETE_BOOK_LIST, connection(), "@book_id", DBValue::DBINT);
	myCheckBookList = SQLiteFactory::createCommand(BooksDBQuery::CHECK_BOOK_LIST, connection(), "@book_id", DBValue::DBINT);

	mySaveTableBook = new SaveTableBookRunnable(connection());
	mySaveAuthors = new SaveAuthorsRunnable(connection());
	mySaveSeries = new SaveSeriesRunnable(connection());
	mySaveTags = new SaveTagsRunnable(connection());
	mySaveBook = new SaveBookRunnable(*mySaveTableBook, *mySaveAuthors, *mySaveSeries, *mySaveTags);
	mySaveFileEntries = new SaveFileEntriesRunnable(connection());

	myFindFileId = new FindFileIdRunnable(connection());

	myLoadFileEntries = new LoadFileEntriesRunnable(connection());

	myLoadRecentBooks = new LoadRecentBooksRunnable(connection());
	mySaveRecentBooks = new SaveRecentBooksRunnable(connection());

	mySaveBookStateStack = new SaveBookStateStackRunnable(connection());

	myDeleteBook = new DeleteBookRunnable(connection());
}

bool BooksDB::clearDatabase() {
	if (!isInitialized()) {
		return false;
	}
	shared_ptr<DBRunnable> runnable = new ClearBooksDBRunnable(connection());
	return executeAsTransaction(*runnable);
}

shared_ptr<Book> BooksDB::loadBook(const std::string &fileName) {
	if (!isInitialized()) {
		return 0;
	}

	myFindFileId->setFileName(fileName);
	if (!myFindFileId->run()) {
		return false;
	}
	((DBIntValue&)*myLoadBook->parameter("@file_id").value()) = myFindFileId->fileId();
	shared_ptr<DBDataReader> reader = myLoadBook->executeReader();

	if (reader.isNull() || !reader->next() ||
			reader->type(0) != DBValue::DBINT /* book_id */) {
		return 0;
	}
	const int bookId = reader->intValue(0);

	shared_ptr<Book> book = Book::createBook(
		fileName, bookId,
		reader->textValue(1, Book::AutoEncoding),
		reader->textValue(2, ZLLanguageUtil::OtherLanguageCode),
		reader->textValue(3, std::string())
	);

	loadSeries(*book);
	loadAuthors(*book);
	loadTags(*book);

	return book;
}


bool BooksDB::saveBook(const shared_ptr<Book> book) {
	if (!isInitialized()) {
		return false;
	}
	mySaveBook->setBook(book);
	return executeAsTransaction(*mySaveBook);
}

bool BooksDB::saveAuthors(const shared_ptr<Book> book) {
	if (!isInitialized()) {
		return false;
	}
	mySaveAuthors->setBook(book);
	return executeAsTransaction(*mySaveAuthors);
}

bool BooksDB::saveSeries(const shared_ptr<Book> book) {
	if (!isInitialized()) {
		return false;
	}
	mySaveSeries->setBook(book);
	return executeAsTransaction(*mySaveSeries);
}

bool BooksDB::saveTags(const shared_ptr<Book> book) {
	if (!isInitialized()) {
		return false;
	}
	mySaveTags->setBook(book);
	return executeAsTransaction(*mySaveTags);
}

int BooksDB::getFileSize(const std::string fileName) {
	if (!isInitialized()) {
		return -1;
	}
	myFindFileId->setFileName(fileName);
	if (!myFindFileId->run()) {
		return 0;
	}
	((DBIntValue&)*myGetFileSize->parameter("@file_id").value()) = myFindFileId->fileId();

	shared_ptr<DBValue> fileSize = myGetFileSize->executeScalar();

	if (fileSize.isNull()) {
		return -1;
	}
	if (fileSize->type() == DBValue::DBNULL) {
		return 0;
	}
	if (fileSize->type() != DBValue::DBINT) {
		return -1;
	}
	return ((DBIntValue&)*fileSize).value();
}

bool BooksDB::setFileSize(const std::string fileName, int size) {
	if (!isInitialized()) {
		return false;
	}
	myFindFileId->setFileName(fileName, true);
	if (!executeAsTransaction(*myFindFileId)) {
		return false;
	}
	((DBIntValue&)*mySetFileSize->parameter("@file_id").value()) = myFindFileId->fileId();
	((DBIntValue&)*mySetFileSize->parameter("@size").value()) = size;
	return mySetFileSize->execute();
}

bool BooksDB::setEncoding(const Book &book, const std::string &encoding) {
	if (!isInitialized()) {
		return false;
	}

	shared_ptr<DBCommand> command = SQLiteFactory::createCommand(BooksDBQuery::SET_ENCODING, connection());

	command->parameters().push_back(DBCommandParameter("@book_id", new DBIntValue(book.bookId())));
	command->parameters().push_back(DBCommandParameter("@encoding", new DBTextValue(encoding)));

	return command->execute();
}

bool BooksDB::loadFileEntries(const std::string &fileName, std::vector<std::string> &entries) {
	myLoadFileEntries->setFileName(fileName);
	if (!myLoadFileEntries->run()) {
		return false;
	}
	myLoadFileEntries->collectEntries(entries);
	return true;
}

bool BooksDB::saveFileEntries(const std::string &fileName, const std::vector<std::string> &entries) {
	if (!isInitialized()) {
		return false;
	}
	mySaveFileEntries->setEntries(fileName, entries);
	return executeAsTransaction(*mySaveFileEntries);
}

bool BooksDB::loadRecentBooks(std::vector<std::string> &fileNames) {
	std::vector<int> fileIds;
	if (!myLoadRecentBooks->run()) {
		return false;
	}
	myLoadRecentBooks->collectFileIds(fileIds);
	for (std::vector<int>::const_iterator it = fileIds.begin(); it != fileIds.end(); ++it) {
		const int fileId = *it;
		const std::string fileName = getFileName(fileId);
		fileNames.push_back(fileName);
	}
	return true;
}

bool BooksDB::saveRecentBooks(const BookList &books) {
	if (!isInitialized()) {
		return false;
	}
	mySaveRecentBooks->setBooks(books);
	return executeAsTransaction(*mySaveRecentBooks);
}

std::string BooksDB::getFileName(int fileId) {
	std::string fileName;
	DBIntValue &findFileId = (DBIntValue&)*myFindFileName->parameter("@file_id").value();
	findFileId = fileId;
	while (true) {
		shared_ptr<DBDataReader> reader = myFindFileName->executeReader();
		if (reader.isNull() || !reader->next()) {
			return std::string();
		}
		const std::string namePart = reader->textValue(0, std::string());
		switch (reader->type(1)) { /* parent_id */
			default:
				return std::string();
			case DBValue::DBNULL:
				return namePart + ZLibrary::FileNameDelimiter + fileName;
			case DBValue::DBINT:
				if (fileName.empty()) {
					fileName = namePart;
				} else {
					fileName = namePart + BooksDBQuery::ArchiveEntryDelimiter + fileName;
				}
				findFileId = reader->intValue(1);
				break;
		}
	}
}

bool BooksDB::loadBooks(BookList &books) {
	shared_ptr<DBDataReader> reader = myLoadBooks->executeReader();

	books.clear();
	std::map<int,shared_ptr<Book> > bookMap;

	while (reader->next()) {
		if (reader->type(0) != DBValue::DBINT || /* book_id */
				reader->type(4) != DBValue::DBINT) { /* file_id */
			return false;
		}
		const int bookId = reader->intValue(0);
		const int fileId = reader->intValue(4);
		const std::string fileName = getFileName(fileId);

		shared_ptr<Book> book = Book::createBook(
			fileName,
			bookId,
			reader->textValue(1, Book::AutoEncoding),
			reader->textValue(2, ZLLanguageUtil::OtherLanguageCode),
			reader->textValue(3, std::string())
		);
		books.push_back(book);
		bookMap[bookId] = book;
	}

	loadSeries(bookMap);
	loadAuthors(bookMap);
	loadTags(bookMap);

	return true;
}

bool BooksDB::loadBookStateStack(const Book &book, std::deque<ReadingState> &stack) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*myLoadBookStateStack->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = myLoadBookStateStack->executeReader();
	if (reader.isNull()) {
		return false;
	}
	while (reader->next()) {
		if (reader->type(0) != DBValue::DBINT    /* paragraph */
			|| reader->type(1) != DBValue::DBINT /* word      */
			|| reader->type(2) != DBValue::DBINT /* char      */) {
			return false;
		}
		const int paragraph = reader->intValue(0);
		const int word = reader->intValue(1);
		const int character = reader->intValue(2);
		stack.push_back(ReadingState(paragraph, word, character));
	}
	return true;
}

bool BooksDB::saveBookStateStack(const Book &book, const std::deque<ReadingState> &stack) {
	if (!isInitialized() || book.bookId() == 0) {
		return false;
	}
	mySaveBookStateStack->setState(book.bookId(), stack);
	return executeAsTransaction(*mySaveBookStateStack);
}


bool BooksDB::removeBook(const Book &book) {
	if (!isInitialized() || book.bookId() == 0) {
		return false;
	}
	myDeleteBook->setFileName(book.filePath());
	return executeAsTransaction(*myDeleteBook);
}

std::string BooksDB::getPalmType(const std::string &fileName) {
	if (!isInitialized()) {
		return "";
	}
	myFindFileId->setFileName(fileName);
	if (!myFindFileId->run()) {
		return "";
	}
	((DBIntValue&)*myGetPalmType->parameter("@file_id").value()) = myFindFileId->fileId();
	shared_ptr<DBValue> value = myGetPalmType->executeScalar();
	if (value.isNull() || value->type() != DBValue::DBTEXT) {
		return "";
	}
	return ((DBTextValue&)*value).value();
}

bool BooksDB::setPalmType(const std::string &fileName, const std::string &type) {
	if (!isInitialized()) {
		return false;
	}
	myFindFileId->setFileName(fileName, true);
	if (!myFindFileId->run()) {
		return "";
	}
	((DBIntValue&)*mySetPalmType->parameter("@file_id").value()) = myFindFileId->fileId();
	((DBTextValue&)*mySetPalmType->parameter("@type").value()) = type;
	return mySetPalmType->execute();
}

std::string BooksDB::getNetFile(const std::string &url) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		"SELECT file_id FROM NetFiles WHERE url = @url;",
		connection(), "@url", DBValue::DBTEXT
	);

	((DBTextValue&)*command->parameter("@url").value()) = url;
	shared_ptr<DBValue> value = command->executeScalar();
	if (value.isNull() || value->type() != DBValue::DBINT) {
		return std::string();
	}
	return getFileName(((DBIntValue&)*value).value());
}

bool BooksDB::setNetFile(const std::string &url, const std::string &fileName) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		"INSERT OR REPLACE INTO NetFiles (url, file_id) VALUES (@url, @file_id);",
		connection(), "@file_id", DBValue::DBINT, "@url", DBValue::DBTEXT
	);
	
	myFindFileId->setFileName(fileName, true);
	if (!myFindFileId->run()) {
		return false;
	}
	((DBIntValue&)*command->parameter("@file_id").value()) = myFindFileId->fileId();
	((DBTextValue&)*command->parameter("@url").value()) = url;
	return command->execute();
}

bool BooksDB::unsetNetFile(const std::string &url) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		"SELECT file_id FROM NetFiles WHERE url = @url;",
		connection(), "@url", DBValue::DBTEXT
	);

	((DBTextValue&)*command->parameter("@url").value()) = url;
	return command->execute();
}

bool BooksDB::loadBookState(const Book &book, ReadingState &state) {
	state.Paragraph = state.Word = state.Character = 0;
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*myLoadBookState->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = myLoadBookState->executeReader();
	if (reader.isNull()) {
		return false;
	}
	if (!reader->next()
		|| reader->type(0) != DBValue::DBINT /* paragraph */
		|| reader->type(1) != DBValue::DBINT /* word      */
		|| reader->type(2) != DBValue::DBINT /* char      */) {
		return false;
	}
	state.Paragraph = reader->intValue(0);
	state.Word = reader->intValue(1);
	state.Character = reader->intValue(2);
	return true;
}

bool BooksDB::setBookState(const Book &book, const ReadingState &state) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*mySetBookState->parameter("@book_id").value()) = book.bookId();
	((DBIntValue&)*mySetBookState->parameter("@paragraph").value()) = state.Paragraph;
	((DBIntValue&)*mySetBookState->parameter("@word").value()) = state.Word;
	((DBIntValue&)*mySetBookState->parameter("@char").value()) = state.Character;
	return mySetBookState->execute();
}

int BooksDB::loadStackPos(const Book &book) {
	if (book.bookId() == 0) {
		return 0;
	}
	((DBIntValue&)*myLoadStackPos->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBValue> stackPosValue = myLoadStackPos->executeScalar();
	if (stackPosValue.isNull()
		|| stackPosValue->type() != DBValue::DBINT) {
		return 0;
	}
	return ((DBIntValue&)*stackPosValue).value();
}

bool BooksDB::setStackPos(const Book &book, int stackPos) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*mySetStackPos->parameter("@book_id").value()) = book.bookId();
	((DBIntValue&)*mySetStackPos->parameter("@stack_pos").value()) = stackPos;
	return mySetStackPos->execute();
}

bool BooksDB::insertIntoBookList(const Book &book) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*myInsertBookList->parameter("@book_id").value()) = book.bookId();
	return myInsertBookList->execute();
}

bool BooksDB::deleteFromBookList(const Book &book) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*myDeleteBookList->parameter("@book_id").value()) = book.bookId();
	return myDeleteBookList->execute();
}

bool BooksDB::checkBookList(const Book &book) {
	if (book.bookId() == 0) {
		return false;
	}
	((DBIntValue&)*myCheckBookList->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBValue> res = myCheckBookList->executeScalar();
	if (res.isNull() || res->type() != DBValue::DBINT) {
		return false;
	}
	const int checkRes = ((DBIntValue&)*res).value();
	return checkRes > 0;
}

