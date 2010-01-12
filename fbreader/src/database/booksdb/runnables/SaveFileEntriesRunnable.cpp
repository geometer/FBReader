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
#include <algorithm>

#include "../DBRunnables.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"


SaveFileEntriesRunnable::SaveFileEntriesRunnable(DBConnection &connection) {
	myAddFile = SQLiteFactory::createCommand(BooksDBQuery::ADD_FILE, connection, "@name", DBValue::DBTEXT, "@parent_id", DBValue::DBINT, "@size", DBValue::DBINT);

	myFindFileId = new FindFileIdRunnable(connection);
	myDeleteFileEntries = new DeleteFileEntriesRunnable(connection);
}

bool SaveFileEntriesRunnable::run() {
	myFindFileId->setFileName(myFileName, true);
	if (!myFindFileId->run()) {
		return false;
	}

	myDeleteFileEntries->setFileId(myFindFileId->fileId());
	if (!myDeleteFileEntries->run()) {
		return false;
	}

	DBTextValue &addName = (DBTextValue &) *myAddFile->parameter("@name").value();
	((DBIntValue &) *myAddFile->parameter("@parent_id").value()) = myFindFileId->fileId();
	((DBIntValue &) *myAddFile->parameter("@size").value()) = 0;

	for (std::vector<std::string>::const_iterator it = myEntries.begin(); it != myEntries.end(); ++it) {
		const std::string &entry = (*it);
		if (entry.empty()) {
			continue;
		}
		addName = entry;
		if (!myAddFile->execute()) {
			return false;
		}
	}
	return true;
}

