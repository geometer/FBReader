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

#include "BooksDBQuery.h"

const std::string BooksDBQuery::ArchiveEntryDelimiter = ":";

const std::string BooksDBQuery::PREINIT_DATABASE = \
	"ATTACH @stateFile AS State; " \
	"ATTACH @netFile AS Net; ";

const std::string BooksDBQuery::INIT_DATABASE = \
	"CREATE TABLE IF NOT EXISTS Files ( " \
	"	file_id INTEGER PRIMARY KEY, " \
	"	name TEXT NOT NULL, " \
	"	parent_id INTEGER REFERENCES Files (file_id), " \
	"	size INTEGER, " \
	"	CONSTRAINT Files_Unique UNIQUE (name, parent_id) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS Books ( " \
	"	book_id INTEGER PRIMARY KEY, " \
	"	encoding TEXT, " \
	"	language TEXT, " \
	"	title TEXT NOT NULL, " \
	"	file_id INTEGER UNIQUE NOT NULL REFERENCES Files (file_id) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS Authors ( " \
	"	author_id INTEGER PRIMARY KEY, " \
	"	name TEXT NOT NULL, " \
	"	sort_key TEXT NOT NULL, " \
	"	CONSTRAINT Authors_Unique UNIQUE (name, sort_key) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS Series ( " \
	"	series_id INTEGER PRIMARY KEY, " \
	"	name TEXT UNIQUE NOT NULL " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS Tags ( " \
	"	tag_id INTEGER PRIMARY KEY, " \
	"	name TEXT NOT NULL, " \
	"	parent_id INTEGER REFERENCES Tags (tag_id), " \
	"	CONSTRAINT Tags_Unique UNIQUE (parent_id, name) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS BookAuthor ( " \
	"	author_id INTEGER NOT NULL REFERENCES Authors (author_id), " \
	"	book_id INTEGER NOT NULL REFERENCES Books (book_id), " \
	"	author_index INTEGER NOT NULL, " \
	"	CONSTRAINT BookAuthor_Unique0 UNIQUE (author_id, book_id), " \
	"	CONSTRAINT BookAuthor_Unique1 UNIQUE (book_id, author_index) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS BookSeries ( " \
	"	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id), " \
	"	series_id INTEGER NOT NULL REFERENCES Series (series_id), " \
	"	book_index INTEGER " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS BookTag ( " \
	"	book_id INTEGER NOT NULL REFERENCES Books (book_id), " \
	"	tag_id INTEGER NOT NULL REFERENCES Tags (tag_id), " \
	"	CONSTRAINT BookTag_Unique UNIQUE (book_id, tag_id) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS State.RecentBooks ( " \
	"	book_index INTEGER PRIMARY KEY, " \
	"	book_id INTEGER UNIQUE REFERENCES Books (book_id) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS State.BookStateStack ( " \
	"	book_id INTEGER NOT NULL REFERENCES Books (book_id), " \
	"	position INTEGER NOT NULL, " \
	"	paragraph INTEGER NOT NULL, " \
	"	word INTEGER NOT NULL, " \
	"	char INTEGER NOT NULL, " \
	"	CONSTRAINT BookStateStack_Unique UNIQUE (book_id, position) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS PalmType ( " \
	"	file_id INTEGER UNIQUE REFERENCES Files (file_id), " \
	"	type TEXT NOT NULL " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS Net.NetFiles ( " \
	"	url TEXT PRIMARY KEY,  " \
	"	file_id INTEGER UNIQUE REFERENCES Files (file_id) " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS State.StackPosition ( " \
	"	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id), " \
	"	stack_pos INTEGER NOT NULL " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS State.BookList ( " \
	"	book_id INTEGER UNIQUE NOT NULL REFERENCES Books (book_id) " \
	"); ";

const std::string BooksDBQuery::SECOND_INIT_DATABASE = \
	"CREATE TRIGGER IF NOT EXISTS Books_Delete BEFORE DELETE  " \
	"ON Books FOR EACH ROW  " \
	"BEGIN " \
	"	DELETE FROM BookAuthor WHERE book_id = OLD.book_id; " \
	"	DELETE FROM BookSeries WHERE book_id = OLD.book_id; " \
	"	DELETE FROM BookTag WHERE book_id = OLD.book_id; " \
	"	DELETE FROM StackPosition WHERE book_id = OLD.book_id; " \
	"	DELETE FROM BookStateStack WHERE book_id = OLD.book_id; " \
	"	DELETE FROM RecentBooks WHERE book_id = OLD.book_id; " \
	"	DELETE FROM BookList WHERE book_id = OLD.book_id; " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_Delete BEFORE DELETE  " \
	"ON Files FOR EACH ROW  " \
	"BEGIN " \
	"	DELETE FROM Books WHERE file_id = OLD.file_id; " \
	"	DELETE FROM PalmType WHERE file_id = OLD.file_id; " \
	"	DELETE FROM NetFiles WHERE file_id = OLD.file_id; " \
	"END; " \
	" " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_Unique_Insert BEFORE INSERT  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NULL  " \
	"	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.parent_id IS NULL AND f.name = NEW.name) " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"columns name, parent_id are not unique\"); " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_Directory_Insert BEFORE INSERT  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NULL AND NEW.size IS NOT NULL " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"size is not null for Directory entry\"); " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_ArchEntry_Insert BEFORE INSERT  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NOT NULL  " \
	"	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.file_id = NEW.parent_id AND f.parent_id IS NOT NULL) " \
	"	AND NEW.size IS NOT NULL " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"size is not null for Archive Entry entry\"); " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_Unique_Update BEFORE UPDATE  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NULL  " \
	"	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.parent_id IS NULL AND f.name = NEW.name AND f.file_id != NEW.file_id) " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"columns name, parent_id are not unique\"); " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_Directory_Update BEFORE UPDATE  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NULL AND NEW.size IS NOT NULL " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"size is not null for Directory entry\"); " \
	"END; " \
	" " \
	"CREATE TRIGGER IF NOT EXISTS Files_ArchEntry_Update BEFORE UPDATE  " \
	"ON Files FOR EACH ROW  " \
	"WHEN NEW.parent_id IS NOT NULL  " \
	"	AND 0 != (SELECT count(*) FROM Files AS f WHERE f.file_id = NEW.parent_id AND f.parent_id IS NOT NULL) " \
	"	AND NEW.size IS NOT NULL " \
	"BEGIN " \
	"	SELECT RAISE(ABORT, \"size is not null for Archive Entry entry\"); " \
	"END; ";

const std::string BooksDBQuery::CLEAR_DATABASE = \
	"DROP TRIGGER Books_Delete " \
	"DROP TRIGGER Files_Delete " \
	"DROP TRIGGER Files_Unique_Insert " \
	"DROP TRIGGER Files_Directory_Insert " \
	"DROP TRIGGER Files_ArchEntry_Insert " \
	"DROP TRIGGER Files_Unique_Update " \
	"DROP TRIGGER Files_Directory_Update " \
	"DROP TRIGGER Files_ArchEntry_Update " \
	" " \
	"DROP TABLE State.BookList; " \
	"DROP TABLE State.StackPosition; " \
	"DROP TABLE Net.NetFiles; " \
	"DROP TABLE PalmType; " \
	"DROP TABLE State.BookStateStack; " \
	"DROP TABLE State.RecentBooks; " \
	"DROP TABLE BookTag; " \
	"DROP TABLE BookSeries; " \
	"DROP TABLE BookAuthor; " \
	"DROP TABLE Tags; " \
	"DROP TABLE Series; " \
	"DROP TABLE Authors; " \
	"DROP TABLE Books; " \
	"DROP TABLE Files; ";


const std::string BooksDBQuery::LOAD_BOOK = \
	"SELECT " \
	"		b.book_id AS book_id, " \
	"		b.encoding AS encoding, " \
	"		b.language AS language, " \
	"		b.title AS title, " \
	"		b.file_id AS file_id " \
	"FROM Books AS b " \
	"WHERE b.file_id = @file_id; ";

const std::string BooksDBQuery::ADD_BOOK = \
	"INSERT INTO Books (encoding, language, title, file_id) " \
	"	VALUES ( " \
	"		nullif(@encoding,\"auto\"), " \
	"		nullif(@language,\"other\"), " \
	"		@title, " \
	"		@file_id " \
	"	); " \
	" " \
	"SELECT last_insert_rowid() AS book_id; ";

const std::string BooksDBQuery::UPDATE_BOOK = \
	"UPDATE Books SET " \
	"	encoding = nullif(@encoding,\"auto\"), " \
	"	language = nullif(@language,\"other\"), " \
	"	title = @title " \
	"WHERE " \
	"	book_id = @book_id; ";



const std::string BooksDBQuery::SET_BOOK_AUTHOR = \
	"INSERT OR REPLACE INTO BookAuthor (author_id, book_id, author_index) VALUES (@author_id, @book_id, @author_index); ";

const std::string BooksDBQuery::TRIM_BOOK_AUTHORS = \
	"DELETE FROM BookAuthor WHERE book_id = @book_id AND author_index > @authors_number; ";

const std::string BooksDBQuery::FIND_AUTHOR_ID = "SELECT author_id FROM Authors WHERE name = @name AND sort_key = @sort_key; ";

const std::string BooksDBQuery::ADD_AUTHOR = \
	"INSERT INTO Authors (name, sort_key) VALUES (@name, @sort_key); " \
	"SELECT last_insert_rowid() AS author_id; ";


const std::string BooksDBQuery::SET_BOOKSERIES = \
	"INSERT OR REPLACE INTO BookSeries (book_id, series_id, book_index) VALUES (" \
	"	@book_id, " \
	"	@series_id, " \
	"	nullif(@book_index, 0) " \
	"); ";

const std::string BooksDBQuery::DELETE_BOOKSERIES = \
	"DELETE FROM BookSeries " \
	"WHERE " \
	"	book_id = @book_id; ";

const std::string BooksDBQuery::FIND_SERIES_ID = "SELECT series_id FROM Series WHERE name = @name; ";

const std::string BooksDBQuery::ADD_SERIES = \
	"INSERT INTO Series (name) VALUES (@name); " \
	"SELECT last_insert_rowid() AS series_id; ";


const std::string BooksDBQuery::GET_FILE_SIZE = "SELECT size FROM Files WHERE file_id = @file_id";
const std::string BooksDBQuery::SET_FILE_SIZE = "UPDATE Files SET size = @size WHERE file_id = @file_id";

const std::string BooksDBQuery::SET_ENCODING = "UPDATE Books SET encoding = @encoding WHERE book_id = @book_id; ";

const std::string BooksDBQuery::LOAD_FILE_ENTRIES = "SELECT name FROM Files WHERE coalesce(parent_id, 0) = @file_id; ";

const std::string BooksDBQuery::INVALIDATE_BOOKS = "UPDATE Books SET title = \"\" WHERE file_id = @file_id; ";


const std::string BooksDBQuery::DELETE_FILE = "DELETE FROM Files WHERE file_id = @file_id; ";

const std::string BooksDBQuery::DELETE_FILE_ENTRIES = "DELETE FROM Files WHERE parent_id = @file_id; ";

const std::string BooksDBQuery::LOAD_FILE_ENTRY_IDS = "SELECT file_id FROM Files WHERE coalesce(parent_id, 0) = @file_id; ";

const std::string BooksDBQuery::FIND_BOOK_ID = "SELECT book_id FROM Books WHERE file_id = @file_id; ";


const std::string BooksDBQuery::LOAD_RECENT_BOOKS = \
	"SELECT b.file_id " \
	"FROM Books AS b " \
	"	INNER JOIN RecentBooks AS rb ON b.book_id = rb.book_id " \
	"ORDER BY rb.book_index; ";

const std::string BooksDBQuery::CLEAR_RECENT_BOOKS = "DELETE FROM RecentBooks; ";

const std::string BooksDBQuery::INSERT_RECENT_BOOKS = "INSERT INTO RecentBooks (book_id) VALUES (@book_id); ";

const std::string BooksDBQuery::FIND_FILE_NAME = "SELECT name, parent_id FROM Files WHERE file_id = @file_id; ";

const std::string BooksDBQuery::LOAD_BOOKS = "SELECT book_id, encoding, language, title, file_id FROM Books; ";

const std::string BooksDBQuery::UPDATE_AUTHOR = \
	"UPDATE Authors SET " \
	"	name = @newName, " \
	"	sort_key = @newSortKey " \
	"WHERE name = @oldName " \
	"	AND sort_key = @oldSortKey; ";

const std::string BooksDBQuery::UPDATE_BOOKS_AUTHOR = "UPDATE OR REPLACE BookAuthor SET author_id = @newAuthorId WHERE author_id = @oldAuthorId; ";

const std::string BooksDBQuery::LOAD_BOOK_STATE_STACK = "SELECT paragraph, word, char FROM BookStateStack WHERE book_id = @book_id AND position > 0 ORDER BY position; ";
const std::string BooksDBQuery::TRIM_BOOK_STATE_STACK = "DELETE FROM BookStateStack WHERE book_id = @book_id AND position > @stackSize; ";
const std::string BooksDBQuery::SET_BOOK_STATE_STACK = "INSERT OR REPLACE INTO BookStateStack(book_id, position, paragraph, word, char) VALUES (@book_id, @position, @paragraph, @word, @char); ";

const std::string BooksDBQuery::SET_PALM_TYPE = "INSERT OR REPLACE INTO PalmType (file_id, type) VALUES (@file_id, @type); ";
const std::string BooksDBQuery::GET_PALM_TYPE = "SELECT type FROM PalmType WHERE file_id = @file_id; ";

const std::string BooksDBQuery::LOAD_STACK_POS = "SELECT stack_pos FROM StackPosition WHERE book_id = @book_id; ";
const std::string BooksDBQuery::SET_STACK_POS = "INSERT OR REPLACE INTO StackPosition(book_id, stack_pos) VALUES (@book_id, @stack_pos); ";

const std::string BooksDBQuery::LOAD_BOOK_STATE = "SELECT paragraph, word, char FROM BookStateStack WHERE book_id = @book_id AND position = 0; ";
const std::string BooksDBQuery::SET_BOOK_STATE = "INSERT OR REPLACE INTO BookStateStack(book_id, position, paragraph, word, char) VALUES (@book_id, 0, @paragraph, @word, @char); ";

const std::string BooksDBQuery::INSERT_BOOK_LIST = "INSERT OR IGNORE INTO BookList(book_id) VALUES (@book_id); ";
const std::string BooksDBQuery::DELETE_BOOK_LIST = "DELETE FROM BookList WHERE book_id = @book_id; ";
const std::string BooksDBQuery::CHECK_BOOK_LIST = "SELECT COUNT(*) FROM BookList WHERE book_id = @book_id; ";

