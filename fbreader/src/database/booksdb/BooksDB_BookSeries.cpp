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

#include <string>

#include "BooksDB.h"
#include "../../library/Book.h"
#include "../sqldb/implsqlite/SQLiteFactory.h"

static std::string LOAD_SERIES_QUERY =
	"SELECT Series.name, BookSeries.book_index" \
	" FROM BookSeries" \
	" INNER JOIN Series ON Series.series_id = BookSeries.series_id" \
	" WHERE BookSeries.book_id = @book_id;";

static std::string LOAD_ALL_SERIES_QUERY =
	"SELECT Series.name, BookSeries.book_index, BookSeries.book_id" \
	" FROM BookSeries" \
	" INNER JOIN Series ON Series.series_id = BookSeries.series_id";

void BooksDB::loadSeries(Book &book) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_SERIES_QUERY, connection(), "@book_id", DBValue::DBINT
	);

	((DBIntValue&)*command->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = command->executeReader();

	if (!reader->next()) {
		return;
	}

	std::string seriesTitle = reader->textValue(0, std::string());
	if (!seriesTitle.empty()) {
		book.setSeries(
			seriesTitle,
			(reader->type(1) == DBValue::DBINT) ? reader->intValue(1) : 0
		);
	}
}
