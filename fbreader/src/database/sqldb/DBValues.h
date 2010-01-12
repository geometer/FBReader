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


#ifndef __DBVALUES_H__
#define __DBVALUES_H__

#include <string>
#include <shared_ptr.h>


class DBValue {

public:
	enum ValueType {
		DBNULL,
		DBINT,
		DBREAL,
		DBTEXT,
	};

	static shared_ptr<DBValue> create(ValueType type);

public:
	DBValue();
	virtual ~DBValue();

public: // to implement:
	virtual ValueType type() const = 0;
};

class DBNullValue : public DBValue {

friend class DBValue;

public:
	static const shared_ptr<DBValue> Instance; // the only Instance of DBNullValue class

private:
	DBNullValue();
	~DBNullValue();

public:
	ValueType type() const;
};

class DBIntValue : public DBValue {

friend class DBValue;

protected:
	DBIntValue();

public:
	DBIntValue(int value);
	~DBIntValue();

	ValueType type() const;

	int value() const;
	void setValue(int value);
	const DBIntValue &operator = (int value);

private:
	int myValue;
};

class DBRealValue : public DBValue {

friend class DBValue;

protected:
	DBRealValue();

public:
	DBRealValue(double value);
	~DBRealValue();

	ValueType type() const;

	double value() const;
	void setValue(double value);
	const DBRealValue &operator = (double value);

private:
	double myValue;
};

class DBTextValue : public DBValue {

friend class DBValue;

protected:
	DBTextValue();

public:
	DBTextValue(const std::string &value);
	~DBTextValue();

	ValueType type() const;

	const std::string &value() const;
	void setValue(const std::string &value);
	const DBTextValue &operator = (const std::string &value);

public:
	DBTextValue(const DBTextValue &value);
	const DBTextValue &operator = (const DBTextValue &value);

private:
	std::string myValue;
};



inline int DBIntValue::value() const { return myValue; }
inline void DBIntValue::setValue(int value) { myValue = value; }
inline const DBIntValue &DBIntValue::operator = (int value) { myValue = value; return *this; }

inline double DBRealValue::value() const { return myValue; }
inline void DBRealValue::setValue(double value) { myValue = value; }
inline const DBRealValue &DBRealValue::operator = (double value) { myValue = value; return *this; }

inline const std::string &DBTextValue::value() const { return myValue; }
inline void DBTextValue::setValue(const std::string &value) { myValue = value; }
inline const DBTextValue &DBTextValue::operator = (const std::string &value) { myValue = value; return *this; }

#endif /* __DBVALUES_H__ */

