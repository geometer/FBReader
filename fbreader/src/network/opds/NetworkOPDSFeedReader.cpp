/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLMimeType.h>

#include "NetworkOPDSFeedReader.h"
#include "OPDSCatalogItem.h"
#include "OPDSXMLParser.h"

#include "../NetworkOperationData.h"
#include "../NetworkItems.h"
#include "../BookReference.h"
#include "OPDSBookItem.h"

#include "../litres/LitResUtil.h"


NetworkOPDSFeedReader::NetworkOPDSFeedReader(
	const OPDSLink &link,
	const std::string &baseURL,
	NetworkOperationData &result
) :
	myLink(link),
	myBaseURL(baseURL),
	myData(result),
	myIndex(0),
	myOpenSearchStartIndex(0) {
}

void NetworkOPDSFeedReader::processFeedStart() {
}

void NetworkOPDSFeedReader::processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed) {
	for (std::size_t i = 0; i < feed->links().size(); ++i) {
		ATOMLink &link = *(feed->links()[i]);
		const std::string &href = ZLNetworkUtil::url(myBaseURL, link.href());
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = myLink.relation(link.rel(), link.type());
		if (type->weakEquals(*ZLMimeType::APPLICATION_ATOM_XML)) {
			if (rel == "self") {
			} else if (rel == "next") {
				myData.ResumeURI = href;
			}
		}
	}
	myOpenSearchStartIndex = feed->getOpensearchStartIndex() - 1;
}


void NetworkOPDSFeedReader::processFeedEnd() {
	for (std::size_t i = 0; i < myData.Items.size(); ++i) {
		NetworkItem &item = *myData.Items[i];
		if (!item.isInstanceOf(NetworkBookItem::TYPE_ID)) {
			continue;
		}
		NetworkBookItem &book = (NetworkBookItem&) item;
		book.Index += myOpenSearchStartIndex;
	}
}

void NetworkOPDSFeedReader::processFeedEntry(shared_ptr<OPDSEntry> entry) {
	if (entry.isNull()) {
		return;
	}

	std::map<std::string,OPDSLink::FeedCondition>::const_iterator it = myLink.myFeedConditions.find(entry->id()->uri());
	if (it != myLink.myFeedConditions.end() && it->second == OPDSLink::CONDITION_NEVER) {
		return;
	}
	OPDSEntry &e = *entry;
	bool hasBookLink = false;
	for (std::size_t i = 0; i < e.links().size(); ++i) {
		ATOMLink &link = *(e.links()[i]);
		const std::string &type = link.type();
		const std::string &rel = myLink.relation(link.rel(), type);
		if (rel == OPDSConstants::REL_ACQUISITION ||
				rel == OPDSConstants::REL_ACQUISITION_OPEN ||
				rel == OPDSConstants::REL_ACQUISITION_SAMPLE ||
				rel == OPDSConstants::REL_ACQUISITION_BUY ||
				rel == OPDSConstants::REL_ACQUISITION_CONDITIONAL ||
				rel == OPDSConstants::REL_ACQUISITION_SAMPLE_OR_FULL ||
				(rel.empty() && OPDSBookItem::formatByZLMimeType(type) != BookReference::NONE)) {
			hasBookLink = true;
			break;
		}
	}

	shared_ptr<NetworkItem> item;
	if (hasBookLink) {
		item = new OPDSBookItem(myLink, e, myBaseURL, myIndex++);
	} else {
		item = readCatalogItem(e);
	}
	if (!item.isNull()) {
		myData.Items.push_back(item);
	}
}

shared_ptr<NetworkItem> NetworkOPDSFeedReader::readCatalogItem(OPDSEntry &entry) {
	std::string coverURL;
	std::string url;
	bool urlIsAlternate = false;
	std::string htmlURL;
	std::string litresRel;
	shared_ptr<ZLMimeType> litresMimeType;
	int catalogFlags = NetworkCatalogItem::FLAGS_DEFAULT;
	for (std::size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string &href = ZLNetworkUtil::url(myBaseURL, link.href());
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = myLink.relation(link.rel(), link.type());
		if (ZLMimeType::isImage(type)) {
			if (rel == OPDSConstants::REL_THUMBNAIL || rel == OPDSConstants::REL_IMAGE_THUMBNAIL) {
				coverURL = href;
			} else if (coverURL.empty() && (rel == OPDSConstants::REL_COVER || ZLStringUtil::stringStartsWith(rel, OPDSConstants::REL_IMAGE_PREFIX))) {
				coverURL = href;
			}
		} else if (type->weakEquals(*ZLMimeType::APPLICATION_ATOM_XML)) {
			if (rel == ATOMConstants::REL_ALTERNATE) {
				if (url.empty()) {
					url = href;
					urlIsAlternate = true;
				}
			} else {
				url = href;
				urlIsAlternate = false;
				if (rel == OPDSConstants::REL_CATALOG_AUTHOR) {
					catalogFlags &= !NetworkCatalogItem::FLAG_SHOW_AUTHOR;
				}
			}
		} else if (type->weakEquals(*ZLMimeType::TEXT_HTML)) {
			if (rel == OPDSConstants::REL_ACQUISITION ||
					rel == ATOMConstants::REL_ALTERNATE ||
					rel.empty()) {
				htmlURL = href;
			}
		} else if (type->weakEquals(*ZLMimeType::APPLICATION_LITRES_XML)) {
			url = href;
			litresRel = rel;
			litresMimeType = type;
		}
	}

	if (url.empty() && htmlURL.empty()) {
		return 0;
	}

	if (!url.empty() && !urlIsAlternate) {
		htmlURL.erase();
	}

	std::map<std::string,OPDSLink::FeedCondition>::const_iterator it =
		myLink.myFeedConditions.find(entry.id()->uri());
	bool dependsOnAccount =
		it != myLink.myFeedConditions.end() &&
		it->second == OPDSLink::CONDITION_SIGNED_IN;

	std::string annotation = entry.summary();
	annotation.erase(std::remove(annotation.begin(), annotation.end(), 0x09), annotation.end());
	annotation.erase(std::remove(annotation.begin(), annotation.end(), 0x0A), annotation.end());
	NetworkItem::UrlInfoCollection urlMap;
	urlMap[NetworkItem::URL_COVER] = coverURL;
	urlMap[NetworkItem::URL_CATALOG] = url;
	urlMap[NetworkItem::URL_HTML_PAGE] = htmlURL;

	if (!litresMimeType.isNull()) {
		return LitResUtil::createLitResNode(litresMimeType, litresRel, myData.Link, entry.title(), annotation, urlMap, dependsOnAccount);
	}
	return new OPDSCatalogItem(
		(OPDSLink&)myData.Link,
		entry.title(),
		annotation,
		urlMap,
		dependsOnAccount ? NetworkCatalogItem::SIGNED_IN : NetworkCatalogItem::ALWAYS,
		catalogFlags
	);
}
