/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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
#include "../../../library/Tag.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

static const std::string LOAD_BOOK_TAGS_QUERY =
	"SELECT tag_id FROM BookTag WHERE book_id = @book_id";

static const std::string LOAD_SINGLE_TAG_QUERY =
	"SELECT name, parent_id FROM Tags WHERE tag_id = @tag_id";

LoadTagsRunnable::LoadTagsRunnable(DBConnection &connection) {
	myLoadBookTags = SQLiteFactory::createCommand(
		LOAD_BOOK_TAGS_QUERY, connection, "@book_id", DBValue::DBINT
	);
	myLoadSingleTag = SQLiteFactory::createCommand(
		LOAD_SINGLE_TAG_QUERY, connection, "@tag_id", DBValue::DBINT
	);
}

shared_ptr<Tag> LoadTagsRunnable::getTag(int id) {
	if (id == 0) {
		return 0;
	}

	shared_ptr<Tag> tag = Tag::getTagById(id);
	if (!tag.isNull()) {
		return tag;
	}

	((DBIntValue&)*myLoadSingleTag->parameter("@tag_id").value()) = id;
	shared_ptr<DBDataReader> reader = myLoadSingleTag->executeReader();
	if (reader.isNull() || !reader->next()) {
		return 0;
	}
	const std::string name = reader->textValue(0);
	const int parentId = reader->intValue(1);
	reader.reset();

	return Tag::getTag(name, getTag(parentId), id);
}

bool LoadTagsRunnable::run(Book &book) {
	TagList tags;
	if (!run(book.bookId(), tags)) {
		return false;
	}

	book.removeAllTags();
	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		book.addTag(*it);
	}
	return true;
}

bool LoadTagsRunnable::run(int bookId, TagList &tags) {
	((DBIntValue&)*myLoadBookTags->parameter("@book_id").value()) = bookId;

	tags.clear();

	shared_ptr<DBDataReader> reader = myLoadBookTags->executeReader();
	if (reader.isNull()) {
		return false;
	}

	while (reader->next()) {
		shared_ptr<Tag> tag = getTag(reader->intValue(0));
		if (!tag.isNull()) {
			tags.push_back(tag);
		}
	}

	return true;
}
