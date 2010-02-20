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
#include "../../library/Author.h"
#include "../sqldb/implsqlite/SQLiteFactory.h"

static const std::string LOAD_AUTHORS_QUERY =
	"SELECT Authors.name, Authors.sort_key" \
	" FROM BookAuthor" \
	"	INNER JOIN Authors ON Authors.author_id = BookAuthor.author_id" \
	" WHERE BookAuthor.book_id = @book_id" \
	" ORDER BY BookAuthor.author_index;";
static const std::string LOAD_ALL_AUTHORS_QUERY =
	"SELECT Authors.name, Authors.sort_key, BookAuthor.book_id" \
	" FROM BookAuthor" \
	"	INNER JOIN Authors ON Authors.author_id = BookAuthor.author_id" \
	" ORDER BY BookAuthor.author_index;";

void BooksDB::loadAuthors(Book &book) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_AUTHORS_QUERY, connection(), "@book_id", DBValue::DBINT
	);
	((DBIntValue&)*command->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = command->executeReader();
	
	book.removeAllAuthors();

	while (reader->next()) {
		book.addAuthor(
			reader->textValue(0, std::string()),
			reader->textValue(1, std::string())
		);
	}
}

void BooksDB::loadAuthors(const std::map<int,shared_ptr<Book> > &books) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_ALL_AUTHORS_QUERY, connection()
	);
	shared_ptr<DBDataReader> reader = command->executeReader();
	
	for (std::map<int,shared_ptr<Book> >::const_iterator it = books.begin(); it != books.end(); ++it) {
		it->second->removeAllAuthors();
	}

	while (reader->next()) {
		std::map<int,shared_ptr<Book> >::const_iterator it =
			books.find((reader->type(2) == DBValue::DBINT) ? reader->intValue(2) : 0);
		if (it != books.end()) {
			it->second->addAuthor(
				reader->textValue(0, std::string()),
				reader->textValue(1, std::string())
			);
		}
	}
}
