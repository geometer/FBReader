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

#include <cstdlib>

#include <ZLStringUtil.h>
#include <ZLXMLNamespace.h>

#include "OPDSXMLParser.h"

static const std::string TAG_FEED = "feed";
static const std::string TAG_ENTRY = "entry";
static const std::string TAG_AUTHOR = "author";
static const std::string TAG_NAME = "name";
static const std::string TAG_URI = "uri";
static const std::string TAG_EMAIL = "email";
static const std::string TAG_ID = "id";
static const std::string TAG_CATEGORY = "category";
static const std::string TAG_LINK = "link";
static const std::string TAG_PUBLISHED = "published";
static const std::string TAG_SUMMARY = "summary";
static const std::string TAG_CONTENT = "content";
static const std::string TAG_SUBTITLE = "subtitle";
static const std::string TAG_TITLE = "title";
static const std::string TAG_UPDATED = "updated";
static const std::string TAG_PRICE = "price";
static const std::string TAG_ICON = "icon";

static const std::string TAG_HACK_SPAN = "span";

static const std::string DC_TAG_LANGUAGE = "language";
static const std::string DC_TAG_ISSUED = "issued";
static const std::string DC_TAG_PUBLISHER = "publisher";
static const std::string DC_TAG_FORMAT = "format";

static const std::string CALIBRE_TAG_SERIES = "series";
static const std::string CALIBRE_TAG_SERIES_INDEX = "series_index";

static const std::string OPENSEARCH_TAG_TOTALRESULTS = "totalResults";
static const std::string OPENSEARCH_TAG_ITEMSPERPAGE = "itemsPerPage";
static const std::string OPENSEARCH_TAG_STARTINDEX = "startIndex";

const std::string OPDSXMLParser::KEY_PRICE = "price";
const std::string OPDSXMLParser::KEY_CURRENCY = "currency";
const std::string OPDSXMLParser::KEY_FORMAT = "format";

static const std::string TAG_SEARCH_DESCRIPTION = "fbreader:advancedSearch";
static const std::string TAG_AUTHENTICATION = "fbreader:authentication";
static const std::string TAG_URL_REWRITING_RULES = "fbreader:urlRewritingRule";
static const std::string TAG_RELATION_ALIASES = "fbreader:relationAlias";

OPDSXMLParser::OPDSXMLParser(shared_ptr<OPDSFeedReader> feedReader) :myFeedReader(feedReader) {
	myState = START;
}

bool OPDSXMLParser::processNamespaces() const {
	return true;
}

