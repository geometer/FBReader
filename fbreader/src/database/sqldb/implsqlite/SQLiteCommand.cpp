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
#include <algorithm>

#include <ZLLogger.h>
#include <ZLStringUtil.h>

#include "SQLiteCommand.h"

#include "SQLiteConnection.h"
#include "SQLiteDataReader.h"


std::string SQLiteCommand::packCommand(const std::string &command) {
	static const char _spaces[] = " \t\n";
	std::string stripped = command;
	ZLStringUtil::stripWhiteSpaces(stripped);
	
	size_t pos = 0;
	while (true) {
		pos = stripped.find_first_of(_spaces, pos);
		if (pos == std::string::npos) {
			break;
		}
		stripped[pos++] = ' ';
		const size_t next = stripped.find_first_not_of(_spaces, pos);
		if (pos != std::string::npos && next > pos) {
			stripped.erase(pos, next - pos);
		}
	}
	return stripped;
}


SQLiteCommand::~SQLiteCommand() {
	SQLiteConnection &con = (SQLiteConnection &) connection();
	if (con.isOpened() && myStatements.size() != 0) {
		finalizeStatements();
	}
}


bool SQLiteCommand::execute() {
	ZLLogger::Instance().println("sqlite", "execute: " + commandString());

	SQLiteConnection &con = (SQLiteConnection &) connection();
	if (!con.isOpened()) {
		myStatements.clear();
		return false;
	}
	if (!prepareStatements(con)) {
		return false;
	}
	std::vector<sqlite3_stmt *>::iterator it = myStatements.begin();
	std::vector<sqlite3_stmt *>::iterator end = myStatements.end();
	while (true) {
		int res = sqlite3_step(*it);
		switch (res) {
		case SQLITE_DONE:
			if (++it == end) {
				resetStatements();
				return true;
			}
			break;
		case SQLITE_OK:
		case SQLITE_ROW:
			break;
		default:
			dumpError();
			finalizeStatements();
			return false;
		}
	}
}

shared_ptr<DBValue> SQLiteCommand::executeScalar() {
	ZLLogger::Instance().println("sqlite", "executeScalar: " + commandString());

	SQLiteConnection &con = (SQLiteConnection &) connection();
	if (!con.isOpened()) {
		myStatements.clear();
		return 0;
	}
	if (!prepareStatements(con)) {
		return 0;
	}
	std::vector<sqlite3_stmt *>::iterator it = myStatements.begin();
	std::vector<sqlite3_stmt *>::iterator end = myStatements.end();
	while (true) {
		int res = sqlite3_step(*it);
		switch (res) {
		case SQLITE_DONE:
			if (++it == end) {
				resetStatements();
				return 0;
			}
			break;
		case SQLITE_OK:
			break;
		case SQLITE_ROW: {
				shared_ptr<DBValue> val = SQLiteDataReader::makeDBValue(*it, /* column = */ 0);
				resetStatements();
				return val;
			}
		default:
			dumpError();
			finalizeStatements();
			return 0;
		}
	}
}

shared_ptr<DBDataReader> SQLiteCommand::executeReader() {
	ZLLogger::Instance().println("sqlite", "executeReader: " + commandString());

	SQLiteConnection &con = (SQLiteConnection &) connection();
	if (!con.isOpened()) {
		myStatements.clear();
		return 0;
	}
	if (!prepareStatements(con)) {
		return 0;
	}
	myLocked = true;
	return new SQLiteDataReader(*this);
}


bool SQLiteCommand::prepareStatements(SQLiteConnection &conn) {
	sqlite3 *db = conn.database();
	if (myLocked) {
		return false;
	}
	if (myStatements.size() != 0) {
		const size_t size = myStatements.size();
		int res = SQLITE_OK;
		for (size_t i = 0; i < size && res == SQLITE_OK; ++i) {
			res = sqlite3_reset(myStatements[i]);
		}
		if (res == SQLITE_OK) {
			bindParameters();
			return true;
		}
		finalizeStatements();
	}
	const std::string sql = commandString();
	const int length = -1;
	const char *tail = sql.c_str();
	while (true) {
		sqlite3_stmt *statement;
		int res = sqlite3_prepare_v2(db, tail, length, &statement, &tail);
		if (res != SQLITE_OK) {
			dumpError();
			finalizeStatements();
			return false;
		}
		if (statement == 0) {
			break;
		}
		myStatements.push_back(statement);
		conn.addStatement(statement);
	}
	if (!bindParameters()) {
		finalizeStatements();
		return false;
	}
	return true;
}


void SQLiteCommand::prepareBindContext() {
	if (myBindContext.size() > 0) {
		return;
	}

	size_t number = 0;

	for (size_t i = 0; i < myStatements.size(); ++i) {
		sqlite3_stmt *statement = myStatements[i];
		const int count = sqlite3_bind_parameter_count(statement);
		for (int j = 1; j <= count; ++j) {
			++number;
			const char *name = sqlite3_bind_parameter_name(statement, j);
			if (name == 0) {
				myBindContext.push_back(BindParameter(number));
			} else {
				const std::string namestr(name);
				if (std::find_if(myBindContext.begin(), myBindContext.end(), BindParameterComparator(namestr)) == myBindContext.end()) {
					myBindContext.push_back(BindParameter(number, namestr));
				}
			}
		}
	}
}


