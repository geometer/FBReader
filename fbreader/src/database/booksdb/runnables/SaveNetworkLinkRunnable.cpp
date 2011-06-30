/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqq
 */

#include "../DBRunnables.h"
#include <stdio.h>
#include "../../../network/NetworkLink.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"

SaveNetworkLinkRunnable::SaveNetworkLinkRunnable(DBConnection &connection) {
	myFindNetworkLinkId = SQLiteFactory::createCommand(BooksDBQuery::FIND_NETWORK_LINK_ID, connection, "@site_name", DBValue::DBTEXT);
	myAddNetworkLink    = SQLiteFactory::createCommand(BooksDBQuery::ADD_NETWORK_LINK, connection, "@title", DBValue::DBTEXT, "@site_name", DBValue::DBTEXT, "@summary", DBValue::DBTEXT, "@predefined_id", DBValue::DBTEXT, "@is_enabled", DBValue::DBINT);
	myUpdateNetworkLink    = SQLiteFactory::createCommand(BooksDBQuery::UPDATE_NETWORK_LINK, connection, "@title", DBValue::DBTEXT, "@site_name", DBValue::DBTEXT, "@summary", DBValue::DBTEXT, "@predefined_id", DBValue::DBTEXT, "@is_enabled", DBValue::DBINT, "@link_id", DBValue::DBINT);

	myAddNetworkLinkUrl    = SQLiteFactory::createCommand(BooksDBQuery::ADD_NETWORK_LINKURL, connection, "@key", DBValue::DBTEXT, "@link_id", DBValue::DBINT, "@url", DBValue::DBTEXT);
	myDeleteNetworkLinkUrls    = SQLiteFactory::createCommand(BooksDBQuery::DELETE_NETWORK_LINKURLS, connection, "@link_id", DBValue::DBINT);
}

bool SaveNetworkLinkRunnable::run() {
	((DBTextValue &) *myFindNetworkLinkId->parameter("@site_name").value()) = myNetworkLink->SiteName;
	shared_ptr<DBDataReader> reader = myFindNetworkLinkId->executeReader();
	if (reader.isNull() || !reader->next()) {
		return addNetworkLink(myNetworkLink);
	} else if (reader->textValue(1, std::string()) == std::string() || myNetworkLink->PredefinedId != std::string()) {
		return updateNetworkLink(myNetworkLink, reader->intValue(0));
	}
}

bool SaveNetworkLinkRunnable::addNetworkLink(const shared_ptr<NetworkLink> link) {
	((DBTextValue &) *myAddNetworkLink->parameter("@title").value()) = link->Title;
	((DBTextValue &) *myAddNetworkLink->parameter("@site_name").value()) = link->SiteName;
	((DBTextValue &) *myAddNetworkLink->parameter("@summary").value()) = link->Summary;
	((DBTextValue &) *myAddNetworkLink->parameter("@predefined_id").value()) = link->PredefinedId;
	((DBIntValue &) *myAddNetworkLink->parameter("@is_enabled").value()) = link->Enabled;
	shared_ptr<DBValue> dbLinkId = myAddNetworkLink->executeScalar();
	if (dbLinkId.isNull() || dbLinkId->type() != DBValue::DBINT || ((DBIntValue &) *dbLinkId).value() == 0) {
		return false;
	}

	bool allExecuted = true;
	for (std::map<std::string,std::string>::iterator it = link->myLinks.begin(); it != link->myLinks.end(); ++it) {
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@key").value()) = it->first;
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@url").value()) = it->second;
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@link_id").value()) = ((DBIntValue &) *dbLinkId).value();
		allExecuted = allExecuted && myAddNetworkLinkUrl->execute();
	}
	if (link->myLinks.count("icon") == 0 && link->Icon != std::string()) {
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@key").value()) = "icon";
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@url").value()) = link->Icon;
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@link_id").value()) = ((DBIntValue &) *dbLinkId).value();
		allExecuted = allExecuted && myAddNetworkLinkUrl->execute();
	}
	return allExecuted;
}

bool SaveNetworkLinkRunnable::updateNetworkLink(const shared_ptr<NetworkLink> link, int linkId) {
	((DBTextValue &) *myUpdateNetworkLink->parameter("@title").value()) = link->Title;
	((DBTextValue &) *myUpdateNetworkLink->parameter("@site_name").value()) = link->SiteName;
	((DBTextValue &) *myUpdateNetworkLink->parameter("@summary").value()) = link->Summary;
	((DBTextValue &) *myUpdateNetworkLink->parameter("@predefined_id").value()) = link->PredefinedId;
	((DBIntValue &) *myUpdateNetworkLink->parameter("@is_enabled").value()) = int(link->Enabled);
	((DBIntValue &) *myUpdateNetworkLink->parameter("@link_id").value()) = linkId;

	((DBIntValue &) *myDeleteNetworkLinkUrls->parameter("@link_id").value()) = linkId;
	if (!myDeleteNetworkLinkUrls->execute()) return false;

	bool allExecuted = true;
	for (std::map<std::string,std::string>::iterator it = link->myLinks.begin(); it != link->myLinks.end(); ++it) {
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@key").value()) = it->first;
		((DBTextValue &) *myAddNetworkLinkUrl->parameter("@url").value()) = it->second;
		((DBIntValue &) *myAddNetworkLinkUrl->parameter("@link_id").value()) = linkId;
		allExecuted = allExecuted && myAddNetworkLinkUrl->execute();
	}
	return allExecuted && myUpdateNetworkLink->execute();
}

void SaveNetworkLinkRunnable::setNetworkLink(shared_ptr<NetworkLink> link) {
	myNetworkLink = link;
}
