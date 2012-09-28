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
	for (size_t i = 0; i < feed->links().size(); ++i) {
		ATOMLink &link = *(feed->links()[i]);
		const std::string &href = link.href();
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = myLink.relation(link.rel(), link.type());
		if (type == ZLMimeType::APPLICATION_ATOM_XML) {
			if (rel == "self") {
			} else if (rel == "next") {
				myData.ResumeURI = href;
			}
		}
	}
	myOpenSearchStartIndex = feed->getOpensearchStartIndex() - 1;
}


void NetworkOPDSFeedReader::processFeedEnd() {
	for (size_t i = 0; i < myData.Items.size(); ++i) {
		NetworkItem &item = *myData.Items[i];
		if (!item.isInstanceOf(NetworkBookItem::TYPE_ID)) {
			continue;
		}
		NetworkBookItem &book = (NetworkBookItem&) item;
		book.Index += myOpenSearchStartIndex;
	}
}


static BookReference::Format formatByZLMimeType(const std::string &mimeType) {
	shared_ptr<ZLMimeType> type = ZLMimeType::get(mimeType);
	if (type == ZLMimeType::APPLICATION_FB2_ZIP) {
		return BookReference::FB2_ZIP;
	} else if (type == ZLMimeType::APPLICATION_EPUB_ZIP) {
		return BookReference::EPUB;
	} else if (type == ZLMimeType::APPLICATION_MOBIPOCKET_EBOOK) {
		return BookReference::MOBIPOCKET;
	}
	return BookReference::NONE;
}

