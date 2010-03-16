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

#include <cstdlib>

#include <ZLStringUtil.h>

#include "OPDSXMLParser.h"
#include "../../constants/XMLNamespace.h"

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

OPDSXMLParser::OPDSXMLParser(shared_ptr<OPDSFeedReader> feedReader) :myFeedReader(feedReader) {
	myState = START;
}

bool OPDSXMLParser::processNamespaces() const {
	return true;
}

void OPDSXMLParser::namespaceListChangedHandler() {
	myDublinCoreNamespaceId.erase();
	myAtomNamespaceId.erase();
	myOpenSearchNamespaceId.erase();
	myCalibreNamespaceId.erase();
	myOpdsNamespaceId.erase();

	const std::map<std::string,std::string> &nsMap = namespaces();
	for (std::map<std::string,std::string>::const_iterator it = nsMap.begin(); it != nsMap.end(); ++it) {
		if (it->first.empty()) {
			continue;
		}
		if (it->second == XMLNamespace::DublinCoreTerms) {
			myDublinCoreNamespaceId = it->first;
		} else if (it->second == XMLNamespace::Atom) {
			myAtomNamespaceId = it->first;
		} else if (it->second == XMLNamespace::OpenSearch) {
			myOpenSearchNamespaceId = it->first;
		} else if (it->second == XMLNamespace::CalibreMetadata) {
			myCalibreNamespaceId = it->first;
		} else if (it->second == XMLNamespace::Opds) {
			myOpdsNamespaceId = it->first;
		}
	}
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

	std::string tagPrefix;
	std::string tagName = tag;
	const int index = tagName.find(':');
	if (index != -1) {
		tagPrefix = tagName.substr(0, index);
		tagName.erase(0, index + 1);
	}
		
	switch (myState) {
		case START:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_FEED) {
				myFeedReader->processFeedStart();
				myFeed = new OPDSFeedMetadata();
				myFeed->readAttributes(attributeMap);
				myState = FEED;
			}
			break;
		case FEED:
			if (tagPrefix == myAtomNamespaceId) {
				if (tagName == TAG_AUTHOR) {
					myAuthor = new ATOMAuthor();
					myAuthor->readAttributes(attributeMap);
					myState = F_AUTHOR;
				} else if (tagName == TAG_ID) {
					myId = new ATOMId();
					myId->readAttributes(attributeMap);
					myState = F_ID;
				} else if (tagName == TAG_LINK) {
					myLink = new ATOMLink();
					myLink->readAttributes(attributeMap);
					myState = F_LINK;
				} else if (tagName == TAG_CATEGORY) {
					myCategory = new ATOMCategory();
					myCategory->readAttributes(attributeMap);
					myState = F_CATEGORY;
				} else if (tagName == TAG_TITLE) {
					//myTitle = new ATOMTitle(); // TODO:implement ATOMTextConstruct & ATOMTitle
					//myTitle->readAttributes(attributeMap);
					myState = F_TITLE;
				} else if (tagName == TAG_UPDATED) {
					myUpdated = new ATOMUpdated();
					myUpdated->readAttributes(attributeMap);
					myState = F_UPDATED;
				} else if (tagName == TAG_ENTRY) {
					myEntry = new OPDSEntry();
					myEntry->readAttributes(attributeMap);
					mySummaryTagFound = false;
					myState = F_ENTRY;
				} 
			} else if (tagPrefix == myOpenSearchNamespaceId) {
				if (tagName == OPENSEARCH_TAG_TOTALRESULTS) {
					myState = OPENSEARCH_TOTALRESULTS;
				} else if (tagName == OPENSEARCH_TAG_ITEMSPERPAGE) {
					myState = OPENSEARCH_ITEMSPERPAGE;
				} else if (tagName == OPENSEARCH_TAG_STARTINDEX) {
					myState = OPENSEARCH_STARTINDEX;
				} 
			} 
			break;
		case F_ENTRY:
			if (tagPrefix == myAtomNamespaceId) {
				if (tagName == TAG_AUTHOR) {
					myAuthor = new ATOMAuthor();
					myAuthor->readAttributes(attributeMap);
					myState = FE_AUTHOR;
				} else if (tagName == TAG_ID) {
					myId = new ATOMId();
					myId->readAttributes(attributeMap);
					myState = FE_ID;
				} else if (tagName == TAG_CATEGORY) {
					myCategory = new ATOMCategory();
					myCategory->readAttributes(attributeMap);
					myState = FE_CATEGORY;
				} else if (tagName == TAG_LINK) {
					myLink = new ATOMLink();
					myLink->readAttributes(attributeMap);
					myState = FE_LINK;
				} else if (tagName == TAG_PUBLISHED) {
					myPublished = new ATOMPublished();
					myPublished->readAttributes(attributeMap);
					myState = FE_PUBLISHED;
				} else if (tagName == TAG_SUMMARY) {
					//mySummary = new ATOMSummary(); // TODO:implement ATOMTextConstruct & ATOMSummary
					//mySummary->readAttributes(attributeMap);
					myState = FE_SUMMARY;
				} else if (tagName == TAG_CONTENT) {
					// ???
					myState = FE_CONTENT;
				} else if (tagName == TAG_SUBTITLE) {
					// ???
					myState = FE_SUBTITLE;
				} else if (tagName == TAG_TITLE) {
					//myTitle = new ATOMTitle(); // TODO:implement ATOMTextConstruct & ATOMTitle
					//myTitle->readAttributes(attributeMap);
					myState = FE_TITLE;
				} else if (tagName == TAG_UPDATED) {
					myUpdated = new ATOMUpdated();
					myUpdated->readAttributes(attributeMap);
					myState = FE_UPDATED;
				}
			} else if (tagPrefix == myDublinCoreNamespaceId) {
				if (tagName == DC_TAG_LANGUAGE) {
					myState = FE_DC_LANGUAGE;
				} else if (tagName == DC_TAG_ISSUED) {
					myState = FE_DC_ISSUED;
				} else if (tagName == DC_TAG_PUBLISHER) {
					myState = FE_DC_PUBLISHER;
				} 
			} else if (tagPrefix == myCalibreNamespaceId) {
				if (tagName == CALIBRE_TAG_SERIES) {
					myState = FE_CALIBRE_SERIES;
				} else if (tagName == CALIBRE_TAG_SERIES_INDEX) {
					myState = FE_CALIBRE_SERIES_INDEX;
				}
			}
			break;
		case F_AUTHOR:
			if (tagPrefix == myAtomNamespaceId) {
				if (tagName == TAG_NAME) {
					myState = FA_NAME;
				} else if (tagName == TAG_URI) {
					myState = FA_URI;
				} else if (tagName == TAG_EMAIL) {
					myState = FA_EMAIL;
				} 
			} 
			break;
		case FE_TITLE:
			// TODO: remove this temporary code
			// DON'T clear myBuffer
			return;
		case FE_LINK:
			if (tagPrefix == myOpdsNamespaceId && tagName == TAG_PRICE) {
				myLink->setUserData(KEY_CURRENCY, attributeMap["currencycode"]);
				myState = FEL_PRICE;
			} if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_FORMAT) {
				myState = FEL_FORMAT;
			}
			break;
		case FE_AUTHOR:
			if (tagPrefix == myAtomNamespaceId) {
				if (tagName == TAG_NAME) {
					myState = FEA_NAME;
				} else if (tagName == TAG_URI) {
					myState = FEA_URI;
				} else if (tagName == TAG_EMAIL) {
					myState = FEA_EMAIL;
				} 
			}
			break;
		case FE_CONTENT:
			if (tagName == TAG_HACK_SPAN || attributeMap["class"] == "price") {
				myState = FEC_HACK_SPAN;
			}
			break;
		default:
			break;
	}

	myBuffer.clear();
}

