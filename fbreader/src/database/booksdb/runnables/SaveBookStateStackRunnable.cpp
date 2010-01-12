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


SaveBookStateStackRunnable::SaveBookStateStackRunnable(DBConnection &connection) {
	myTrimBookStateStack = SQLiteFactory::createCommand(BooksDBQuery::TRIM_BOOK_STATE_STACK, connection, "@book_id", DBValue::DBINT, "@stackSize", DBValue::DBINT);
	mySetBookStateStack = SQLiteFactory::createCommand(BooksDBQuery::SET_BOOK_STATE_STACK, connection, "@book_id", DBValue::DBINT, "@position", DBValue::DBINT, "@paragraph", DBValue::DBINT, "@word", DBValue::DBINT, "@char", DBValue::DBINT);
}

bool SaveBookStateStackRunnable::run() {
	((DBIntValue &) *myTrimBookStateStack->parameter("@book_id").value()) = myBookId;
	((DBIntValue &) *myTrimBookStateStack->parameter("@stackSize").value()) = myStack.size();
	
	if (!myTrimBookStateStack->execute()) {
		return false;
	}

	((DBIntValue &) *mySetBookStateStack->parameter("@book_id").value()) = myBookId;

	DBIntValue &savePosition  = (DBIntValue &) *mySetBookStateStack->parameter("@position").value();
	DBIntValue &saveParagraph = (DBIntValue &) *mySetBookStateStack->parameter("@paragraph").value();
	DBIntValue &saveWord      = (DBIntValue &) *mySetBookStateStack->parameter("@word").value();
	DBIntValue &saveChar      = (DBIntValue &) *mySetBookStateStack->parameter("@char").value();

	for (size_t i = 0; i < myStack.size(); ++i) {
		const ReadingState &pos = myStack[i];
		savePosition = i + 1;
		saveParagraph = pos.Paragraph;
		saveWord = pos.Word;
		saveChar = pos.Character;
		if (!mySetBookStateStack->execute()) {
			return false;
		}
	}
	return true;
}