static BookReference::Type typeByRelation(const std::string &rel) {
	if (rel == OPDSConstants::REL_ACQUISITION || rel == OPDSConstants::REL_ACQUISITION_OPEN || rel.empty()) {
		return BookReference::DOWNLOAD_FULL;
	} else if (rel == OPDSConstants::REL_ACQUISITION_SAMPLE) {
		return BookReference::DOWNLOAD_DEMO;
	} else if (rel == OPDSConstants::REL_ACQUISITION_CONDITIONAL) {
		return BookReference::DOWNLOAD_FULL_CONDITIONAL;
	} else if (rel == OPDSConstants::REL_ACQUISITION_SAMPLE_OR_FULL) {
		return BookReference::DOWNLOAD_FULL_OR_DEMO;
	} else if (rel == OPDSConstants::REL_ACQUISITION_BUY) {
		return BookReference::BUY;
	} else {
		return BookReference::UNKNOWN;
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
	for (size_t i = 0; i < e.links().size(); ++i) {
		ATOMLink &link = *(e.links()[i]);
		const std::string &type = link.type();
		const std::string &rel = myLink.relation(link.rel(), type);
		if (rel == OPDSConstants::REL_ACQUISITION ||
				rel == OPDSConstants::REL_ACQUISITION_OPEN ||
				rel == OPDSConstants::REL_ACQUISITION_SAMPLE ||
				rel == OPDSConstants::REL_ACQUISITION_BUY ||
				rel == OPDSConstants::REL_ACQUISITION_CONDITIONAL ||
				rel == OPDSConstants::REL_ACQUISITION_SAMPLE_OR_FULL ||
				(rel.empty() && formatByZLMimeType(type) != BookReference::NONE)) {
			hasBookLink = true;
			break;
		}
	}

	shared_ptr<NetworkItem> item;
	if (hasBookLink) {
		item = readBookItem(e);
	} else {
		item = readCatalogItem(e);
	}
	if (!item.isNull()) {
		myData.Items.push_back(item);
	}
}

shared_ptr<NetworkItem> NetworkOPDSFeedReader::readBookItem(OPDSEntry &entry) {
	std::string date;
	if (!entry.dcIssued().isNull()) {
		date = entry.dcIssued()->getDateTime(true);
	}

	std::vector<std::string> tags;
	for (size_t i = 0; i < entry.categories().size(); ++i) {
		ATOMCategory &category = *(entry.categories()[i]);
		tags.push_back(category.term());
	}

	NetworkItem::UrlInfoCollection urlMap;
	std::vector<shared_ptr<BookReference> > references;
	for (size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string &href = link.href();
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = myLink.relation(link.rel(), link.type());
		const BookReference::Type referenceType = typeByRelation(rel);
		if (rel == OPDSConstants::REL_COVER) {
			if (urlMap[NetworkItem::URL_COVER].empty() && ZLMimeType::isImage(type)) {
				urlMap[NetworkItem::URL_COVER] = href;
			}
		} else if (rel == OPDSConstants::REL_THUMBNAIL) {
			if (ZLMimeType::isImage(type)) {
				urlMap[NetworkItem::URL_COVER] = href;
			}
		} else if (referenceType == BookReference::BUY) {
			std::string price = BuyBookReference::price(
				link.userData(OPDSXMLParser::KEY_PRICE),
				link.userData(OPDSXMLParser::KEY_CURRENCY)
			);
			if (price.empty()) {
				price = BuyBookReference::price(
					entry.userData(OPDSXMLParser::KEY_PRICE),
					entry.userData(OPDSXMLParser::KEY_CURRENCY)
				);
			}
			if (type == ZLMimeType::TEXT_HTML) {
				references.push_back(new BuyBookReference(
					href, BookReference::NONE, BookReference::BUY_IN_BROWSER, price
				));
			} else {
				BookReference::Format format = formatByZLMimeType(link.userData(OPDSXMLParser::KEY_FORMAT));
				if (format != BookReference::NONE) {
					references.push_back(new BuyBookReference(
						href, format, BookReference::BUY, price
					));
				}
			}
		} else if (referenceType != BookReference::UNKNOWN) {
			BookReference::Format format = formatByZLMimeType(link.type());
			if (format != BookReference::NONE) {
				references.push_back(new BookReference(href, format, referenceType));
			}
		}
	}

	std::vector<NetworkBookItem::AuthorData> authors;
	for (size_t i = 0; i < entry.authors().size(); ++i) {
		ATOMAuthor &author = *(entry.authors()[i]);
		NetworkBookItem::AuthorData authorData;
		std::string name = author.name();
		std::string lowerCased = ZLUnicodeUtil::toLower(name);
		static const std::string authorPrefix = "author:";
		size_t index = lowerCased.find(authorPrefix);
		if (index != std::string::npos) {
			name = name.substr(index + authorPrefix.size());
		} else {
			static const std::string authorsPrefix = "authors:";
			index = lowerCased.find(authorsPrefix);
			if (index != std::string::npos) {
				name = name.substr(index + authorsPrefix.size());
			}
		}
		index = name.find(',');
		if (index != std::string::npos) {
			std::string before = name.substr(0, index);
			std::string after = name.substr(index + 1);
			ZLStringUtil::stripWhiteSpaces(before);
			ZLStringUtil::stripWhiteSpaces(after);
			authorData.SortKey = before;
			authorData.DisplayName = after + ' ' + before;
		} else {
			ZLStringUtil::stripWhiteSpaces(name);
			index = name.rfind(' ');
			authorData.SortKey = name.substr(index + 1);
			authorData.DisplayName = name;
		}
		authors.push_back(authorData);
	}

	//entry.dcPublisher();
	//entry.updated();
	//entry.published();
	/*for (size_t i = 0; i < entry.contributors().size(); ++i) {
		ATOMContributor &contributor = *(entry.contributors()[i]);
		std::cerr << "\t\t<contributor>" << std::endl;
		std::cerr << "\t\t\t<name>"  << contributor.name()  << "</name>"  << std::endl;
		if (!contributor.uri().empty())   std::cerr << "\t\t\t<uri>"   << contributor.uri()   << "</uri>"   << std::endl;
		if (!contributor.email().empty()) std::cerr << "\t\t\t<email>" << contributor.email() << "</email>" << std::endl;
		std::cerr << "\t\t</contributor>" << std::endl;
	}*/
	//entry.rights();

	NetworkBookItem *book = new NetworkBookItem(
		myData.Link,
		entry.id()->uri(),
		myIndex++,
		entry.title(),
		entry.summary(),
		entry.dcLanguage(),
		date,
		authors,
		tags,
		entry.seriesTitle(),
		entry.seriesIndex(),
		urlMap,
		references
	);

	return book;
}

shared_ptr<NetworkItem> NetworkOPDSFeedReader::readCatalogItem(OPDSEntry &entry) {
	std::string coverURL;
	std::string url;
	bool urlIsAlternate = false;
	std::string htmlURL;
	std::string litresRel;
	std::string litresType;
	int catalogFlags = NetworkCatalogItem::FLAGS_DEFAULT;
	for (size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string &href = link.href();
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = myLink.relation(link.rel(), link.type());
		if (ZLMimeType::isImage(type)) {
			if (rel == OPDSConstants::REL_THUMBNAIL ||
					(coverURL.empty() && rel == OPDSConstants::REL_COVER)) {
				coverURL = href;
			}
		} else if (type == ZLMimeType::APPLICATION_ATOM_XML) {
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
		} else if (type == ZLMimeType::TEXT_HTML) {
			if (rel == OPDSConstants::REL_ACQUISITION ||
					rel == ATOMConstants::REL_ALTERNATE ||
					rel.empty()) {
				htmlURL = href;
			}
		} else if (type->weakEquals(*ZLMimeType::APPLICATION_LITRES_XML)) {
			url = href;
			litresRel = rel;
			litresType = type->getParameter("type");
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
	urlMap[NetworkItem::URL_CATALOG] = ZLNetworkUtil::url(myBaseURL, url);
	urlMap[NetworkItem::URL_HTML_PAGE] = ZLNetworkUtil::url(myBaseURL, htmlURL);

	if (!litresType.empty() || !litresRel.empty()) {
		return LitResUtil::createLitResNode(litresType, litresRel, myData.Link, entry.title(), annotation, urlMap, dependsOnAccount);
	} else {
		return new OPDSCatalogItem(
			(OPDSLink&)myData.Link,
			entry.title(),
			annotation,
			urlMap,
			dependsOnAccount ? NetworkCatalogItem::SIGNED_IN : NetworkCatalogItem::AlWAYS,
			catalogFlags
		);
	}
}
