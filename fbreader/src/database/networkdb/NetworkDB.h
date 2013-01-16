/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKDB_H__
#define __NETWORKDB_H__

#include <set>
#include <map>
#include <deque>

#include "../sqldb/implsqlite/SQLiteDataBase.h"
#include "DBRunnables.h"

class NetworkDB : public SQLiteDataBase {

public:
	static const std::string DATABASE_NAME;

	static NetworkDB &Instance();

private:
	static shared_ptr<NetworkDB> ourInstance;

	NetworkDB(const std::string &path);

public:
	virtual ~NetworkDB();

public:
	bool initDatabase();
	bool isInitialized() const;
	bool clearDatabase();

	bool saveNetworkLink(shared_ptr<NetworkLink> link);
	bool loadNetworkLinks(std::vector<shared_ptr<NetworkLink> >& links);
	bool deleteNetworkLink(const std::string &siteName);

private:
	void initCommands();

private:
	bool myInitialized;

	shared_ptr<DBCommand> myLoadNetworkLinks;
	shared_ptr<DBCommand> myFindNetworkLinkId;
	shared_ptr<DBCommand> myDeleteNetworkLink;
	shared_ptr<DBCommand> myDeleteNetworkLinkUrls;
	shared_ptr<DBCommand> myLoadNetworkLinkUrls;

	shared_ptr<SaveNetworkLinkRunnable> mySaveNetworkLink;

private: // disable copying
	NetworkDB(const NetworkDB &);
	const NetworkDB &operator = (const NetworkDB &);
};


inline bool NetworkDB::isInitialized() const { return myInitialized; }

#endif /* __NETWORKDB_H__ */
