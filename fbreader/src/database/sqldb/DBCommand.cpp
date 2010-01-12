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


#include "DBCommand.h"
#include <algorithm>

DBCommand::DBCommand(const std::string &command, DBConnection &connection)
	: myParameters()
	, myCommandString(command) 
	, myConnection(connection) {
}

DBCommand::~DBCommand() {
}


class ParameterPredicate {

public:
	ParameterPredicate(const std::string &name);
	bool operator () (const DBCommandParameter &p);
private:
	const std::string &myName;
};

ParameterPredicate::ParameterPredicate(const std::string &name) : myName(name) {}

bool ParameterPredicate::operator () (const DBCommandParameter &p) {
	return p.name() == myName;
}


DBCommandParameter &DBCommand::parameter(const std::string &name) {
	std::vector<DBCommandParameter>::iterator it = std::find_if(myParameters.begin(), myParameters.end(), ParameterPredicate(name));
	return *it;
}

const DBCommandParameter &DBCommand::parameters(const std::string &name) const {
	std::vector<DBCommandParameter>::const_iterator it = std::find_if(myParameters.begin(), myParameters.end(), ParameterPredicate(name));
	return *it;
}

