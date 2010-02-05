/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

//#include <iostream>
#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLNetworkUtil.h>

#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"
#include "../NetworkLibraryItems.h"
#include "../NetworkAuthenticationManager.h"
#include "../opdsLink/OPDSLink.h"
#include "../opdsLink/OPDSCatalogItem.h"

NetworkOPDSFeedReader::NetworkOPDSFeedReader(const std::string &baseURL, NetworkOperationData &result, 
		const std::set<std::string> &ignoredFeeds, const std::set<std::string> &accountDependentFeeds) : 
	myBaseURL(baseURL), 
	myData(result), 
	myIndex(0), 
	myIgnoredFeeds(ignoredFeeds),
	myAccountDependentFeeds(accountDependentFeeds) {
}

void NetworkOPDSFeedReader::processFeedStart() {
}

void NetworkOPDSFeedReader::processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed) {
	for (size_t i = 0; i < feed->links().size(); ++i) {
		ATOMLink &link = *(feed->links()[i]);
		const std::string &href = link.href();
		const std::string &rel = link.rel();
		const std::string &type = link.type();
		if (type == OPDSConstants::MIME_APP_ATOM) {
			if (rel == "self") {
				//std::cerr << "<link type=\"application/atom+xml\" rel=\"self\" href=\"" << href << "\"/>" << std::endl;
			} else if (rel == "next") {
				myData.ResumeURI = href;
				//std::cerr << "<link type=\"application/atom+xml\" rel=\"next\" href=\"" << href << "\" title=\"Next Page\"/>" << std::endl;
			}
		}
	}
	myIndex = feed->getOpensearchStartIndex() - 1;
}


void NetworkOPDSFeedReader::processFeedEnd() {
}


void NetworkOPDSFeedReader::processFeedEntry(shared_ptr<OPDSEntry> entry) {
	if (entry.isNull()) {
		return;
	}
	if (myIgnoredFeeds.count(entry->id()->uri()) != 0) {
		return;
	}
	OPDSEntry &e = *entry;
	bool hasBookLink = false;
	for (size_t i = 0; i < e.links().size(); ++i) {
		ATOMLink &link = *(e.links()[i]);
		const std::string &rel = link.rel();
		const std::string &type = link.type();
		if (rel == OPDSConstants::REL_ACQUISITION || rel.empty()) {
			if (type == OPDSConstants::MIME_APP_EPUB
				|| type == OPDSConstants::MIME_APP_MOBI) {
				hasBookLink = true;
				break;
			}
		}
	}

	shared_ptr<NetworkLibraryItem> item;
	if (hasBookLink) {
		item = readBookItem(e);
	} else {
		item = readCatalogItem(e);
	}
	if (!item.isNull()) {
		myData.Items.push_back(item);
	}
}

shared_ptr<NetworkLibraryItem> NetworkOPDSFeedReader::readBookItem(OPDSEntry &entry) {
	shared_ptr<NetworkLibraryItem> bookPtr = new NetworkLibraryBookItem(entry.id()->uri(), myIndex++);
	NetworkLibraryBookItem &book = (NetworkLibraryBookItem &) *bookPtr;
	
	book.setAuthenticationManager(myData.Link.authenticationManager());

	book.setTitle(entry.title());
	book.setLanguage(entry.dcLanguage());
	if (!entry.dcIssued().isNull()) {
		book.setDate(entry.dcIssued()->getDateTime(true));
	}
	book.setAnnotation(entry.summary());

	for (size_t i = 0; i < entry.categories().size(); ++i) {
		ATOMCategory &category = *(entry.categories()[i]);
		book.tags().push_back(category.term());
	}

	for (size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string &href = link.href();
		const std::string &rel = link.rel();
		const std::string &type = link.type();
		if (rel == OPDSConstants::REL_COVER ||
				rel == OPDSConstants::REL_STANZA_COVER) {
			if (book.coverURL().empty() && 
					(type == OPDSConstants::MIME_IMG_PNG ||
					 type == OPDSConstants::MIME_IMG_JPEG)) {
				book.setCoverURL(href);
			}
		} else if (rel == OPDSConstants::REL_THUMBNAIL ||
							 rel == OPDSConstants::REL_STANZA_THUMBNAIL) {
			if (type == OPDSConstants::MIME_IMG_PNG ||
					type == OPDSConstants::MIME_IMG_JPEG) {
				book.setCoverURL(href);
			}
		} else if (rel == OPDSConstants::REL_ACQUISITION || rel.empty()) {
			if (type == OPDSConstants::MIME_APP_EPUB) {
				book.urlByType()[NetworkLibraryBookItem::BOOK_EPUB] = href;
			} else if (type == OPDSConstants::MIME_APP_MOBI) {
				book.urlByType()[NetworkLibraryBookItem::BOOK_MOBIPOCKET] = href;
			//} else if (type == OPDSConstants::MIME_APP_PDF) {
				//book.urlByType()[NetworkLibraryBookItem::BOOK_PDF] = href;
			}
		}
	}

	for (size_t i = 0; i < entry.authors().size(); ++i) {
		ATOMAuthor &author = *(entry.authors()[i]);
		NetworkLibraryBookItem::AuthorData authorData;
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
		book.authors().push_back(authorData);
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

	return bookPtr;
}

shared_ptr<NetworkLibraryItem> NetworkOPDSFeedReader::readCatalogItem(OPDSEntry &entry) {
	std::string coverURL;
	std::string url;
	bool urlIsAlternate = false;
	std::string htmlURL;
	for (size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string &href = link.href();
		const std::string &rel = link.rel();
		const std::string &type = link.type();
		if (rel == OPDSConstants::REL_COVER ||
				rel == OPDSConstants::REL_STANZA_COVER) {
			if (coverURL.empty() &&
					(type == OPDSConstants::MIME_IMG_PNG ||
					 type == OPDSConstants::MIME_IMG_JPEG)) {
				coverURL = href;
			}
		} else if (rel == OPDSConstants::REL_THUMBNAIL ||
							 rel == OPDSConstants::REL_STANZA_THUMBNAIL) {
			if (type == OPDSConstants::MIME_IMG_PNG ||
					type == OPDSConstants::MIME_IMG_JPEG) {
				coverURL = href;
			}
		} else if (rel == OPDSConstants::REL_ACQUISITION ||
							 rel == OPDSConstants::REL_ALTERNATE ||
							 rel.empty()) {
			if (type == OPDSConstants::MIME_APP_ATOM) {
				if (rel == OPDSConstants::REL_ALTERNATE) {
					if (url.empty()) {
						url = href;
						urlIsAlternate = true;
					}
				} else {
					url = href;
				}
			} else if (type == OPDSConstants::MIME_TEXT_HTML) {
				htmlURL = href;
			}
		}
	}

	if (url.empty() && htmlURL.empty()) {
		return 0;
	}

	if (!url.empty() && !urlIsAlternate) {
		htmlURL.erase();
	}

	bool dependsOnAccount = myAccountDependentFeeds.count(entry.id()->uri()) > 0;

	std::string annotation = entry.summary();
	annotation.erase(std::remove(annotation.begin(), annotation.end(), 0x09), annotation.end());
	annotation.erase(std::remove(annotation.begin(), annotation.end(), 0x0A), annotation.end());
	return new OPDSCatalogItem(
		(OPDSLink&)myData.Link,
		ZLNetworkUtil::url(myBaseURL, url),
		ZLNetworkUtil::url(myBaseURL, htmlURL),
		entry.title(),
		annotation,
		coverURL,
		dependsOnAccount
	);
}
