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

#include <ZLNetworkManager.h>
#include <ZLNetworkUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "../NetworkLink.h"
#include "OPDSXMLParser.h"

#include "OPDSBookItem.h"
#include "OPDSCatalogItem.h"

#include "../tree/NetworkTreeFactory.h"

OPDSBookItem::OPDSBookItem(const OPDSLink &link, OPDSEntry &entry, std::string baseUrl, unsigned int index) :
	NetworkBookItem(
		link,
		entry.id()->uri(),
		index,
		entry.title(),
		getAnnotation(entry),
		entry.dcLanguage(),
		getDate(entry),
		getAuthors(entry),
		getTags(entry),
		entry.seriesTitle(),
		entry.seriesIndex(),
		getUrls(link, entry, baseUrl),
		getReferences(link, entry, baseUrl)
		) {
	myRelatedInfos = getRelatedUrls(link, entry, baseUrl);
	myInformationIsFull = false;
}

bool OPDSBookItem::isFullyLoaded() const {
	return myInformationIsFull || URLByType.find(URL_SINGLE_ENTRY) == URLByType.end();
}

class OPDSBookItemFullInfoLoader : public ZLNetworkRequest::Listener {
public:
	OPDSBookItemFullInfoLoader(OPDSBookItem &item, shared_ptr<ZLNetworkRequest> request, shared_ptr<ZLNetworkRequest::Listener> listener) :
		myItem(item), myListener(listener) {
		request->setListener(this);
		ZLNetworkManager::Instance().performAsync(request);
	}

	void finished(const std::string &error) {
		if (error.empty()) {
			myItem.myInformationIsFull = true;
		}
		myListener->finished(error);
	}
private:
	OPDSBookItem &myItem;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
};

void OPDSBookItem::loadFullInformation(shared_ptr<ZLNetworkRequest::Listener> listener) {
	if (myInformationIsFull) {
		listener->finished();
		return;
	}

	if (URLByType.find(URL_SINGLE_ENTRY) == URLByType.end()) {
		myInformationIsFull = true;
		listener->finished();
		return;
	}

	std::string url = URLByType[URL_SINGLE_ENTRY];
	shared_ptr<ZLNetworkRequest> request = ZLNetworkManager::Instance().createXMLParserRequest(
				url, new OPDSXMLParser(new FullEntryReader(*this, (const OPDSLink&)Link, url), true)
	);

	new OPDSBookItemFullInfoLoader(*this, request, listener);
}

std::vector<shared_ptr<NetworkItem> > OPDSBookItem::getRelatedCatalogsItems() const {
	std::vector<shared_ptr<NetworkItem> > items;
	for (std::size_t i = 0; i < myRelatedInfos.size(); ++i) {
		shared_ptr<RelatedUrlInfo> urlInfo = myRelatedInfos.at(i);
		if (!urlInfo->MimeType->weakEquals(*ZLMimeType::APPLICATION_ATOM_XML)) {
			continue;
			//TODO implement items for loading link in browser
		}
		UrlInfoCollection urlByType = URLByType;
		urlByType[URL_CATALOG] = urlInfo->Url;
		OPDSCatalogItem *item = new OPDSCatalogItem(static_cast<const OPDSLink&>(Link), urlInfo->Title, std::string(), urlByType);
		items.push_back(item);
	}
	return items;
}

std::string OPDSBookItem::getAnnotation(OPDSEntry &entry) {
	//TODO implement ATOMContent support (and return content)
	return entry.summary();
}

std::string OPDSBookItem::getDate(OPDSEntry &entry) {
	std::string date;
	if (!entry.dcIssued().isNull()) {
		date = entry.dcIssued()->getDateTime(true);
	}
	return date;
}

std::vector<NetworkBookItem::AuthorData> OPDSBookItem::getAuthors(OPDSEntry &entry) {
	std::vector<NetworkBookItem::AuthorData> authors;
	for (std::size_t i = 0; i < entry.authors().size(); ++i) {
		ATOMAuthor &author = *(entry.authors()[i]);
		NetworkBookItem::AuthorData authorData;
		std::string name = author.name();
		std::string lowerCased = ZLUnicodeUtil::toLower(name);
		static const std::string authorPrefix = "author:";
		std::size_t index = lowerCased.find(authorPrefix);
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
			ZLUnicodeUtil::utf8Trim(before);
			ZLUnicodeUtil::utf8Trim(after);
			authorData.SortKey = before;
			authorData.DisplayName = after + ' ' + before;
		} else {
			ZLUnicodeUtil::utf8Trim(name);
			index = name.rfind(' ');
			authorData.SortKey = name.substr(index + 1);
			authorData.DisplayName = name;
		}
		authors.push_back(authorData);
	}
	return authors;
}

