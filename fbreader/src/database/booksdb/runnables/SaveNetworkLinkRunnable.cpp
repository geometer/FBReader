/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include "../DBRunnables.h"
#include "../../../network/NetworkLink.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

SaveNetworkLinkRunnable::SaveNetworkLinkRunnable(DBConnection &connection) {
	myFindNetworkLinkId = SQLiteFactory::createCommand(BooksDBQuery::FIND_NETWORK_LINK_ID, connection, "@site_name", DBValue::DBTEXT);
	myAddNetworkLink    = SQLiteFactory::createCommand(BooksDBQuery::ADD_NETWORK_LINK, connection, "@title", DBValue::DBTEXT, "@site_name", DBValue::DBTEXT, "@summary", DBValue::DBTEXT, "@language", DBValue::DBTEXT, "@predefined_id", DBValue::DBTEXT, "@is_enabled", DBValue::DBINT);
	myUpdateNetworkLink    = SQLiteFactory::createCommand(BooksDBQuery::UPDATE_NETWORK_LINK, connection, "@title", DBValue::DBTEXT, "@summary", DBValue::DBTEXT, "@language", DBValue::DBTEXT, "@predefined_id", DBValue::DBTEXT, "@is_enabled", DBValue::DBINT, "@link_id", DBValue::DBINT);

	myFindNetworkLinkUrls = SQLiteFactory::createCommand(BooksDBQuery::FIND_NETWORK_LINKURLS, connection, "@link_id", DBValue::DBINT);
	myAddNetworkLinkUrl    = SQLiteFactory::createCommand(BooksDBQuery::ADD_NETWORK_LINKURL, connection, "@key", DBValue::DBTEXT, "@link_id", DBValue::DBINT, "@url", DBValue::DBTEXT, "@update_time", DBValue::DBINT);
	myUpdateNetworkLinkUrl    = SQLiteFactory::createCommand(BooksDBQuery::UPDATE_NETWORK_LINKURL, connection, "@key", DBValue::DBTEXT, "@link_id", DBValue::DBINT, "@url", DBValue::DBTEXT, "@update_time", DBValue::DBINT);
	myDeleteNetworkLinkUrl    = SQLiteFactory::createCommand(BooksDBQuery::DELETE_NETWORK_LINKURL, connection, "@key", DBValue::DBTEXT, "@link_id", DBValue::DBINT);
}

bool SaveNetworkLinkRunnable::run() {
	((DBTextValue &) *myFindNetworkLinkId->parameter("@site_name").value()) = myNetworkLink->SiteName;
	shared_ptr<DBDataReader> reader = myFindNetworkLinkId->executeReader();
	if (reader.isNull() || !reader->next()) {
		return addNetworkLink();
	} else if (myNetworkLink->getPredefinedId() != std::string()) {
		return updateNetworkLink(reader->intValue(0)) && updateNetworkLinkUrls(reader->intValue(0));
	} else if (reader->textValue(1, std::string()) == std::string()) {
		if (isAuto) {
			return updateNetworkLinkUrls(reader->intValue(0));
		} else {
			return updateNetworkLink(reader->intValue(0));
		}
	}
	return false;
}

bool SaveNetworkLinkRunnable::addNetworkLink() {
	((DBTextValue &) *myAddNetworkLink->parameter("@title").value()) = myNetworkLink->getTitle();
	((DBTextValue &) *myAddNetworkLink->parameter("@site_name").value()) = myNetworkLink->SiteName;
	((DBTextValue &) *myAddNetworkLink->parameter("@summary").value()) = myNetworkLink->getSummary();
	((DBTextValue &) *myAddNetworkLink->parameter("@language").value()) = myNetworkLink->getLanguage();
	((DBTextValue &) *myAddNetworkLink->parameter("@predefined_id").value()) = myNetworkLink->getPredefinedId();
	((DBIntValue &) *myAddNetworkLink->parameter("@is_enabled").value()) = myNetworkLink->isEnabled();
	shared_ptr<DBValue> dbLinkId = myAddNetworkLink->executeScalar();
	if (dbLinkId.isNull() || dbLinkId->type() != DBValue::DBINT || ((DBIntValue &) *dbLinkId).value() == 0) {
		return false;
	}

	bool allExecuted = true;
	std::map<std::string,std::string> tempLinks = myNetworkLink->getLinks();
	if (myNetworkLink->getIcon() != std::string()) {
		tempLinks["icon"] = myNetworkLink->getIcon();
	}
	long t = 0;
	if (myNetworkLink->getUpdated() != 0) {
		t = myNetworkLink->getUpdated()->getLongSeconds_stupid();
	}
	for (std::map<std::string,std::string>::iterator it = tempLinks.begin(); it != tempLinks.end(); ++it) {
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@key").value()) = it->first;
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@url").value()) = it->second;
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@link_id").value()) = ((DBIntValue &) *dbLinkId).value();
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@update_time").value()) = t;
		allExecuted = allExecuted && myAddNetworkLinkUrl->execute();
	}
	return allExecuted;
}

