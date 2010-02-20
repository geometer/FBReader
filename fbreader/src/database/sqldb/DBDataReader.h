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

#ifndef __DBDATAREADER_H__
#define __DBDATAREADER_H__

#include "DBValues.h"

class DBDataReader {

public:
	DBDataReader();
	virtual ~DBDataReader();

public: // to implement:
	virtual bool next() = 0;
	virtual bool reset() = 0;
	virtual void close() = 0;

	virtual size_t columnsNumber() const = 0;
	
	virtual DBValue::ValueType type(size_t column) const = 0;
	virtual shared_ptr<DBValue> value(size_t column) const = 0;

	virtual int intValue(size_t column) const = 0;
	virtual double realValue(size_t column) const = 0;
	virtual std::string textValue(size_t column, const std::string &defaultValue) const = 0;

public:
	bool isDBNull(size_t column) const;
	bool isInt(size_t column) const;
	bool isReal(size_t column) const;
	bool isText(size_t column) const;
};

inline bool DBDataReader::isDBNull(size_t column) const { return type(column) == DBValue::DBNULL; }
inline bool DBDataReader::isInt(size_t column) const { return type(column) == DBValue::DBINT; }
inline bool DBDataReader::isReal(size_t column) const { return type(column) == DBValue::DBREAL; }
inline bool DBDataReader::isText(size_t column) const { return type(column) == DBValue::DBTEXT; }

#endif /* __DBDATAREADER_H__ */

