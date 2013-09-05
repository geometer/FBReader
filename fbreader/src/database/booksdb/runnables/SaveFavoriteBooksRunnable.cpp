/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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
#include "../../../library/Book.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

SaveFavoriteBooksRunnable::SaveFavoriteBooksRunnable(DBConnection &connection) {
    myClearFavoriteBooks = SQLiteFactory::createCommand(BooksDBQuery::CLEAR_FAVORITE_BOOKS, connection);
    myInsertFavoriteBooks = SQLiteFactory::createCommand(BooksDBQuery::INSERT_FAVORITE_BOOKS, connection, "@book_id", DBValue::DBINT);
}

bool SaveFavoriteBooksRunnable::run() {
    if (!myClearFavoriteBooks->execute()) {
        return false;
    }
    DBIntValue &insertBookId = (DBIntValue &) *myInsertFavoriteBooks->parameter("@book_id").value();
    for (BookList::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
        shared_ptr<Book> book = (*it);
        if (book->bookId() == 0) {
            return false;
        }
        insertBookId = book->bookId();
        if (!myInsertFavoriteBooks->execute()) {
            return false;
        }
    }
    return true;
}

void SaveFavoriteBooksRunnable::setBooks(const BookList &books) {
    myBooks = books; // copy vector
}

