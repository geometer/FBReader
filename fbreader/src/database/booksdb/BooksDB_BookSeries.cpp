/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

#include <string>

#include "BooksDB.h"
#include "../../library/Book.h"
#include "../sqldb/implsqlite/SQLiteFactory.h"

static const std::string LOAD_SERIES_QUERY =
	"SELECT Series.name, BookSeries.book_index" \
	" FROM BookSeries" \
	" INNER JOIN Series ON Series.series_id = BookSeries.series_id" \
	" WHERE BookSeries.book_id = @book_id;";
static const std::string LOAD_ALL_SERIES_QUERY =
	"SELECT Series.name, BookSeries.book_index, BookSeries.book_id" \
	" FROM BookSeries" \
	" INNER JOIN Series ON Series.series_id = BookSeries.series_id";

static Number getSeriesIndex(shared_ptr<DBDataReader> reader) {
	Number seriesIndex;
	if (reader->type(1) == DBValue::DBTEXT) {
		seriesIndex = Number(reader->textValue(1, std::string()));
	} else if (reader->type(1) == DBValue::DBREAL){ //for old database scheme
		seriesIndex = Number((int)reader->realValue(1));
	} else { //for old database scheme
		seriesIndex = Number(reader->intValue(1));
	}
	return seriesIndex;
}

void BooksDB::loadSeries(Book &book) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_SERIES_QUERY, connection(), "@book_id", DBValue::DBINT
	);
	((DBIntValue&)*command->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = command->executeReader();

	if (reader->next()) {
		std::string seriesTitle = reader->textValue(0, std::string());
		if (!seriesTitle.empty()) {
			book.setSeries(
				seriesTitle,
				getSeriesIndex(reader)
			);
		}
	}
}

void BooksDB::loadSeries(const std::map<int,shared_ptr<Book> > &books) {
	shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_ALL_SERIES_QUERY, connection()
	);
	shared_ptr<DBDataReader> reader = command->executeReader();

	while (reader->next()) {
		std::string seriesTitle = reader->textValue(0, std::string());
		std::map<int,shared_ptr<Book> >::const_iterator it =
			books.find((reader->type(2) == DBValue::DBINT) ? reader->intValue(2) : 0);
		if (!seriesTitle.empty() && it != books.end()) {
			it->second->setSeries(
				seriesTitle,
				getSeriesIndex(reader)
			);
		}
	}
}
