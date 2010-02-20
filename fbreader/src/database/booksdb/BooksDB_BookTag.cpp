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
#include <algorithm>

#include "BooksDB.h"
#include "DBRunnables.h"
#include "../../library/Book.h"
#include "../../library/Tag.h"
#include "../sqldb/implsqlite/SQLiteFactory.h"

static const std::string LOAD_BOOK_TAGS_QUERY =
	"SELECT tag_id FROM BookTag WHERE book_id = @book_id";
static const std::string LOAD_ALL_BOOK_TAGS_QUERY =
	"SELECT tag_id, book_id FROM BookTag";
static const std::string LOAD_SINGLE_TAG_QUERY =
	"SELECT name, parent_id FROM Tags WHERE tag_id = @tag_id";
static const std::string LOAD_ALL_TAGS_QUERY =
	"SELECT name, parent_id, tag_id FROM Tags ORDER BY tag_id";
static const std::string ADD_BOOKTAG =
	"INSERT INTO BookTag (book_id, tag_id) VALUES (@book_id, @tag_id)";
static const std::string DELETE_BOOKTAG =
	"DELETE FROM BookTag WHERE book_id = @book_id AND tag_id = @tag_id";
static const std::string FIND_TAG_ID =
	"SELECT tag_id FROM Tags" \
	" WHERE name = @name AND coalesce(parent_id, 0) = @parent_id";
static const std::string ADD_TAG =
	"INSERT INTO Tags (name, parent_id) VALUES (@name, nullif(@parent_id, 0));" \
	" SELECT last_insert_rowid() AS tag_id";

void BooksDB::loadTags(Book &book) {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_BOOK_TAGS_QUERY, connection(), "@book_id", DBValue::DBINT
	);
	((DBIntValue&)*command->parameter("@book_id").value()) = book.bookId();
	shared_ptr<DBDataReader> reader = command->executeReader();

	book.removeAllTags();
	while (reader->next()) {
		book.addTag(getTagById(reader->intValue(0)));
	}
}

void BooksDB::loadTags(const std::map<int,shared_ptr<Book> > &books) {
	loadAllTagsById();

	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_ALL_BOOK_TAGS_QUERY, connection()
	);
	shared_ptr<DBDataReader> reader = command->executeReader();

	for (std::map<int,shared_ptr<Book> >::const_iterator it = books.begin(); it != books.end(); ++it) {
		it->second->removeAllTags();
	}

	while (reader->next()) {
		std::map<int,shared_ptr<Book> >::const_iterator it =
			books.find((reader->type(1) == DBValue::DBINT) ? reader->intValue(1) : 0);
		if (it != books.end()) {
			it->second->addTag(getTagById(reader->intValue(0)));
		}
	}
}

shared_ptr<Tag> BooksDB::getTagById(int id) const {
	if (id == 0) {
		return 0;
	}

	shared_ptr<Tag> tag = Tag::getTagById(id);
	if (!tag.isNull()) {
		return tag;
	}

	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_SINGLE_TAG_QUERY, connection(), "@tag_id", DBValue::DBINT
	);
	((DBIntValue&)*command->parameter("@tag_id").value()) = id;
	shared_ptr<DBDataReader> reader = command->executeReader();
	if (!reader->next()) {
		return 0;
	}
	const std::string name = reader->textValue(0, std::string());
	const int parentId = (reader->type(1) == DBValue::DBINT) ?
		reader->intValue(1) : 0;
	reader.reset();

	return Tag::getTag(name, getTagById(parentId), id);
}

void BooksDB::loadAllTagsById() const {
	static shared_ptr<DBCommand> command = SQLiteFactory::createCommand(
		LOAD_ALL_TAGS_QUERY, connection()
	);
	shared_ptr<DBDataReader> reader = command->executeReader();
	while (reader->next()) {
		if (reader->type(2) != DBValue::DBINT) {
			continue;
		}
		const int id = reader->intValue(2);
		if (!Tag::getTagById(id).isNull()) {
			continue;
		}
		Tag::getTag(
			reader->textValue(0, std::string()),
			Tag::getTagById(
				(reader->type(1) == DBValue::DBINT) ? reader->intValue(1) : 0
			),
			id
		);
	}
}

