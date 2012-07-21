/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __DBRUNNABLES_H__
#define __DBRUNNABLES_H__

#include "../sqldb/DBConnection.h"
#include "../sqldb/DBCommand.h"
#include "../sqldb/DBRunnable.h"

#include "NetworkDBQuery.h"

#include "../../network/NetworkLink.h"
#include "../../network/opds/OPDSLink.h"

class InitNetworkDBRunnable : public DBRunnable {

public:
	InitNetworkDBRunnable(DBConnection &connection);
	bool run();

private:
	DBConnection &myConnection;
};

class ClearNetworkDBRunnable : public DBRunnable {

public:
	ClearNetworkDBRunnable(DBConnection &connection);
	bool run();

private:
	DBConnection &myConnection;
};

class SaveNetworkLinkRunnable : public DBRunnable {

public:
	SaveNetworkLinkRunnable(DBConnection &connection);
	bool run();
	void setNetworkLink(const NetworkLink &link);

private:
	bool addNetworkLink();
	bool updateNetworkLink(int linkId);
	bool updateNetworkLinkUrls(int linkId);

private:
	const NetworkLink *myNetworkLink;

	shared_ptr<DBCommand> myFindNetworkLinkId;
	shared_ptr<DBCommand> myAddNetworkLink;
	shared_ptr<DBCommand> myUpdateNetworkLink;

	shared_ptr<DBCommand> myFindNetworkLinkUrls;
	shared_ptr<DBCommand> myAddNetworkLinkUrl;
	shared_ptr<DBCommand> myUpdateNetworkLinkUrl;
	shared_ptr<DBCommand> myDeleteNetworkLinkUrl;

};

inline InitNetworkDBRunnable::InitNetworkDBRunnable(DBConnection &connection) : myConnection(connection) {}
inline ClearNetworkDBRunnable::ClearNetworkDBRunnable(DBConnection &connection) : myConnection(connection) {}

#endif /* __DBRUNNABLES_H__ */
