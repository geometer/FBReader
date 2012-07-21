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

#include <ZLibrary.h>
#include <ZLDir.h>

#include "NetworkDB.h"

shared_ptr<NetworkDB> NetworkDB::ourInstance = 0;

const std::string NetworkDB::DATABASE_NAME = "network.db";

NetworkDB &NetworkDB::Instance() {
	if (ourInstance.isNull()) {
		ZLFile dir(databaseDirName());
		dir.directory(true);
		ZLFile file(databaseDirName() + ZLibrary::FileNameDelimiter + DATABASE_NAME);
		ourInstance = new NetworkDB(file.physicalFilePath());
		ourInstance->initDatabase();
	}
	return *ourInstance;
}

NetworkDB::NetworkDB(const std::string &path) : SQLiteDataBase(path), myInitialized(false) {
	initCommands();
}

NetworkDB::~NetworkDB() {
}

bool NetworkDB::initDatabase() {
	if (isInitialized()) {
		return true;
	}

	if (!open()) {
		return false;
	}

	myInitialized = true;

	shared_ptr<DBRunnable> runnable = new InitNetworkDBRunnable(connection());
	if (!executeAsTransaction(*runnable)) {
		myInitialized = false;
		close();
		return false;
	}

	return true;
}

void NetworkDB::initCommands() {
	myLoadNetworkLinks = SQLiteFactory::createCommand(NetworkDBQuery::LOAD_NETWORK_LINKS, connection());
	myLoadNetworkLinkUrls = SQLiteFactory::createCommand(NetworkDBQuery::LOAD_NETWORK_LINKURLS, connection(), "@link_id", DBValue::DBINT);
	myFindNetworkLinkId = SQLiteFactory::createCommand(NetworkDBQuery::FIND_NETWORK_LINK_ID, connection(), "@site_name", DBValue::DBTEXT);
	myDeleteNetworkLinkUrls = SQLiteFactory::createCommand(NetworkDBQuery::DELETE_NETWORK_LINKURLS, connection(), "@link_id", DBValue::DBINT);
	myDeleteNetworkLink = SQLiteFactory::createCommand(NetworkDBQuery::DELETE_NETWORK_LINK, connection(), "@link_id", DBValue::DBINT);

	mySaveNetworkLink = new SaveNetworkLinkRunnable(connection());
}

bool NetworkDB::clearDatabase() {
	if (!isInitialized()) {
		return false;
	}
	shared_ptr<DBRunnable> runnable = new ClearNetworkDBRunnable(connection());
	return executeAsTransaction(*runnable);
}


bool NetworkDB::saveNetworkLink(const NetworkLink &link) {
	if (!isInitialized()) {
		return false;
	}
	mySaveNetworkLink->setNetworkLink(link);
	bool result = executeAsTransaction(*mySaveNetworkLink);
	return result;
}

bool NetworkDB::loadNetworkLinks(std::vector<shared_ptr<NetworkLink> >& links) {
	shared_ptr<DBDataReader> reader = myLoadNetworkLinks->executeReader();

	links.clear();

	while (reader->next()) {
		if (reader->type(0) != DBValue::DBINT) {/* link_id */
			return false;
		}
		std::map<std::string,std::string> linkUrls;
		((DBIntValue &) *myLoadNetworkLinkUrls->parameter("@link_id").value()) = reader->intValue(0);
		shared_ptr<DBDataReader> urlreader = myLoadNetworkLinkUrls->executeReader();
		long t = 0;
		while (urlreader->next()) {
			linkUrls[urlreader->textValue(0, std::string())] = urlreader->textValue(1, std::string());
			t = urlreader->intValue(2);
		}
		shared_ptr<ATOMUpdated> atomUpdated = new ATOMUpdated();
		atomUpdated->setLongSeconds_stupid(t);
		std::string iconUrl;
		if (linkUrls .count("icon") != 0) {
			iconUrl = linkUrls["icon"];
			linkUrls.erase("icon");
		}
		std::string siteName = reader->textValue(2, std::string());
		std::string predId = reader->textValue(5, std::string());
		std::string title = reader->textValue(1, std::string());
		std::string summary = reader->textValue(3, std::string());
		std::string language = reader->textValue(4, std::string());
		bool isEnabled = reader->intValue(6) == 1;

		shared_ptr<NetworkLink> link = new OPDSLink(siteName);
		link->setTitle(title);
		link->setSummary(summary);
		link->setLanguage(language);
		link->setIcon(iconUrl);
		link->setLinks(linkUrls);
		link->setPredefinedId(predId);
		link->setEnabled(isEnabled);
		link->setUpdated(atomUpdated);
		links.push_back(link);
	}
	return true;
}

bool NetworkDB::deleteNetworkLink(const std::string &siteName){
	((DBTextValue &) *myFindNetworkLinkId->parameter("@site_name").value()) = siteName;
	shared_ptr<DBDataReader> reader = myFindNetworkLinkId->executeReader();
	if (reader.isNull() || !reader->next()) {
		return false;
	}
	int linkId = reader->intValue(0);
	((DBIntValue &) *myDeleteNetworkLink->parameter("@link_id").value()) = linkId;
	((DBIntValue &) *myDeleteNetworkLinkUrls->parameter("@link_id").value()) = linkId;
	return myDeleteNetworkLinkUrls->execute() && myDeleteNetworkLink->execute();

}
