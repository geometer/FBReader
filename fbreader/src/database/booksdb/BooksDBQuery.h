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

#ifndef __BOOKSDBQUERY_H__
#define __BOOKSDBQUERY_H__

#include <string>

class BooksDBQuery {

public:
	static const std::string ArchiveEntryDelimiter;

public:
	static const std::string PREINIT_DATABASE;
	static const std::string INIT_DATABASE;
	static const std::string SECOND_INIT_DATABASE;
	static const std::string CLEAR_DATABASE;

	static const std::string LOAD_BOOK;

	static const std::string ADD_BOOK;
	static const std::string UPDATE_BOOK;

	static const std::string FIND_AUTHOR_ID;
	static const std::string ADD_AUTHOR;

	static const std::string GET_BOOK_AUTHORS_NUMBER;
	static const std::string TRIM_BOOK_AUTHORS;
	static const std::string SET_BOOK_AUTHOR;

	static const std::string DELETE_BOOKSERIES;
	static const std::string FIND_SERIES_ID;
	static const std::string ADD_SERIES;
	static const std::string SET_BOOKSERIES;

	static const std::string GET_FILE_SIZE;
	static const std::string SET_FILE_SIZE;

	static const std::string SET_ENCODING;

	static const std::string LOAD_FILE_ENTRIES;

	static const std::string INVALIDATE_BOOKS;

	static const std::string DELETE_FILE;
	static const std::string DELETE_FILE_ENTRIES;
	static const std::string LOAD_FILE_ENTRY_IDS;

	static const std::string FIND_BOOK_ID;

	static const std::string LOAD_RECENT_BOOKS;
	static const std::string CLEAR_RECENT_BOOKS;
	static const std::string INSERT_RECENT_BOOKS;

	static const std::string FIND_FILE_NAME;

	static const std::string LOAD_BOOKS;

	static const std::string UPDATE_AUTHOR;
	static const std::string UPDATE_BOOKS_AUTHOR;

	static const std::string LOAD_BOOK_STATE_STACK;
	static const std::string TRIM_BOOK_STATE_STACK;
	static const std::string SET_BOOK_STATE_STACK;

	static const std::string GET_PALM_TYPE;
	static const std::string SET_PALM_TYPE;

	static const std::string LOAD_BOOK_STATE;
	static const std::string SET_BOOK_STATE;
	static const std::string LOAD_STACK_POS;
	static const std::string SET_STACK_POS;

	static const std::string INSERT_BOOK_LIST;
	static const std::string DELETE_BOOK_LIST;
	static const std::string CHECK_BOOK_LIST;

private: // disable creation Instances
	BooksDBQuery();
};

#endif /* __BOOKSDBQUERY_H__ */