bool SQLiteCommand::bindParameters() {
	prepareBindContext();

	std::vector<DBCommandParameter> &params = parameters();

	bool res = true;
	const size_t size = params.size();
	for (size_t i = 0; i < size; ++i) {
		DBCommandParameter &p = params[i];
		if (p.hasName()) {
			const std::string &name = p.name();
			if (!bindParameterByName(name, p.value())) {
				res = false;
			}
		} else if (i < myBindContext.size()) {
			BindParameter &bp = myBindContext[i];
			if (myBindContext[i].hasName()) {
				if (!bindParameterByName(bp.Name, p.value())) {
					res = false;
				}
			} else {
				if (!bindParameterByIndex(bp.Position, p.value())) {
					res = false;
				}
			}
		} else {
			res = false;
		}
	}
	return res;
}


bool SQLiteCommand::bindParameterByName(const std::string &name, shared_ptr<DBValue> value) {
	const size_t size = myStatements.size();
	bool res = true;
	bool binded = false;
	for (size_t i = 0; i < size; ++i) {
		sqlite3_stmt *statement = myStatements[i];
		const int index = sqlite3_bind_parameter_index(statement, name.c_str());
		if (index == 0) {
			continue;
		}
		binded = true;
		if (!bindParameter(statement, index, value)) {
			res = false;
		}
	}
	if (!binded) {
		dumpError("parameter \"" + name + "\" is not found");
	}
	return res;
}

bool SQLiteCommand::bindParameterByIndex(size_t index, shared_ptr<DBValue> value) {
	if (index == 0) {
		return true;
	}
	const size_t size = myStatements.size();
	int number = index;
	for (size_t i = 0; i < size; ++i) {
		sqlite3_stmt *statement = myStatements[i];
		const int count = sqlite3_bind_parameter_count(statement);
		if (number > count) {
			number -= count;
			continue;
		}
		return bindParameter(statement, number, value);
	}
	return true;
}

bool SQLiteCommand::bindParameter(sqlite3_stmt *statement, int number, shared_ptr<DBValue> value) {
	DBValue::ValueType type = (value.isNull()) ? (DBValue::DBNULL) : (value->type());
	int res;
	switch (type) {
	case DBValue::DBNULL:
		res = sqlite3_bind_null(statement, number);
		break;
	case DBValue::DBINT:
		res = sqlite3_bind_int(statement, number, ((DBIntValue &) *value).value());
		break;
	case DBValue::DBREAL:
		res = sqlite3_bind_double(statement, number, ((DBRealValue &) *value).value());
		break;
	case DBValue::DBTEXT:
		res = sqlite3_bind_text(statement, number, ((DBTextValue &) *value).value().c_str(), -1 /* zero-terminated string */, SQLITE_TRANSIENT);
		break;
	default:
		return false;
	}
	if (res != SQLITE_OK) {
		dumpError();
	}
	return res == SQLITE_OK;
}


void SQLiteCommand::finalizeStatements() {
	SQLiteConnection &con = (SQLiteConnection &) connection();
	const size_t size = myStatements.size();
	for (size_t i = 0; i < size; ++i) {
		sqlite3_stmt *statement = myStatements[i];
		con.removeStatement(statement);
		const int res = sqlite3_finalize(statement);
		if (res != SQLITE_OK) {
			dumpError();
		}
	}
	myStatements.clear();
}


void SQLiteCommand::dumpError() const { 
	static const size_t cmdlimit = 114;
	((SQLiteConnection &) connection()).dumpError(); 
	const std::string &cmd = commandString();
	if (cmd.length() > cmdlimit) {
		std::cerr << "SQLITE IMPLEMENTATION ERROR: in command \"" << cmd.substr(0, cmdlimit - 3) << "...\"" << std::endl;
	} else {
		std::cerr << "SQLITE IMPLEMENTATION ERROR: in command \"" << cmd << "\"" << std::endl;
	}
}

void SQLiteCommand::dumpError(const std::string &msg) const {
	static const size_t cmdlimit = 129;
	std::cerr << "SQLITE ERROR: " << msg << std::endl;
	const std::string &cmd = commandString();
	if (cmd.length() > cmdlimit) {
		std::cerr << "SQLITE ERROR: in command \"" << cmd.substr(0, cmdlimit - 3) << "...\"" << std::endl;
	} else {
		std::cerr << "SQLITE ERROR: in command \"" << cmd << "\"" << std::endl;
	}
}

bool SQLiteCommand::resetStatements() {
	if (myStatements.size() == 0) {
		return true;
	}
	const size_t size = myStatements.size();
	int res = SQLITE_OK;
	for (size_t i = 0; i < size && res == SQLITE_OK; ++i) {
		res = sqlite3_reset(myStatements[i]);
	}
	if (res == SQLITE_OK) {
		return true;
	}
	dumpError();
	finalizeStatements();
	return false;
}

