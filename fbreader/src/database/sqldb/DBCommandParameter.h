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


#ifndef __DBCOMMANDPARAMETERS_H__
#define __DBCOMMANDPARAMETERS_H__

#include <vector>

#include "DBValues.h"


class DBCommandParameter {

public:
	// create anonymous parameter with DBNULL value:
	DBCommandParameter();

	// create named parameter with DBNULL value:
	DBCommandParameter(const std::string &name);

	// create anonymous parameter with default value of specified type:
	DBCommandParameter(DBValue::ValueType type);

	// create named parameter with default value of specified type:
	DBCommandParameter(const std::string &name, DBValue::ValueType type);

	// create anonymous parameter with specified value:
	DBCommandParameter(shared_ptr<DBValue> value); 

	// create named parameter with specified value:
	DBCommandParameter(const std::string &name, shared_ptr<DBValue> value); 

public:
	const std::string &name() const;
	void setName(const std::string &name);

	shared_ptr<DBValue> value() const;
	void setValue(shared_ptr<DBValue> value);

	DBValue::ValueType type() const;

	bool hasName() const;

public: // implement copying:
	DBCommandParameter(const DBCommandParameter &par);
	const DBCommandParameter &operator = (const DBCommandParameter &par);

private:
	std::string myName;
	DBValue::ValueType myType;
	shared_ptr<DBValue> myValue;
};


inline const std::string &DBCommandParameter::name() const { return myName; }
inline void DBCommandParameter::setName(const std::string &name) { myName = name; }
inline shared_ptr<DBValue> DBCommandParameter::value() const { return myValue; }
inline void DBCommandParameter::setValue(shared_ptr<DBValue> value) { myValue = value; }
inline DBValue::ValueType DBCommandParameter::type() const { return myValue->type(); }
inline bool DBCommandParameter::hasName() const { return myName != ""; }


#endif /* __DBCOMMANDPARAMETERS_H__ */

