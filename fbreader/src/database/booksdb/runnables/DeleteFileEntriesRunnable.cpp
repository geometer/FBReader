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

DeleteFileEntriesRunnable::DeleteFileEntriesRunnable(DBConnection &connection) {
	myDeleteFileEntries = SQLiteFactory::createCommand(BooksDBQuery::DELETE_FILE_ENTRIES, connection, "@file_id", DBValue::DBINT);
	myLoadFileEntryIds  = SQLiteFactory::createCommand(BooksDBQuery::LOAD_FILE_ENTRY_IDS, connection, "@file_id", DBValue::DBINT);
}

bool DeleteFileEntriesRunnable::run() {
	return doDelete(myFileId);
}

bool DeleteFileEntriesRunnable::doDelete(int fileId) {
	(DBIntValue &) *myLoadFileEntryIds->parameter("@file_id").value() = fileId;
	shared_ptr<DBDataReader> reader = myLoadFileEntryIds->executeReader();
	if (reader.isNull()) {
		return false;
	}

	std::vector<int> fileIds;
	while (reader->next()) {
		if (reader->type(0) != DBValue::DBINT /* file_id  */ ) {
			reader->close();
			return false;
		}
		fileIds.push_back(reader->intValue(0));
	}
	reader->close();

	if (fileIds.empty()) {
		return true;
	}
	for (std::vector<int>::const_iterator it = fileIds.begin(); it != fileIds.end(); ++it) {
		if (!doDelete(*it)) {
			return false;
		}
	}
	(DBIntValue &) *myDeleteFileEntries->parameter("@file_id").value() = fileId;
	return myDeleteFileEntries->execute();
}