void OPDSXMLParser::startElementHandler(const char *tag, const char **attributes) {
	std::map<std::string,std::string> attributeMap;
	while (*attributes != 0) {
		std::string name(*attributes++);
		if (*attributes == 0) {
			break;
		}
		std::string value(*attributes++);
		attributeMap.insert(std::make_pair(name, value));
	}

	switch (myState) {
		case START:
			if (testTag(ZLXMLNamespace::Atom, TAG_FEED, tag)) {
				myFeedReader->processFeedStart();
				myFeed = new OPDSFeedMetadata();
				myFeed->readAttributes(attributeMap);
				myState = FEED;
			}
			break;
		case FEED:
			if (testTag(ZLXMLNamespace::Atom, TAG_AUTHOR, tag)) {
				myAuthor = new ATOMAuthor();
				myAuthor->readAttributes(attributeMap);
				myState = F_AUTHOR;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_ID, tag)) {
				myId = new ATOMId();
				myId->readAttributes(attributeMap);
				myState = F_ID;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_ICON, tag)) {
				myIcon = new ATOMIcon();
				myIcon->readAttributes(attributeMap);
				myState = F_ICON;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
				myLink = new ATOMLink();
				myLink->readAttributes(attributeMap);
				myState = F_LINK;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_CATEGORY, tag)) {
				myCategory = new ATOMCategory();
				myCategory->readAttributes(attributeMap);
				myState = F_CATEGORY;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
				//myTitle = new ATOMTitle(); // TODO:implement ATOMTextConstruct & ATOMTitle
				//myTitle->readAttributes(attributeMap);
				myState = F_TITLE;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_SUBTITLE, tag)) {
				myState = F_SUBTITLE;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_SUMMARY, tag)) {
				myState = F_SUMMARY;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_UPDATED, tag)) {
				myUpdated = new ATOMUpdated();
				myUpdated->readAttributes(attributeMap);
				myState = F_UPDATED;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_ENTRY, tag)) {
				myEntry = new OPDSEntry();
				myEntry->readAttributes(attributeMap);
				mySummaryTagFound = false;
				myState = F_ENTRY;
			} else if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_TOTALRESULTS, tag)) {
				myState = OPENSEARCH_TOTALRESULTS;
			} else if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_ITEMSPERPAGE, tag)) {
				myState = OPENSEARCH_ITEMSPERPAGE;
			} else if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_STARTINDEX, tag)) {
				myState = OPENSEARCH_STARTINDEX;
			}
			break;
		case F_ENTRY:
			if (testTag(ZLXMLNamespace::Atom, TAG_AUTHOR, tag)) {
				myAuthor = new ATOMAuthor();
				myAuthor->readAttributes(attributeMap);
				myState = FE_AUTHOR;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_ID, tag)) {
				myId = new ATOMId();
				myId->readAttributes(attributeMap);
				myState = FE_ID;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_CATEGORY, tag)) {
				myCategory = new ATOMCategory();
				myCategory->readAttributes(attributeMap);
				myState = FE_CATEGORY;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_ICON, tag)) {
				myIcon = new ATOMIcon();
				myIcon->readAttributes(attributeMap);
				myState = FE_ICON;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
				myLink = new ATOMLink();
				myLink->readAttributes(attributeMap);
				myState = FE_LINK;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_PUBLISHED, tag)) {
				myPublished = new ATOMPublished();
				myPublished->readAttributes(attributeMap);
				myState = FE_PUBLISHED;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_SUMMARY, tag)) {
				//mySummary = new ATOMSummary(); // TODO:implement ATOMTextConstruct & ATOMSummary
				//mySummary->readAttributes(attributeMap);
				myState = FE_SUMMARY;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_CONTENT, tag)) {
				// ???
				myState = FE_CONTENT;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_SUBTITLE, tag)) {
				// ???
				myState = FE_SUBTITLE;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
				//myTitle = new ATOMTitle(); // TODO:implement ATOMTextConstruct & ATOMTitle
				//myTitle->readAttributes(attributeMap);
				myState = FE_TITLE;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_UPDATED, tag)) {
				myUpdated = new ATOMUpdated();
				myUpdated->readAttributes(attributeMap);
				myState = FE_UPDATED;
			} else if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_LANGUAGE, tag)) {
					myState = FE_DC_LANGUAGE;
			} else if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_ISSUED, tag)) {
				myState = FE_DC_ISSUED;
			} else if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_PUBLISHER, tag)) {
				myState = FE_DC_PUBLISHER;
			} else if (testTag(ZLXMLNamespace::CalibreMetadata, CALIBRE_TAG_SERIES, tag)) {
				myState = FE_CALIBRE_SERIES;
			} else if (testTag(ZLXMLNamespace::CalibreMetadata, CALIBRE_TAG_SERIES_INDEX, tag)) {
				myState = FE_CALIBRE_SERIES_INDEX;
			}
			break;
		case F_AUTHOR:
			if (testTag(ZLXMLNamespace::Atom, TAG_NAME, tag)) {
				myState = FA_NAME;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_URI, tag)) {
				myState = FA_URI;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_EMAIL, tag)) {
				myState = FA_EMAIL;
			}
			break;
		case FE_TITLE:
			// TODO: remove this temporary code
			// DON'T clear myBuffer
			return;
		case FE_LINK:
			if (testTag(ZLXMLNamespace::Opds, TAG_PRICE, tag)) {
				myLink->setUserData(KEY_CURRENCY, attributeMap["currencycode"]);
				myState = FEL_PRICE;
			} if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_FORMAT, tag)) {
				myState = FEL_FORMAT;
			}
			break;
		case FE_AUTHOR:
			if (testTag(ZLXMLNamespace::Atom, TAG_NAME, tag)) {
				myState = FEA_NAME;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_URI, tag)) {
				myState = FEA_URI;
			} else if (testTag(ZLXMLNamespace::Atom, TAG_EMAIL, tag)) {
				myState = FEA_EMAIL;
			}
			break;
		case FE_CONTENT:
			if (TAG_HACK_SPAN == tag || attributeMap["class"] == "price") {
				myState = FEC_HACK_SPAN;
			}
			break;
		default:
			break;
	}

	myBuffer.clear();
}

