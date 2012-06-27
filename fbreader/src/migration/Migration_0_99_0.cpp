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

#include <ZLLogger.h>

#include "../database/booksdb/BooksDB.h"
#include "../library/Number.h"

#include "Migration.h"

static const std::string RENAME_TABLE_TO_OBSOLETE =	"ALTER TABLE BookSeries RENAME TO BookSeries_Obsolete";
static const std::string CREATE_NEW_TABLE = \
	"CREATE TABLE IF NOT EXISTS BookSeries ( " \
	"	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id), " \
	"	series_id INTEGER NOT NULL REFERENCES Series (series_id), " \
	"	book_index TEXT " \
	"); ";
static const std::string DROP_OBSOLETE_TABLE = "DROP TABLE BookSeries_Obsolete";
static const std::string LOAD_OBSOLETE_SERIES_QUERY =
	"SELECT book_id, series_id, book_index" \
	" FROM BookSeries_Obsolete;";

class Migration_0_99_0_Runnable : public DBRunnable {
public:
	bool run();

};

bool Migration_0_99_0_Runnable::run() {
	DBConnection &connection = BooksDB::Instance().connection();

	shared_ptr<DBCommand> renameTable = SQLiteFactory::createCommand(RENAME_TABLE_TO_OBSOLETE, connection);
	shared_ptr<DBCommand> createNewTable = SQLiteFactory::createCommand(CREATE_NEW_TABLE, connection);
	shared_ptr<DBCommand> dropObsoleteTable = SQLiteFactory::createCommand(DROP_OBSOLETE_TABLE, connection);
	shared_ptr<DBCommand> loadObsoleteSeries = SQLiteFactory::createCommand(LOAD_OBSOLETE_SERIES_QUERY, connection);
	shared_ptr<DBCommand> insertBookSeries =  SQLiteFactory::createCommand(BooksDBQuery::SET_BOOKSERIES, connection, "@book_id", DBValue::DBINT, "@series_id", DBValue::DBINT, "@book_index", DBValue::DBTEXT);

	if (!renameTable->execute()) {
		return false;
	}
	if (!createNewTable->execute()) {
		return false;
	}

	shared_ptr<DBDataReader> reader = loadObsoleteSeries->executeReader();
	while (reader->next()) {
		((DBIntValue &) *insertBookSeries->parameter("@book_id").value()) = reader->intValue(0);
		((DBIntValue &) *insertBookSeries->parameter("@series_id").value()) = reader->intValue(1);
		Number seriesIndex;
		if (reader->type(2) == DBValue::DBREAL){
			seriesIndex = Number((int)reader->realValue(2));
		} else {
			seriesIndex = Number(reader->intValue(2));
		}
		((DBTextValue &) *insertBookSeries->parameter("@book_index").value()) = seriesIndex.value();
		if (!insertBookSeries->execute()) {
			ZLLogger::Instance().println("Migration", "problems with inserting series & book index");
		}
	}
	dropObsoleteTable->execute();
	return true;
}


Migration_0_99_0::Migration_0_99_0() : Migration("0.99.0")  {

}

void Migration_0_99_0::doMigrationInternal() {
	Migration_0_99_0_Runnable r;
	BooksDB::Instance().executeAsTransaction(r);
}

