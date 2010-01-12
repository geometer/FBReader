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


#ifndef __DBCOMMAND_H__
#define __DBCOMMAND_H__

#include "DBCommandParameter.h"
#include "DBDataReader.h"
#include "DBConnection.h"

class DBCommand {

public:
	DBCommand(const std::string &command, DBConnection &connection);
	virtual ~DBCommand();

	std::vector<DBCommandParameter> &parameters();
	const std::vector<DBCommandParameter> &parameters() const;

	DBCommandParameter &parameter(const std::string &name);
	const DBCommandParameter &parameters(const std::string &name) const;

	const std::string &commandString() const;

protected:
	const DBConnection &connection() const;

public: // to implement:
	virtual bool execute() = 0;
	virtual shared_ptr<DBValue> executeScalar() = 0;
	virtual shared_ptr<DBDataReader> executeReader() = 0;

private: // against copying:
	DBCommand(const DBCommand &);
	const DBCommand &operator = (const DBCommand &);

private:
	std::vector<DBCommandParameter> myParameters;
	const std::string myCommandString;
	const DBConnection &myConnection;
};


inline std::vector<DBCommandParameter> &DBCommand::parameters() { return myParameters; }
inline const std::vector<DBCommandParameter> &DBCommand::parameters() const { return myParameters; }

inline const std::string &DBCommand::commandString() const { return myCommandString; }
inline const DBConnection &DBCommand::connection() const { return myConnection; }


#endif /* __DBCOMMAND_H__ */

