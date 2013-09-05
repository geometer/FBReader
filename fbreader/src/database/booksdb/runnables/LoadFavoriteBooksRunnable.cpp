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
#include "../../../library/Author.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

LoadFavoriteBooksRunnable::LoadFavoriteBooksRunnable(DBConnection &connection) {
    myLoadFavoriteBooks = SQLiteFactory::createCommand(BooksDBQuery::LOAD_FAVORITE_BOOKS, connection);
}

bool LoadFavoriteBooksRunnable::run() {
    shared_ptr<DBDataReader> reader = myLoadFavoriteBooks->executeReader();
    if (reader.isNull()) {
        return false;
    }
    myFileIds.clear();

    bool res = true;
    while (reader->next()) {
        if (reader->type(0) != DBValue::DBINT /* file_id */) {
            res = false;
            continue;
        }
        const int fileId = reader->intValue(0);
        myFileIds.push_back(fileId);
    }
    reader->close();
    return res;
}

void LoadFavoriteBooksRunnable::collectFileIds(std::vector<int> &fileIds) {
    myFileIds.swap(fileIds);
    myFileIds.clear();
}

