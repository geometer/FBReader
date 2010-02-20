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
#include "../../sqldb/implsqlite/SQLiteFactory.h"

LoadSeriesRunnable::LoadSeriesRunnable(DBConnection &connection) {
	myLoadSeries      = SQLiteFactory::createCommand(BooksDBQuery::LOAD_SERIES, connection, "@book_id", DBValue::DBINT);
}

bool LoadSeriesRunnable::run() {
	mySeriesTitle = "";
	myIndexInSeries = 0;

	((DBIntValue &) *myLoadSeries->parameter("@book_id").value()) = myBookId;
	shared_ptr<DBDataReader> reader = myLoadSeries->executeReader();
	if (reader.isNull()) {
		return false;
	}

	if (!reader->next()) {
		return true;
	}

	int numType = reader->type(1);
	if (reader->type(0) != DBValue::DBTEXT || (numType != DBValue::DBINT && numType != DBValue::DBNULL)) {
		return false;
	}
	mySeriesTitle = reader->textValue(0, std::string());
	if (numType == DBValue::DBINT) {
		myIndexInSeries = reader->intValue(1);
	} else {
		myIndexInSeries = 0;
	}
	return true;
}

void LoadSeriesRunnable::setBookId(int bookId) {
	myBookId = bookId;
}

const std::string &LoadSeriesRunnable::seriesTitle() const {
	return mySeriesTitle;
}

int LoadSeriesRunnable::indexInSeries() const {
	return myIndexInSeries;
}
