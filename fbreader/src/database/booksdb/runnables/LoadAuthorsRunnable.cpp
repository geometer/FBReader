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
#include "../../../library/Author.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

LoadAuthorsRunnable::LoadAuthorsRunnable(DBConnection &connection) {
	myLoadAuthors = SQLiteFactory::createCommand(BooksDBQuery::LOAD_AUTHORS, connection, "@book_id", DBValue::DBINT);
}

bool LoadAuthorsRunnable::run() {
	DBCommand &cmd = *myLoadAuthors;
	((DBIntValue &) *cmd.parameter("@book_id").value()) = myBookId;
	shared_ptr<DBDataReader> reader = cmd.executeReader();
	
	myAuthors.clear();

	if (reader.isNull()) {
		return false;
	}

	while (reader->next()) {
		if (reader->type(0) != DBValue::DBTEXT /* name */
		 || reader->type(1) != DBValue::DBTEXT /* sort_key */ 
		 || reader->type(2) != DBValue::DBINT  /* author_index */) {
			return false;
		}
		const std::string name = reader->textValue(0, std::string());
		const std::string sortKey = reader->textValue(1, std::string());
		const size_t authorIndex = reader->intValue(2);
		if (authorIndex != (1 + myAuthors.size())) {
			return false;
		}
		shared_ptr<Author> author = Author::getAuthor(name, sortKey);
		if (author.isNull()) {
			return false;
		}
		myAuthors.push_back(author);
	}
	return true;
}

void LoadAuthorsRunnable::setBookId(int bookId) {
	myBookId = bookId;
}

void LoadAuthorsRunnable::collectAuthors(AuthorList &authors) {
	myAuthors.swap(authors);
	myAuthors.clear();
}
