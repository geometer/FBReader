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


#include "DBCommandParameter.h"


DBCommandParameter::DBCommandParameter() 
	: myName(""), myValue(DBValue::create(DBValue::DBNULL)) {
}

DBCommandParameter::DBCommandParameter(const std::string &name) 
	: myName(name), myValue(DBValue::create(DBValue::DBNULL)) {
}

DBCommandParameter::DBCommandParameter(DBValue::ValueType type) 
	: myName(""), myValue(DBValue::create(type)) {
}

DBCommandParameter::DBCommandParameter(const std::string &name, DBValue::ValueType type) 
	: myName(name), myValue(DBValue::create(type)) {
}

DBCommandParameter::DBCommandParameter(shared_ptr<DBValue> value) 
	: myName(""), myValue(value) {
	if (value.isNull()) {
		myValue = DBValue::create(DBValue::DBNULL);
	}
}

DBCommandParameter::DBCommandParameter(const std::string &name, shared_ptr<DBValue> value) 
	: myName(name), myValue(value) {
	if (value.isNull()) {
		myValue = DBValue::create(DBValue::DBNULL);
	}
}

DBCommandParameter::DBCommandParameter(const DBCommandParameter &parameter) 
	: myName(parameter.name()), myValue(parameter.value()) {
}

const DBCommandParameter &DBCommandParameter::operator = (const DBCommandParameter &parameter) {
	myName = parameter.name();
	myValue = parameter.value();
	return *this;
}

