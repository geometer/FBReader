/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
 */

#include <ZLNetworkUtil.h>
#include <ZLMimeType.h>
#include <stdio.h>
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
		const std::string &type = link.type();
		const std::string &rel = link.rel();
		if (rel == "search") {
			if (type == ZLMimeType::APPLICATION_ATOM_XML) {
				links[rel] = ZLNetworkUtil::url(myBaseURL, OpenSearchXMLReader::convertOpenSearchURL(href));
			} else if (type == ZLMimeType::APPLICATION_OPENSEARCH_XML) {
				OpenSearchXMLReader* osr = new OpenSearchXMLReader();
				shared_ptr<ZLXMLReader> zr = osr;
				ZLExecutionData::perform(ZLNetworkManager::Instance().createXMLParserRequest(ZLNetworkUtil::url(myBaseURL, href), zr));
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
		myBaseURL,
		feed->title(),
		summary,
		iconURL,
		links
	);
}


void OPDSLink::FeedReader::processFeedEnd() {
}

void OPDSLink::FeedReader::processFeedEntry(shared_ptr<OPDSEntry> entry) {
}