std::vector<std::string> OPDSBookItem::getTags(OPDSEntry &entry) {
	std::vector<std::string> tags;
	for (std::size_t i = 0; i < entry.categories().size(); ++i) {
		ATOMCategory &category = *(entry.categories()[i]);
		tags.push_back(category.label());
	}
	return tags;
}

NetworkItem::UrlInfoCollection OPDSBookItem::getUrls(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl) {
	//TODO split urls and references in UrlInfoCollection, like it's implemented in FBReaderJ
	NetworkItem::UrlInfoCollection urlMap;
	for (std::size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string href = ZLNetworkUtil::url(baseUrl, link.href());
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = networkLink.relation(link.rel(), link.type());
		if (ZLStringUtil::stringStartsWith(rel, OPDSConstants::REL_IMAGE_PREFIX) || rel == OPDSConstants::REL_COVER) {
			if (urlMap[NetworkItem::URL_COVER].empty() && ZLMimeType::isImage(type)) {
				urlMap[NetworkItem::URL_COVER] = href;
			}
		} else if (rel == OPDSConstants::REL_THUMBNAIL || rel == OPDSConstants::REL_IMAGE_THUMBNAIL) {
			if (ZLMimeType::isImage(type)) {
				urlMap[NetworkItem::URL_COVER] = href;
			}
		} else if (type->weakEquals(*ZLMimeType::APPLICATION_ATOM_XML) &&
				   rel == ATOMConstants::REL_ALTERNATE &&
				   type->getParameter("type") == "entry") {
			urlMap[NetworkItem::URL_SINGLE_ENTRY] = href;
		}
	}
	return urlMap;
}

OPDSBookItem::RelatedUrlsList OPDSBookItem::getRelatedUrls(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl) {
	OPDSBookItem::RelatedUrlsList relatedUrlList;
	for (std::size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string href = ZLNetworkUtil::url(baseUrl, link.href());
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = networkLink.relation(link.rel(), link.type());
		if (rel == ATOMConstants::REL_RELATED) {
			relatedUrlList.push_back(new RelatedUrlInfo(link.title(), type, href));
		}
	}
	return relatedUrlList;
}

std::vector<shared_ptr<BookReference> >  OPDSBookItem::getReferences(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl) {
	//TODO split urls and references in UrlInfoCollection, like it's implemented in FBReaderJ
	std::vector<shared_ptr<BookReference> > references;
	for (std::size_t i = 0; i < entry.links().size(); ++i) {
		ATOMLink &link = *(entry.links()[i]);
		const std::string href = ZLNetworkUtil::url(baseUrl, link.href());
		shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
		const std::string &rel = networkLink.relation(link.rel(), link.type());
		const BookReference::Type referenceType = typeByRelation(rel);
		if (referenceType == BookReference::BUY) {
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
	return references;
}

BookReference::Format OPDSBookItem::formatByZLMimeType(const std::string &mimeType) {
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

BookReference::Type OPDSBookItem::typeByRelation(const std::string &rel) {
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

OPDSBookItem::FullEntryReader::FullEntryReader(OPDSBookItem &item, const OPDSLink &link, std::string url) :
	myItem(item), myLink(link), myUrl(url) {
}

void OPDSBookItem::FullEntryReader::processFeedEntry(shared_ptr<OPDSEntry> entry) {
	NetworkItem::UrlInfoCollection urlMap = OPDSBookItem::getUrls(myLink, *entry, myUrl);
	std::vector<shared_ptr<BookReference> > references = OPDSBookItem::getReferences(myLink, *entry, myUrl);
	for (NetworkItem::UrlInfoCollection::iterator it = urlMap.begin(); it != urlMap.end(); ++it) {
		myItem.URLByType[(*it).first] = (*it).second;
	}
	myItem.updateReferences(references);
	std::string summary = OPDSBookItem::getAnnotation(*entry);
	if (!summary.empty()) {
		myItem.Summary = summary;
	}
	myItem.myRelatedInfos = OPDSBookItem::getRelatedUrls(myLink, *entry, myUrl);
}

void OPDSBookItem::FullEntryReader::processFeedStart() {
}

void OPDSBookItem::FullEntryReader::processFeedMetadata(shared_ptr<OPDSFeedMetadata> /*feed*/) {
}

void OPDSBookItem::FullEntryReader::processFeedEnd() {
}

OPDSBookItem::RelatedUrlInfo::RelatedUrlInfo(const std::string &title, shared_ptr<ZLMimeType> mimeType, const std::string url) :
	Title(title), MimeType(mimeType), Url(url) { }


