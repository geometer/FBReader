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

#include <ZLNetworkUtil.h>
#include <ZLMimeType.h>
#include <ZLExecutionData.h>
#include <ZLNetworkManager.h>

#include "OPDSLink_FeedReader.h"
#include "OpenSearchXMLReader.h"

OPDSLink::FeedReader::FeedReader(
	shared_ptr<NetworkLink>& link,
	const std::string &baseURL
) :
	myBaseURL(ZLNetworkUtil::hostFromUrl(baseURL)),
	myLink(link) {
}

void OPDSLink::FeedReader::processFeedStart() {
}

void OPDSLink::FeedReader::processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed) {
	std::map<std::string,std::string> links;
	for (size_t i = 0; i < feed->links().size(); ++i) {
		ATOMLink &link = *(feed->links()[i]);
		const std::string &href = link.href();
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = link.rel();
		if (rel == "search") {
			if (*type == *ZLMimeType::APPLICATION_ATOM_XML) {
				links[rel] = ZLNetworkUtil::url(myBaseURL, OpenSearchXMLReader::convertOpenSearchURL(href));
			} else if (*type == *ZLMimeType::APPLICATION_OPENSEARCH_XML) {
				OpenSearchXMLReader* osr = new OpenSearchXMLReader();
				shared_ptr<ZLXMLReader> zr = osr;
				ZLNetworkManager::Instance().perform(ZLNetworkManager::Instance().createXMLParserRequest(ZLNetworkUtil::url(myBaseURL, href), zr));
				links[rel] = osr->templateURL();
			}
		} else if (rel == "self") {
			links[NetworkLink::URL_MAIN] = ZLNetworkUtil::url(myBaseURL, href);
		} else {
			links[rel] = ZLNetworkUtil::url(myBaseURL, href);
		}
	}
	if (myBaseURL.empty() || feed->title().empty() || links[NetworkLink::URL_MAIN].empty()) {

		return;
	}
	std::string iconURL;
	if (feed->icon() != 0) {
		iconURL = feed->icon()->uri();
	}
	std::string summary;
	if (feed->summary() != "") {
		summary = feed->summary();
	} else {
		summary = feed->subtitle();
	}

	myLink = new OPDSLink(
		myBaseURL
	);

	myLink->setTitle(feed->title());
	myLink->setSummary(summary);
	myLink->setIcon(iconURL);
	myLink->setLinks(links);
}


void OPDSLink::FeedReader::processFeedEnd() {
}

void OPDSLink::FeedReader::processFeedEntry(shared_ptr<OPDSEntry>) {
}


