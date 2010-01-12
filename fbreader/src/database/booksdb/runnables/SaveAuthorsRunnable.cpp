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

#include <algorithm>

#include "../DBRunnables.h"
#include "../../../library/Book.h"
#include "../../../library/Author.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

SaveAuthorsRunnable::SaveAuthorsRunnable(DBConnection &connection) {
	mySetBookAuthor   = SQLiteFactory::createCommand(BooksDBQuery::SET_BOOK_AUTHOR, connection, "@author_id", DBValue::DBINT, "@book_id", DBValue::DBINT, "@author_index", DBValue::DBINT);
	myTrimBookAuthors = SQLiteFactory::createCommand(BooksDBQuery::TRIM_BOOK_AUTHORS, connection, "@book_id", DBValue::DBINT, "@authors_number", DBValue::DBINT);
	myFindAuthorId     = SQLiteFactory::createCommand(BooksDBQuery::FIND_AUTHOR_ID, connection, "@name", DBValue::DBTEXT, "@sort_key", DBValue::DBTEXT);
	myAddAuthor        = SQLiteFactory::createCommand(BooksDBQuery::ADD_AUTHOR, connection, "@name", DBValue::DBTEXT, "@sort_key", DBValue::DBTEXT);
}

bool SaveAuthorsRunnable::run() {
	if (myBook->bookId() == 0) {
		return false;
	}
	const AuthorList &bookAuthors = myBook->authors(); // save link to vector

	((DBIntValue &) *mySetBookAuthor->parameter("@book_id").value()) = myBook->bookId();
	DBIntValue &setAuthorId    = (DBIntValue &) *mySetBookAuthor->parameter("@author_id").value();
	DBIntValue &setAuthorIndex = (DBIntValue &) *mySetBookAuthor->parameter("@author_index").value();
	DBTextValue &findAuthor  = (DBTextValue &) *myFindAuthorId->parameter("@name").value();
	DBTextValue &findSortKey = (DBTextValue &) *myFindAuthorId->parameter("@sort_key").value();
	DBTextValue &addAuthor   = (DBTextValue &) *myAddAuthor->parameter("@name").value();
	DBTextValue &addSortKey  = (DBTextValue &) *myAddAuthor->parameter("@sort_key").value();

	int index = 0;
	for (AuthorList::const_iterator it = bookAuthors.begin(); it != bookAuthors.end(); ++it) {
		const Author &author = **it;
		findAuthor.setValue( author.name() );
		findSortKey.setValue( author.sortKey() );
		shared_ptr<DBValue> tableAuthorId = myFindAuthorId->executeScalar();
		if (tableAuthorId.isNull() || tableAuthorId->type() != DBValue::DBINT || ((DBIntValue &) *tableAuthorId).value() == 0) {
			addAuthor.setValue( author.name() );
			addSortKey.setValue( author.sortKey() );
			tableAuthorId = myAddAuthor->executeScalar();
			if (tableAuthorId.isNull() || tableAuthorId->type() != DBValue::DBINT || ((DBIntValue &) *tableAuthorId).value() == 0) {
				return false;
			}
		}
		setAuthorId = ((DBIntValue &) *tableAuthorId).value();
		setAuthorIndex = ++index;
		if (!mySetBookAuthor->execute()) {
			return false;
		}
	}
	((DBIntValue &) *myTrimBookAuthors->parameter("@book_id").value()) = myBook->bookId();
	((DBIntValue &) *myTrimBookAuthors->parameter("@authors_number").value()) = index;
	return myTrimBookAuthors->execute();
}

void SaveAuthorsRunnable::setBook(shared_ptr<Book> book) {
	myBook = book;
}