SaveTagsRunnable::SaveTagsRunnable(DBConnection &connection) {
	myDeleteBookTag = SQLiteFactory::createCommand(
		DELETE_BOOKTAG, connection,
		"@book_id", DBValue::DBINT,
		"@tag_id", DBValue::DBINT
	);
	myFindTagId = SQLiteFactory::createCommand(
		FIND_TAG_ID, connection,
		"@name", DBValue::DBTEXT,
		"@parent_id", DBValue::DBINT
	);
	myAddTag = SQLiteFactory::createCommand(
		ADD_TAG, connection,
		"@name", DBValue::DBTEXT,
		"@parent_id", DBValue::DBINT
	);
	myAddBookTag = SQLiteFactory::createCommand(
		ADD_BOOKTAG, connection,
		"@book_id", DBValue::DBINT,
		"@tag_id", DBValue::DBINT
	);
	myLoadBookTags = SQLiteFactory::createCommand(
		LOAD_BOOK_TAGS_QUERY, connection, "@book_id", DBValue::DBINT
	);
}

bool SaveTagsRunnable::run() {
	if (myBook->bookId() == 0) {
		return false;
	}

	((DBIntValue&)*myDeleteBookTag->parameter("@book_id").value()) = myBook->bookId();
	DBIntValue &delTagId = (DBIntValue&)*myDeleteBookTag->parameter("@tag_id").value();
	((DBIntValue&)*myAddBookTag->parameter("@book_id").value()) = myBook->bookId();
	DBIntValue &addTagId = (DBIntValue&)*myAddBookTag->parameter("@tag_id").value();

	TagList dbTags;

	((DBIntValue&)*myLoadBookTags->parameter("@book_id").value()) = myBook->bookId();
	shared_ptr<DBDataReader> reader = myLoadBookTags->executeReader();

	while (reader->next()) {
		shared_ptr<Tag> tag = BooksDB::Instance().getTagById(reader->intValue(0));
		if (!tag.isNull()) {
			dbTags.push_back(tag);
		}
	}

	TagList tags = myBook->tags(); // make copy of vector

	for (TagList::const_iterator it = dbTags.begin(); it != dbTags.end(); ++it) {
		shared_ptr<Tag> tag = (*it);
		findTagId(tag);
		TagList::iterator jt = std::find(tags.begin(), tags.end(), tag);
		if (jt == tags.end()) {
			// Tag `tag` must be removed from BookTag table.
			delTagId.setValue(tag->tagId());
			if (!myDeleteBookTag->execute()) {
				return false;
			}
		} else {
			// This tag is already in DataBase => need not to be inserted.
			tags.erase(jt);
		}
	}

	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		int tableTagId = findTagId(*it);
		if (tableTagId == 0) {
			return false;
		}
		addTagId.setValue(tableTagId);
		if (!myAddBookTag->execute()) {
			return false;
		}
	}
	return true;
}


int SaveTagsRunnable::findTagId(shared_ptr<Tag> tag) {
	int tagId = tag->tagId();
	if (tagId != 0) {
		return tagId;
	}
	int parentId = 0;
	if (!tag->parent().isNull()) {
		parentId = findTagId(tag->parent());
		if (parentId == 0) {
			return 0;
		}
	}

	DBIntValue &findParent = (DBIntValue&)*myFindTagId->parameter("@parent_id").value();
	DBTextValue &findName = (DBTextValue&)*myFindTagId->parameter("@name").value();
	DBIntValue &addParent = (DBIntValue&)*myAddTag->parameter("@parent_id").value();
	DBTextValue &addName  = (DBTextValue&)*myAddTag->parameter("@name").value();

	findParent.setValue(parentId);
	findName.setValue(tag->name());
	shared_ptr<DBValue> tableTagId = myFindTagId->executeScalar();
	if (tableTagId.isNull() || tableTagId->type() != DBValue::DBINT || ((DBIntValue&)*tableTagId).value() == 0) {
		addParent.setValue(parentId);
		addName.setValue(tag->name());
		tableTagId = myAddTag->executeScalar();
		if (tableTagId.isNull() || tableTagId->type() != DBValue::DBINT || ((DBIntValue&)*tableTagId).value() == 0) {
			return 0;
		}
	}
	Tag::setTagId(tag, ((DBIntValue&)*tableTagId).value());
	return ((DBIntValue&)*tableTagId).value();
}

void SaveTagsRunnable::setBook(shared_ptr<Book> book) {
	myBook = book;
}
