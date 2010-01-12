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


#include "SQLiteFactory.h"

#include "SQLiteCommand.h"


shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection) {
	return new SQLiteCommand(command, connection);
}

shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection, 
	const std::string &name1, DBValue::ValueType type1) {

	shared_ptr<DBCommand> cmd = createCommand(command, connection);
	cmd->parameters().push_back( DBCommandParameter(name1, type1) );

	return cmd;
}

shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection, 
	const std::string &name1, DBValue::ValueType type1,
	const std::string &name2, DBValue::ValueType type2) {

	shared_ptr<DBCommand> cmd = createCommand(command, connection);
	cmd->parameters().push_back( DBCommandParameter(name1, type1) );
	cmd->parameters().push_back( DBCommandParameter(name2, type2) );

	return cmd;
}

shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection, 
	const std::string &name1, DBValue::ValueType type1,
	const std::string &name2, DBValue::ValueType type2,
	const std::string &name3, DBValue::ValueType type3) {

	shared_ptr<DBCommand> cmd = createCommand(command, connection);
	cmd->parameters().push_back( DBCommandParameter(name1, type1) );
	cmd->parameters().push_back( DBCommandParameter(name2, type2) );
	cmd->parameters().push_back( DBCommandParameter(name3, type3) );

	return cmd;
}

shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection, 
	const std::string &name1, DBValue::ValueType type1,
	const std::string &name2, DBValue::ValueType type2,
	const std::string &name3, DBValue::ValueType type3,
	const std::string &name4, DBValue::ValueType type4) {

	shared_ptr<DBCommand> cmd = createCommand(command, connection);
	cmd->parameters().push_back( DBCommandParameter(name1, type1) );
	cmd->parameters().push_back( DBCommandParameter(name2, type2) );
	cmd->parameters().push_back( DBCommandParameter(name3, type3) );
	cmd->parameters().push_back( DBCommandParameter(name4, type4) );

	return cmd;
}

shared_ptr<DBCommand> SQLiteFactory::createCommand(const std::string &command, DBConnection &connection, 
	const std::string &name1, DBValue::ValueType type1,
	const std::string &name2, DBValue::ValueType type2,
	const std::string &name3, DBValue::ValueType type3,
	const std::string &name4, DBValue::ValueType type4,
	const std::string &name5, DBValue::ValueType type5) {

	shared_ptr<DBCommand> cmd = createCommand(command, connection);
	cmd->parameters().push_back( DBCommandParameter(name1, type1) );
	cmd->parameters().push_back( DBCommandParameter(name2, type2) );
	cmd->parameters().push_back( DBCommandParameter(name3, type3) );
	cmd->parameters().push_back( DBCommandParameter(name4, type4) );
	cmd->parameters().push_back( DBCommandParameter(name5, type5) );

	return cmd;
}