bool SaveNetworkLinkRunnable::updateNetworkLink(int linkId) {
	((DBTextValue &) *myUpdateNetworkLink->parameter("@title").value()) = myNetworkLink->getTitle();
	((DBTextValue &) *myUpdateNetworkLink->parameter("@summary").value()) = myNetworkLink->getSummary();
	((DBTextValue &) *myUpdateNetworkLink->parameter("@language").value()) = myNetworkLink->getLanguage();
	((DBTextValue &) *myUpdateNetworkLink->parameter("@predefined_id").value()) = myNetworkLink->getPredefinedId();
	((DBIntValue &) *myUpdateNetworkLink->parameter("@is_enabled").value()) = myNetworkLink->isEnabled();
	((DBIntValue &) *myUpdateNetworkLink->parameter("@link_id").value()) = linkId;

	return myUpdateNetworkLink->execute();
}

bool SaveNetworkLinkRunnable::updateNetworkLinkUrls(int linkId) {
	bool allExecuted = true;
	((DBIntValue &) *myFindNetworkLinkUrls->parameter("@link_id").value()) = linkId;
	shared_ptr<DBDataReader> reader = myFindNetworkLinkUrls->executeReader();
	std::map<std::string,std::string> linksToCheck = myNetworkLink->getLinks();
	if (myNetworkLink->getIcon() != std::string()) {
		linksToCheck["icon"] = myNetworkLink->getIcon();
	}
	long t = 0;
	if (myNetworkLink->getUpdated() != 0) {
		t = myNetworkLink->getUpdated()->getLongSeconds_stupid();
	}
	while (reader->next()) {
		if (reader->type(0) != DBValue::DBTEXT || reader->type(1) != DBValue::DBTEXT) {
			return false;
		}
		std::string key = reader->textValue(0, std::string());
		std::string url = reader->textValue(1, std::string());
		if (linksToCheck.count(key) == 0) {
			((DBTextValue &) *myDeleteNetworkLinkUrl->parameter("@key").value()) = key;
			((DBIntValue &) *myDeleteNetworkLinkUrl->parameter("@link_id").value()) = linkId;
			allExecuted = allExecuted && myDeleteNetworkLinkUrl->execute();
		} else {
			((DBTextValue &) *myUpdateNetworkLinkUrl->parameter("@key").value()) = key;
			((DBTextValue &) *myUpdateNetworkLinkUrl->parameter("@url").value()) = linksToCheck[key];
			((DBIntValue &) *myUpdateNetworkLinkUrl->parameter("@link_id").value()) = linkId;
			((DBIntValue &) *myUpdateNetworkLinkUrl->parameter("@update_time").value()) = t;
			linksToCheck.erase(key);
			allExecuted = allExecuted && myUpdateNetworkLinkUrl->execute();
		}
	}

	for (std::map<std::string,std::string>::iterator it = linksToCheck.begin(); it != linksToCheck.end(); ++it) {
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@key").value()) = it->first;
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@url").value()) = it->second;
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@link_id").value()) = linkId;
		allExecuted = allExecuted && myAddNetworkLinkUrl->execute();
	}
	return allExecuted;
}

void SaveNetworkLinkRunnable::setNetworkLink(NetworkLink* link) {
	myNetworkLink = link;
}
