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

#include <iostream>

#include "SQLiteConnection.h"

SQLiteConnection::SQLiteConnection(const std::string &name)
	: DBConnection() 
	, myName(name) 
	, myDatabase(0) {
}

SQLiteConnection::~SQLiteConnection() {
	if (isOpened()) {
		close();
	}
}


bool SQLiteConnection::open() {
	if (myDatabase != 0) {
		return true;
	}
	int res = sqlite3_open(myName.c_str(), &myDatabase);
	if (res == SQLITE_OK) {
		return true;
	}
	dumpError();
	if (myDatabase != 0) {
		sqlite3_close(myDatabase);
		myDatabase = 0;
	}
	return false;
}

void SQLiteConnection::finalizeOpenedStatements() {
	size_t size = myStatements.size();
	for (size_t i = 0; i < size; ++i) {
		const int res = sqlite3_finalize(myStatements[i]);
		if (res != SQLITE_OK) {
			dumpError();
		}
	}
	myStatements.clear();
}

bool SQLiteConnection::close() {
	if (myDatabase == 0) {
		return true;
	}

	finalizeOpenedStatements();

	int res = sqlite3_close(myDatabase);
	if (res == SQLITE_OK) {
		myDatabase = 0;
		return true;
	}
	dumpError();
	return false;
}

void SQLiteConnection::dumpError() const {
	if (myDatabase != 0) {
		const std::string msg = sqlite3_errmsg(myDatabase); // TODO: error & message handling
		const int code = sqlite3_errcode(myDatabase); // TODO: error & message handling
		std::cerr << "SQLITE IMPLEMENTATION ERROR: (" << code << ") " << msg << std::endl;
	}
}
