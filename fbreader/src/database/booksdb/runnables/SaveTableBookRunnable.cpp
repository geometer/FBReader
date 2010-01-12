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

#include "../DBRunnables.h"
#include "../../../library/Book.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

SaveTableBookRunnable::SaveTableBookRunnable(DBConnection &connection) {
	myFindBookId = SQLiteFactory::createCommand(BooksDBQuery::FIND_BOOK_ID, connection, "@file_id", DBValue::DBINT);

	myAddBook    = SQLiteFactory::createCommand(BooksDBQuery::ADD_BOOK, connection, "@encoding", DBValue::DBTEXT, "@language", DBValue::DBTEXT, "@title", DBValue::DBTEXT, "@file_id", DBValue::DBINT);
	myUpdateBook = SQLiteFactory::createCommand(BooksDBQuery::UPDATE_BOOK, connection, "@encoding", DBValue::DBTEXT, "@language", DBValue::DBTEXT, "@title", DBValue::DBTEXT, "@book_id", DBValue::DBINT);

	myFindFileId = new FindFileIdRunnable(connection);
}

bool SaveTableBookRunnable::run() {
	if (myBook->bookId() != 0) {
		return updateTableBook(myBook);
	}

	myFindFileId->setFileName(myBook->filePath(), true);
	if (!myFindFileId->run()) {
		return false;
	}
	const int fileId = myFindFileId->fileId();

	((DBIntValue &) *myFindBookId->parameter("@file_id").value()) = fileId;
	shared_ptr<DBValue> dbBookId = myFindBookId->executeScalar();

	if (dbBookId.isNull() || dbBookId->type() != DBValue::DBINT || ((DBIntValue &) *dbBookId).value() == 0) {
		return addTableBook(myBook, fileId);
	} else {
		myBook->setBookId( ((DBIntValue &) *dbBookId).value() );
		return updateTableBook(myBook);
	}
}

bool SaveTableBookRunnable::addTableBook(const shared_ptr<Book> book, int fileId) {

	((DBTextValue &) *myAddBook->parameter("@encoding").value()) = book->encoding();
	((DBTextValue &) *myAddBook->parameter("@language").value()) = book->language();
	((DBTextValue &) *myAddBook->parameter("@title").value()) = book->title();
	((DBIntValue &) *myAddBook->parameter("@file_id").value()) = fileId;
	shared_ptr<DBValue> dbBookId = myAddBook->executeScalar();

	if (dbBookId.isNull() || dbBookId->type() != DBValue::DBINT || ((DBIntValue &) *dbBookId).value() == 0) {
		return false;
	}
	book->setBookId(((DBIntValue&)*dbBookId).value());
	return true;
}

bool SaveTableBookRunnable::updateTableBook(const shared_ptr<Book> book) {
	((DBTextValue&)*myUpdateBook->parameter("@encoding").value()) = book->encoding();
	((DBTextValue&)*myUpdateBook->parameter("@language").value()) = book->language();
	((DBTextValue&)*myUpdateBook->parameter("@title").value()) = book->title();
	((DBIntValue&)*myUpdateBook->parameter("@book_id").value()) = book->bookId();
	return myUpdateBook->execute();
}

void SaveTableBookRunnable::setBook(shared_ptr<Book> book) {
	myBook = book;
}
