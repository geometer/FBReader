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

SaveSeriesRunnable::SaveSeriesRunnable(DBConnection &connection) {
	mySetBookSeries    = SQLiteFactory::createCommand(BooksDBQuery::SET_BOOKSERIES, connection, "@book_id", DBValue::DBINT, "@series_id", DBValue::DBINT, "@book_index", DBValue::DBINT);
	myDeleteBookSeries = SQLiteFactory::createCommand(BooksDBQuery::DELETE_BOOKSERIES, connection, "@book_id", DBValue::DBINT);
	myFindSeriesId     = SQLiteFactory::createCommand(BooksDBQuery::FIND_SERIES_ID, connection, "@name", DBValue::DBTEXT);
	myAddSeries        = SQLiteFactory::createCommand(BooksDBQuery::ADD_SERIES, connection, "@name", DBValue::DBTEXT);
}

bool SaveSeriesRunnable::run() {
	if (myBook->bookId() == 0) {
		return false;
	}

	if (myBook->seriesTitle().empty()) {
		((DBIntValue &) *myDeleteBookSeries->parameter("@book_id").value()) = myBook->bookId();
		return myDeleteBookSeries->execute();
	}

	((DBTextValue &) *myFindSeriesId->parameter("@name").value()) = myBook->seriesTitle();
	shared_ptr<DBValue> tableSeriesId = myFindSeriesId->executeScalar();
	if (tableSeriesId.isNull() || tableSeriesId->type() != DBValue::DBINT || ((DBIntValue &) *tableSeriesId).value() == 0) {
		((DBTextValue &) *myAddSeries->parameter("@name").value()) = myBook->seriesTitle();
		tableSeriesId = myAddSeries->executeScalar();
		if (tableSeriesId.isNull() || tableSeriesId->type() != DBValue::DBINT || ((DBIntValue &) *tableSeriesId).value() == 0) {
			return false;
		}
	}
	((DBIntValue &) *mySetBookSeries->parameter("@book_id").value()) = myBook->bookId();
	mySetBookSeries->parameter("@series_id").setValue( tableSeriesId );
	((DBIntValue &) *mySetBookSeries->parameter("@book_index").value()) = myBook->indexInSeries();
	return mySetBookSeries->execute();
}

void SaveSeriesRunnable::setBook(shared_ptr<Book> book) {
	myBook = book;
}
