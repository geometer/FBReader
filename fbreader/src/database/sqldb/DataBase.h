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


#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <shared_ptr.h>

#include "DBConnection.h"
#include "DBRunnable.h"

class DataBase {

public:
	static std::string databaseDirName();

public:
	DataBase(shared_ptr<DBConnection> connection);
	virtual ~DataBase();

	DBConnection &connection() const;

protected:
	virtual bool executeAsTransaction(DBRunnable &runnable) = 0;

private:
	shared_ptr<DBConnection> myConnection;

private: // against copying:
	DataBase(const DataBase &);
	const DataBase &operator = (const DataBase &);
};

inline DataBase::DataBase(shared_ptr<DBConnection> connection) : myConnection(connection) { }

inline DBConnection &DataBase::connection() const { return *myConnection; }

#endif /* __DATABASE_H__ */
