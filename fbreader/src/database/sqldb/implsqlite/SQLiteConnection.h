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

#ifndef __SQLITECONNECTION_H__
#define __SQLITECONNECTION_H__

#include <sqlite3.h>

#include <algorithm>
#include <vector>
#include <string>

#include "../DBConnection.h"

class SQLiteConnection : public DBConnection {

public:
	SQLiteConnection(const std::string &name);
	~SQLiteConnection();

	bool open();
	bool close();
	bool isOpened() const;

public:
	const std::string &name() const;

	sqlite3 *database(); // TODO: hide sqlite3 object inside

	void addStatement(sqlite3_stmt *statement);
	void removeStatement(sqlite3_stmt *statement);

public:
	void dumpError() const;

private:
	void finalizeOpenedStatements();

private:
	const std::string myName;
	sqlite3 *myDatabase;
	std::vector<sqlite3_stmt *> myStatements;

private: // disable copying:
	SQLiteConnection(const SQLiteConnection &);
	const SQLiteConnection &operator = (const SQLiteConnection &);
};


inline const std::string &SQLiteConnection::name() const { return myName; }
inline sqlite3 *SQLiteConnection::database() { return myDatabase; }

inline void SQLiteConnection::addStatement(sqlite3_stmt *statement) { myStatements.push_back(statement); }

inline void SQLiteConnection::removeStatement(sqlite3_stmt *statement) {
	std::vector<sqlite3_stmt *>::iterator it = std::find(myStatements.begin(), myStatements.end(), statement);
	if (it != myStatements.end()) {
		myStatements.erase(it);
	}
}

inline bool SQLiteConnection::isOpened() const { return myDatabase != 0; }


#endif /* __SQLITECONNECTION_H__ */
