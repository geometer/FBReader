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

LoadFileEntriesRunnable::LoadFileEntriesRunnable(DBConnection &connection) {
	myLoadFileEntries = SQLiteFactory::createCommand(BooksDBQuery::LOAD_FILE_ENTRIES, connection, "@file_id", DBValue::DBINT);

	myFindFileId = new FindFileIdRunnable(connection);
}

bool LoadFileEntriesRunnable::run() {
	DBCommand &cmd = *myLoadFileEntries;

	myFindFileId->setFileName(myFileName);
	if (!myFindFileId->run()) {
		return false;
	}
	((DBIntValue &) *cmd.parameter("@file_id").value()) = myFindFileId->fileId();

	shared_ptr<DBDataReader> reader = cmd.executeReader();

	if (reader.isNull()) {
		return false;
	}
	
	myEntries.clear();

	bool res = true;
	while (reader->next()) {
		if (reader->type(0) != DBValue::DBTEXT /* name */) {
			res = false;
			continue;
		}
		myEntries.push_back(
			myFileName + BooksDBQuery::ArchiveEntryDelimiter +
			reader->textValue(0, std::string())
		);
	}
	reader->close();
	return res;
}

void LoadFileEntriesRunnable::setFileName(const std::string &fileName) {
	myFileName = fileName;
}

void LoadFileEntriesRunnable::collectEntries(std::vector<std::string> &entries) {
	myEntries.swap(entries);
	myEntries.clear();
}