void OPDSXMLParser::endElementHandler(const char *tag) {
	ZLStringUtil::stripWhiteSpaces(myBuffer);

	switch (myState) {
		case START:
			break;
		case FEED:
			if (testTag(ZLXMLNamespace::Atom, TAG_FEED, tag)) {
				myFeedReader->processFeedMetadata(myFeed);
				myFeed.reset();
				myFeedReader->processFeedEnd();
				myState = START;
			}
			break;
		case F_ENTRY:
			if (testTag(ZLXMLNamespace::Atom, TAG_ENTRY, tag)) {
				myFeedReader->processFeedEntry(myEntry);
				myEntry.reset();
				myState = FEED;
			}
			break;
		case F_ID:
			if (testTag(ZLXMLNamespace::Atom, TAG_ID, tag)) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				myId->setUri(myBuffer);
				if (!myFeed.isNull()) {
					myFeed->setId(myId);
				}
				myId.reset();
				myState = FEED;
			}
			break;
		case F_ICON:
			if (testTag(ZLXMLNamespace::Atom, TAG_ICON, tag)) {
				myIcon->setUri(myBuffer);
				if (!myFeed.isNull()) {
					myFeed->setIcon(myIcon);
				}
				myIcon.reset();
				myState = FEED;
			}
			break;
		case F_LINK:
			if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
				if (!myFeed.isNull()) {
					myFeed->links().push_back(myLink);
				}
				myLink.reset();
				myState = FEED;
			}
			break;
		case F_CATEGORY:
			if (testTag(ZLXMLNamespace::Atom, TAG_CATEGORY, tag)) {
				if (!myFeed.isNull()) {
					myFeed->categories().push_back(myCategory);
				}
				myCategory.reset();
				myState = FEED;
			}
			break;
		case F_TITLE:
			if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
				// FIXME:title can be lost:buffer will be truncated, if there are extension tags inside the <title> tag
				// TODO:implement ATOMTextConstruct & ATOMTitle
				if (!myFeed.isNull()) {
					myFeed->setTitle(myBuffer);
				}
				myState = FEED;
			}
			break;
		case F_SUBTITLE:
			if (testTag(ZLXMLNamespace::Atom, TAG_SUBTITLE, tag)) {
				if (!myFeed.isNull()) {
					myFeed->setSubtitle(myBuffer);
				}
				myState = FEED;
			}
			break;
		case F_SUMMARY:
			if (testTag(ZLXMLNamespace::Atom, TAG_SUMMARY, tag)) {
				if (!myFeed.isNull()) {
					myFeed->setSummary(myBuffer);
				}
				myState = FEED;
			}
			break;
		case F_UPDATED:
			if (testTag(ZLXMLNamespace::Atom, TAG_UPDATED, tag)) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				ATOMDateConstruct::parse(myBuffer, *myUpdated);
				if (!myFeed.isNull()) {
					myFeed->setUpdated(myUpdated);
				}
				myUpdated.reset();
				myState = FEED;
			}
			break;
		case F_AUTHOR:
			if (testTag(ZLXMLNamespace::Atom, TAG_AUTHOR, tag)) {
				if (!myFeed.isNull()) {
					myFeed->authors().push_back(myAuthor);
				}
				myAuthor.reset();
				myState = FEED;
			}
			break;
		case FA_NAME:
			if (testTag(ZLXMLNamespace::Atom, TAG_NAME, tag)) {
				myAuthor->setName(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_NAME:
			if (testTag(ZLXMLNamespace::Atom, TAG_NAME, tag)) {
				myAuthor->setName(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FEL_PRICE:
			if (testTag(ZLXMLNamespace::Opds, TAG_PRICE, tag)) {
				myLink->setUserData(KEY_PRICE, myBuffer);
				myState = FE_LINK;
			}
			break;
		case FEL_FORMAT:
			if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_FORMAT, tag)) {
				myLink->setUserData(KEY_FORMAT, myBuffer);
				myState = FE_LINK;
			}
			break;
		case FA_URI:
			if (testTag(ZLXMLNamespace::Atom, TAG_URI, tag)) {
				myAuthor->setUri(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_URI:
			if (testTag(ZLXMLNamespace::Atom, TAG_URI, tag)) {
				myAuthor->setUri(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FA_EMAIL:
			if (testTag(ZLXMLNamespace::Atom, TAG_EMAIL, tag)) {
				myAuthor->setEmail(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_EMAIL:
			if (testTag(ZLXMLNamespace::Atom, TAG_EMAIL, tag)) {
				myAuthor->setEmail(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FE_AUTHOR:
			if (testTag(ZLXMLNamespace::Atom, TAG_AUTHOR, tag)) {
				myEntry->authors().push_back(myAuthor);
				myAuthor.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_ICON:
			if (testTag(ZLXMLNamespace::Atom, TAG_ICON, tag)) {
				myIcon->setUri(myBuffer);
				if (!myEntry.isNull()) {
					myEntry->setIcon(myIcon);
				}
				myIcon.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_ID:
			if (testTag(ZLXMLNamespace::Atom, TAG_ID, tag)) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				myId->setUri(myBuffer);
				myEntry->setId(myId);
				myId.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_CATEGORY:
			if (testTag(ZLXMLNamespace::Atom, TAG_CATEGORY, tag)) {
				myEntry->categories().push_back(myCategory);
				myCategory.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_LINK:
			if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
				myEntry->links().push_back(myLink);
				myLink.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_PUBLISHED:
			if (testTag(ZLXMLNamespace::Atom, TAG_PUBLISHED, tag)) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				ATOMDateConstruct::parse(myBuffer, *myPublished);
				myEntry->setPublished(myPublished);
				myPublished.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_SUMMARY:
			if (testTag(ZLXMLNamespace::Atom, TAG_SUMMARY, tag)) {
				// FIXME:summary can be lost:buffer will be truncated, if there are extension tags inside the <summary> tag
				// TODO:implement ATOMTextConstruct & ATOMSummary
				myEntry->setSummary(myBuffer);
				mySummaryTagFound = true;
				myState = F_ENTRY;
			}
			break;
		case FE_CONTENT:
			if (testTag(ZLXMLNamespace::Atom, TAG_CONTENT, tag)) {
				// TODO:check this accurately
				if (!mySummaryTagFound) {
					myEntry->setSummary(myBuffer);
				}
				myState = F_ENTRY;
			}
			break;
		case FEC_HACK_SPAN:
			myEntry->setUserData(KEY_PRICE, myBuffer);
			myState = FE_CONTENT;
			break;
		case FE_SUBTITLE:
			if (testTag(ZLXMLNamespace::Atom, TAG_SUBTITLE, tag)) {
				// TODO:check this accurately
				if (!mySummaryTagFound) {
					myEntry->setSummary(myBuffer);
				}
				myState = F_ENTRY;
			}
			break;
		case FE_TITLE:
			if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
				// FIXME:title can be lost:buffer will be truncated, if there are extension tags inside the <title> tag
				// TODO:implement ATOMTextConstruct & ATOMTitle
				myEntry->setTitle(myBuffer);
				myState = F_ENTRY;
			} else {
				// TODO: remove this temporary code
				// DON'T clear myBuffer
				return;
			}
			break;
		case FE_UPDATED:
			if (testTag(ZLXMLNamespace::Atom, TAG_UPDATED, tag)) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				ATOMDateConstruct::parse(myBuffer, *myUpdated);
				myEntry->setUpdated(myUpdated);
				myUpdated.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_DC_LANGUAGE:
			if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_LANGUAGE, tag)) {
				// FIXME:language can be lost:buffer will be truncated, if there are extension tags inside the <dc:language> tag
				myEntry->setDCLanguage(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_ISSUED:
			if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_ISSUED, tag)) {
				// FIXME:issued can be lost:buffer will be truncated, if there are extension tags inside the <dc:issued> tag
				DCDate *issued = new DCDate();
				ATOMDateConstruct::parse(myBuffer, *issued);
				myEntry->setDCIssued(issued);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_PUBLISHER:
			if (testTag(ZLXMLNamespace::DublinCoreTerms, DC_TAG_PUBLISHER, tag)) {
				// FIXME:publisher can be lost:buffer will be truncated, if there are extension tags inside the <dc:publisher> tag
				myEntry->setDCPublisher(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_CALIBRE_SERIES:
			if (testTag(ZLXMLNamespace::CalibreMetadata, CALIBRE_TAG_SERIES, tag)) {
				myEntry->setSeriesTitle(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_CALIBRE_SERIES_INDEX:
			if (testTag(ZLXMLNamespace::CalibreMetadata, CALIBRE_TAG_SERIES_INDEX, tag)) {
				myEntry->setSeriesIndex(atoi(myBuffer.c_str()));
				myState = F_ENTRY;
			}
			break;
		case OPENSEARCH_TOTALRESULTS:
			if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_TOTALRESULTS, tag)) {
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchTotalResults(number);
				}
				myState = FEED;
			}
			break;
		case OPENSEARCH_ITEMSPERPAGE:
			if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_ITEMSPERPAGE, tag)) {
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchItemsPerPage(number);
				}
				myState = FEED;
			}
			break;
		case OPENSEARCH_STARTINDEX:
			if (testTag(ZLXMLNamespace::OpenSearch, OPENSEARCH_TAG_STARTINDEX, tag)) {
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchStartIndex(number);
				}
				myState = FEED;
			}
			break;
	}

	myBuffer.clear();
}

void OPDSXMLParser::characterDataHandler(const char *data, size_t len) {
	myBuffer.append(data, len);
}
