/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLNetworkUtil.h>
#include <ZLMimeType.h>
#include <ZLExecutionData.h>
#include <ZLNetworkManager.h>

#include "OPDSLink_GenericFeedReader.h"
#include "OpenSearchXMLReader.h"

OPDSLink::GenericFeedReader::GenericFeedReader(
	std::vector<shared_ptr<NetworkLink> >& links
) :
	myLinks(links) {
}

void OPDSLink::GenericFeedReader::processFeedStart() {
}

void OPDSLink::GenericFeedReader::processFeedMetadata(shared_ptr<OPDSFeedMetadata>) {

}


void OPDSLink::GenericFeedReader::processFeedEnd() {
}

void OPDSLink::GenericFeedReader::processFeedEntry(shared_ptr<OPDSEntry> entry) {
	std::map<std::string,std::string> links;
	std::string iconURL;
	for (size_t i = 0; i < entry->links().size(); ++i) {
		ATOMLink &link = *(entry->links()[i]);
		const std::string &href = link.href();
		const std::string &rel = link.rel();
		if (rel == "search") {
			links[rel] = OpenSearchXMLReader::convertOpenSearchURL(href);
		} else if (rel == "") {
			links[NetworkLink::URL_MAIN] = href;
		} else if (rel == OPDSConstants::REL_LINK_SIGN_IN) {
			links[NetworkLink::URL_SIGN_IN] = href;
		} else if (rel == OPDSConstants::REL_LINK_SIGN_OUT) {
			links[NetworkLink::URL_SIGN_OUT] = href;
		} else if (rel == OPDSConstants::REL_LINK_SIGN_UP) {
			links[NetworkLink::URL_SIGN_UP] = href;
		} else if (rel == OPDSConstants::REL_LINK_TOPUP) {
			links[NetworkLink::URL_REFILL_ACCOUNT] = href;
		} else if (rel == OPDSConstants::REL_LINK_RECOVER_PASSWORD) {
			links[NetworkLink::URL_RECOVER_PASSWORD] = href;
		} else if (rel == OPDSConstants::REL_THUMBNAIL) {
			iconURL = href;
		} else {
			links[rel] = href;
		}
	}
	if (entry->title().empty() || links[NetworkLink::URL_MAIN].empty()) {
		return;
	}
	if (entry->id() == 0) {
		return;
	}
	std::string id = entry->id()->uri();
	std::string summary = entry->summary();
	std::string language = entry->dcLanguage();

	shared_ptr<NetworkLink> link = new OPDSLink(id.substr(25, -1));
	link->setTitle(entry->title());
	link->setSummary(summary);
	link->setLanguage(language);
	link->setIcon(iconURL);
	link->setLinks(links);
	link->setPredefinedId(id);
	link->setUpdated(entry->updated());
	link->init();
	myLinks.push_back(link);
}


