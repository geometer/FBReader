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


#ifndef __SQLITEFACTORY_H__
#define __SQLITEFACTORY_H__

#include "../DBConnection.h"
#include "../DBCommand.h"

class SQLiteFactory {

private:
	SQLiteFactory();

public:
	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection);

	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection, 
		const std::string &name1, DBValue::ValueType type1);

	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection, 
		const std::string &name1, DBValue::ValueType type1, const std::string &name2, DBValue::ValueType type2);

	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection, 
		const std::string &name1, DBValue::ValueType type1, const std::string &name2, DBValue::ValueType type2,
		const std::string &name3, DBValue::ValueType type3);

	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection, 
		const std::string &name1, DBValue::ValueType type1, const std::string &name2, DBValue::ValueType type2,
		const std::string &name3, DBValue::ValueType type3, const std::string &name4, DBValue::ValueType type4);

	static shared_ptr<DBCommand> createCommand(const std::string &command, DBConnection &connection, 
		const std::string &name1, DBValue::ValueType type1, const std::string &name2, DBValue::ValueType type2,
		const std::string &name3, DBValue::ValueType type3, const std::string &name4, DBValue::ValueType type4,
		const std::string &name5, DBValue::ValueType type5);
};


#endif /* __SQLITEFACTORY_H__ */
