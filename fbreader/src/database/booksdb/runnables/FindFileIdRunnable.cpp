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

#include "../DBRunnables.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

static const std::string FIND_FILE_ID =
	"SELECT file_id FROM Files" \
	" WHERE name = @name AND coalesce(parent_id, 0) = @parent_id;";

static const std::string ADD_FILE =
	"INSERT INTO Files (name, parent_id, size)" \
	" VALUES(@name, nullif(@parent_id, 0), nullif(@size, 0));" \
	" SELECT last_insert_rowid() AS file_id;";

FindFileIdRunnable::FindFileIdRunnable(DBConnection &connection) {
	myFindFileId      = SQLiteFactory::createCommand(FIND_FILE_ID, connection, "@name", DBValue::DBTEXT, "@parent_id", DBValue::DBINT);
	myAddFile         = SQLiteFactory::createCommand(ADD_FILE, connection, "@name", DBValue::DBTEXT, "@parent_id", DBValue::DBINT, "@size", DBValue::DBINT);
}

bool FindFileIdRunnable::run() {
	const std::string resolvedPath = ZLFile(myFileName).resolvedPath();
	const std::string physPath = ZLFile(resolvedPath).physicalFilePath();
	const std::string dirName  = physPath.substr(0, physPath.rfind(ZLibrary::FileNameDelimiter));

	DBTextValue &findName = (DBTextValue &) *myFindFileId->parameter("@name").value();
	DBIntValue &findParent = (DBIntValue &) *myFindFileId->parameter("@parent_id").value();

	DBTextValue &addName = (DBTextValue &) *myAddFile->parameter("@name").value();
	DBIntValue &addParent = (DBIntValue &) *myAddFile->parameter("@parent_id").value();
	((DBIntValue &) *myAddFile->parameter("@size").value()) = 0;

	size_t index = dirName.length() + 1;
	findName = dirName;
	findParent = 0;
	while (true) {
		shared_ptr<DBValue> physId = myFindFileId->executeScalar();
		if (physId.isNull() || physId->type() != DBValue::DBINT || ((DBIntValue &) *physId).value() == 0) {
			if (!myAdd) {
				return false;
			}
			addName = findName.value();
			addParent = findParent.value();
			physId = myAddFile->executeScalar();
			if (physId.isNull() || physId->type() != DBValue::DBINT || ((DBIntValue &) *physId).value() == 0) {
				return false;
			}
		}
		if (index == 0) {
			myFileId = ((DBIntValue &) *physId).value();
			return true;
		}
		size_t index2 = resolvedPath.find(BooksDBQuery::ArchiveEntryDelimiter, index);
		findName = resolvedPath.substr(index, index2 - index);
		index = index2 + 1;
		findParent = ((DBIntValue &) *physId).value();
	}
}

void FindFileIdRunnable::setFileName(const std::string &fileName, bool add) {
	myFileName = fileName;
	myAdd = add;
	myFileId = 0;
}

int FindFileIdRunnable::fileId() const {
	return myFileId;
}

SaveFileEntriesRunnable::SaveFileEntriesRunnable(DBConnection &connection) {
	myAddFile = SQLiteFactory::createCommand(ADD_FILE, connection, "@name", DBValue::DBTEXT, "@parent_id", DBValue::DBINT, "@size", DBValue::DBINT);

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