void OPDSXMLParser::endElementHandler(const char *tag) {
	std::string tagPrefix;
	std::string tagName = tag;
	const int index = tagName.find(':');
	if (index != -1) {
		tagPrefix = tagName.substr(0, index);
		tagName.erase(0, index + 1);
	}
	ZLStringUtil::stripWhiteSpaces(myBuffer);

	switch (myState) {
		case START:
			break;
		case FEED:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_FEED) {
				myFeedReader->processFeedMetadata(myFeed);
				myFeed.reset();
				myFeedReader->processFeedEnd();
				myState = START;
			} 
			break;
		case F_ENTRY:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_ENTRY) {
				myFeedReader->processFeedEntry(myEntry);
				myEntry.reset();
				myState = FEED;
			}
			break;
		case F_ID:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_ID) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				myId->setUri(myBuffer);
				if (!myFeed.isNull()) {
					myFeed->setId(myId);
				}
				myId.reset();
				myState = FEED;
			} 
			break;
		case F_LINK:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_LINK) {
				if (!myFeed.isNull()) {
					myFeed->links().push_back(myLink);
				}
				myLink.reset();
				myState = FEED;
			} 
			break;
		case F_CATEGORY:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_CATEGORY) {
				if (!myFeed.isNull()) {
					myFeed->categories().push_back(myCategory);
				}
				myCategory.reset();
				myState = FEED;
			} 
			break;
		case F_TITLE:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_TITLE) {
				// FIXME:title can be lost:buffer will be truncated, if there are extension tags inside the <title> tag
				// TODO:implement ATOMTextConstruct & ATOMTitle
				if (!myFeed.isNull()) {
					myFeed->setTitle(myBuffer);
				}
				myState = FEED;
			} 
			break;
		case F_UPDATED:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_UPDATED) {
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
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_AUTHOR) {
				if (!myFeed.isNull()) {
					myFeed->authors().push_back(myAuthor);
				}
				myAuthor.reset();
				myState = FEED;
			} 
			break;
		case FA_NAME:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_NAME) {
				myAuthor->setName(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_NAME:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_NAME) {
				myAuthor->setName(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FEL_PRICE:
			if (tagPrefix == myOpdsNamespaceId && tagName == TAG_PRICE) {
				myLink->setUserData(KEY_PRICE, myBuffer);
				myState = FE_LINK;
			}
			break;
		case FEL_FORMAT:
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_FORMAT) {
				myLink->setUserData(KEY_FORMAT, myBuffer);
				myState = FE_LINK;
			}
			break;
		case FA_URI:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_URI) {
				myAuthor->setUri(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_URI:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_URI) {
				myAuthor->setUri(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FA_EMAIL:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_EMAIL) {
				myAuthor->setEmail(myBuffer);
				myState = F_AUTHOR;
			}
			break;
		case FEA_EMAIL:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_EMAIL) {
				myAuthor->setEmail(myBuffer);
				myState = FE_AUTHOR;
			}
			break;
		case FE_AUTHOR:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_AUTHOR) {
				myEntry->authors().push_back(myAuthor);
				myAuthor.reset();
				myState = F_ENTRY;
			} 
			break;
		case FE_ID:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_ID) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				myId->setUri(myBuffer);
				myEntry->setId(myId);
				myId.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_CATEGORY:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_CATEGORY) {
				myEntry->categories().push_back(myCategory);
				myCategory.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_LINK:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_LINK) {
				myEntry->links().push_back(myLink);
				myLink.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_PUBLISHED:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_PUBLISHED) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				ATOMDateConstruct::parse(myBuffer, *myPublished);
				myEntry->setPublished(myPublished);
				myPublished.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_SUMMARY:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_SUMMARY) {
				// FIXME:summary can be lost:buffer will be truncated, if there are extension tags inside the <summary> tag
				// TODO:implement ATOMTextConstruct & ATOMSummary
				myEntry->setSummary(myBuffer);
				mySummaryTagFound = true;
				myState = F_ENTRY;
			}
			break;
		case FE_CONTENT:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_CONTENT) {
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
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_SUBTITLE) {
				// TODO:check this accurately
				if (!mySummaryTagFound) {
					myEntry->setSummary(myBuffer);
				}
				myState = F_ENTRY;
			}
			break;
		case FE_TITLE:
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_TITLE) {
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
			if (tagPrefix == myAtomNamespaceId && tagName == TAG_UPDATED) {
				// FIXME:uri can be lost:buffer will be truncated, if there are extension tags inside the <id> tag
				ATOMDateConstruct::parse(myBuffer, *myUpdated);
				myEntry->setUpdated(myUpdated);
				myUpdated.reset();
				myState = F_ENTRY;
			}
			break;
		case FE_DC_LANGUAGE:
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_LANGUAGE) {
				// FIXME:language can be lost:buffer will be truncated, if there are extension tags inside the <dc:language> tag
				myEntry->setDCLanguage(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_ISSUED:
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_ISSUED) {
				// FIXME:issued can be lost:buffer will be truncated, if there are extension tags inside the <dc:issued> tag
				DCDate *issued = new DCDate();
				ATOMDateConstruct::parse(myBuffer, *issued);
				myEntry->setDCIssued(issued);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_PUBLISHER:
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_PUBLISHER) {
				// FIXME:publisher can be lost:buffer will be truncated, if there are extension tags inside the <dc:publisher> tag
				myEntry->setDCPublisher(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_CALIBRE_SERIES:
			if (tagPrefix == myCalibreNamespaceId && tagName == CALIBRE_TAG_SERIES) {
				myEntry->setSeriesTitle(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_CALIBRE_SERIES_INDEX:
			if (tagPrefix == myCalibreNamespaceId && tagName == CALIBRE_TAG_SERIES_INDEX) {
				myEntry->setSeriesIndex(atoi(myBuffer.c_str()));
				myState = F_ENTRY;
			}
			break;
		case OPENSEARCH_TOTALRESULTS:
			if (tagPrefix == myOpenSearchNamespaceId &&
					tagName == OPENSEARCH_TAG_TOTALRESULTS) {
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchTotalResults(number);
				}
				myState = FEED;
			}
			break;
		case OPENSEARCH_ITEMSPERPAGE:
			if (tagPrefix == myOpenSearchNamespaceId &&
					tagName == OPENSEARCH_TAG_ITEMSPERPAGE) {
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchItemsPerPage(number);
				}
				myState = FEED;
			}
			break;
		case OPENSEARCH_STARTINDEX:
			if (tagPrefix == myOpenSearchNamespaceId &&
					tagName == OPENSEARCH_TAG_STARTINDEX) {
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
