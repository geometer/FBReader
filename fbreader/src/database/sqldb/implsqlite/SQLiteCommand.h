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


#ifndef __SQLITECOMMAND_H__
#define __SQLITECOMMAND_H__

#include <sqlite3.h>

#include "../DBCommand.h"

#include "SQLiteConnection.h"


/*
 * Command can contain parameters with following names:
 *     ?    - anonymous parameter
 *     @AAA - named parameter
 *
 * where AAA is alpha-numeric parameter name
 */
class SQLiteCommand : public DBCommand {

public:
	static std::string packCommand(const std::string &command);

public:
	SQLiteCommand(const std::string &command, DBConnection &connection);
	~SQLiteCommand();

	bool execute();
	shared_ptr<DBValue> executeScalar();
	shared_ptr<DBDataReader> executeReader();

public:
	void unlock();

	// TODO: hide sqlite3_stmt object inside
	std::vector<sqlite3_stmt *> &statements();
	const std::vector<sqlite3_stmt *> &statements() const;

	void dumpError() const;
	void dumpError(const std::string &msg) const;

	bool resetStatements();

private:

	struct BindParameter {
		size_t Position;
		std::string Name;

		BindParameter(size_t pos) : Position(pos), Name("") {}
		BindParameter(size_t pos, const std::string &name) : Position(pos), Name(name) {}

		bool hasName() const;
	};
	
	class BindParameterComparator {

	public:
		BindParameterComparator(const std::string &name);
		bool operator () (const SQLiteCommand::BindParameter &p) const;

	private:
		const std::string myName;
	};

	void prepareBindContext();
	bool bindParameters();
	bool bindParameterByName(const std::string &name, shared_ptr<DBValue> value);
	bool bindParameterByIndex(size_t index, shared_ptr<DBValue> value);
	bool bindParameter(sqlite3_stmt *statement, int number, shared_ptr<DBValue> value);
	bool prepareStatements(SQLiteConnection &conn);

	void finalizeStatements();
	
private:
	std::vector<sqlite3_stmt *> myStatements;
	std::vector<BindParameter> myBindContext;
	bool myLocked;

private: // disable copying:
	SQLiteCommand(const SQLiteCommand &);
	const SQLiteCommand &operator = (const SQLiteCommand &);
};


inline SQLiteCommand::SQLiteCommand(const std::string &command, DBConnection &connection) 
	: DBCommand(SQLiteCommand::packCommand(command), connection), myStatements(), myLocked(false) {}

inline void SQLiteCommand::unlock() { myLocked = false; }
inline std::vector<sqlite3_stmt *> &SQLiteCommand::statements() { return myStatements; }
inline const std::vector<sqlite3_stmt *> &SQLiteCommand::statements() const { return myStatements; }

inline bool SQLiteCommand::BindParameter::hasName() const { return Name.size() > 0; }

inline SQLiteCommand::BindParameterComparator::BindParameterComparator(const std::string &name) : myName(name) {}
inline bool SQLiteCommand::BindParameterComparator::operator () (const SQLiteCommand::BindParameter &p) const { return myName == p.Name; }

#endif /* __SQLITECOMMAND_H__ */
