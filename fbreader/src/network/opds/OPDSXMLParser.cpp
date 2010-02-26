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

static const std::string DC_TAG_LANGUAGE = "language";
static const std::string DC_TAG_ISSUED = "issued";
static const std::string DC_TAG_PUBLISHER = "publisher";

static const std::string OPENSEARCH_TAG_TOTALRESULTS = "totalResults";
static const std::string OPENSEARCH_TAG_ITEMSPERPAGE = "itemsPerPage";
static const std::string OPENSEARCH_TAG_STARTINDEX = "startIndex";



OPDSXMLParser::OPDSXMLParser(shared_ptr<OPDSFeedReader> feedReader) : myFeedReader(feedReader) {
	myState = START;
}

bool OPDSXMLParser::processNamespaces() const {
	return true;
}

void OPDSXMLParser::namespaceListChangedHandler() {
	myDublinCoreNamespaceId.erase();
	myAtomNamespaceId.erase();
	myAtomPrefix.erase();
	myOpenSearchNamespaceId.erase();
	myCalibrePrefix.erase();

	const std::map<std::string,std::string> &nsMap = namespaces();
	for (std::map<std::string,std::string>::const_iterator it = nsMap.begin(); it != nsMap.end(); ++it) {
		if (it->first.empty()) {
			continue;
		}
		if (ZLStringUtil::stringStartsWith(it->second, XMLNamespace::DublinCoreTermsPrefix)) {
			myDublinCoreNamespaceId = it->first;
		} else if (it->second == XMLNamespace::Atom) {
			myAtomNamespaceId = it->first + ':';
			myAtomPrefix = it->first + ':';
		} else if (it->second == XMLNamespace::OpenSearch) {
			myOpenSearchNamespaceId = it->first;
		} else if (it->second == XMLNamespace::CalibreMetadata) {
			myCalibrePrefix = it->first + ':';
		}
	}
}

bool OPDSXMLParser::checkAtomTag(const std::string &tag, const std::string &pattern) const {
	return checkNSTag(tag, myAtomPrefix, pattern);
}

bool OPDSXMLParser::checkNSTag(const std::string &tag, const std::string &nsprefix, const std::string &pattern) {
	return
		tag.length() == nsprefix.length() + pattern.length() &&
		tag == nsprefix + pattern;
}


void OPDSXMLParser::startElementHandler(const char *tag, const char **attributes) {
	myAttributes.clear();
	while (*attributes != 0) {
		std::string name(*attributes++);
		if (*attributes == 0) {
			break;
		}
		std::string value(*attributes++);
		myAttributes.insert(std::make_pair(name, value));
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
				myFeed->readAttributes(myAttributes);
				myState = FEED;
			}
			break;
		case FEED:
			if (tagPrefix == myAtomNamespaceId) {
				if (tagName == TAG_AUTHOR) {
					myAuthor = new ATOMAuthor();
					myAuthor->readAttributes(myAttributes);
					myState = F_AUTHOR;
				} else if (tagName == TAG_ID) {
					myId = new ATOMId();
					myId->readAttributes(myAttributes);
					myState = F_ID;
				} else if (tagName == TAG_LINK) {
					myLink = new ATOMLink();
					myLink->readAttributes(myAttributes);
					myState = F_LINK;
				} else if (tagName == TAG_CATEGORY) {
					myCategory = new ATOMCategory();
					myCategory->readAttributes(myAttributes);
					myState = F_CATEGORY;
				} else if (tagName == TAG_TITLE) {
					//myTitle = new ATOMTitle(); // TODO: implement ATOMTextConstruct & ATOMTitle
					//myTitle->readAttributes(myAttributes);
					myState = F_TITLE;
				} else if (tagName == TAG_UPDATED) {
					myUpdated = new ATOMUpdated();
					myUpdated->readAttributes(myAttributes);
					myState = F_UPDATED;
				} else if (tagName == TAG_ENTRY) {
					myEntry = new OPDSEntry();
					myEntry->readAttributes(myAttributes);
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
					myAuthor->readAttributes(myAttributes);
					myState = FE_AUTHOR;
				} else if (tagName == TAG_ID) {
					myId = new ATOMId();
					myId->readAttributes(myAttributes);
					myState = FE_ID;
				} else if (tagName == TAG_CATEGORY) {
					myCategory = new ATOMCategory();
					myCategory->readAttributes(myAttributes);
					myState = FE_CATEGORY;
				} else if (tagName == TAG_LINK) {
					myLink = new ATOMLink();
					myLink->readAttributes(myAttributes);
					myState = FE_LINK;
				} else if (tagName == TAG_PUBLISHED) {
					myPublished = new ATOMPublished();
					myPublished->readAttributes(myAttributes);
					myState = FE_PUBLISHED;
				} else if (tagName == TAG_SUMMARY) {
					//mySummary = new ATOMSummary(); // TODO: implement ATOMTextConstruct & ATOMSummary
					//mySummary->readAttributes(myAttributes);
					myState = FE_SUMMARY;
				} else if (tagName == TAG_CONTENT) {
					// ???
					myState = FE_CONTENT;
				} else if (tagName == TAG_SUBTITLE) {
					// ???
					myState = FE_SUBTITLE;
				} else if (tagName == TAG_TITLE) {
					//myTitle = new ATOMTitle(); // TODO: implement ATOMTextConstruct & ATOMTitle
					//myTitle->readAttributes(myAttributes);
					myState = FE_TITLE;
				} else if (tagName == TAG_UPDATED) {
					myUpdated = new ATOMUpdated();
					myUpdated->readAttributes(myAttributes);
					myState = FE_UPDATED;
				}
			} else if (tagPrefix == myDublinCoreNamespaceId) {
				if (tagName == DC_TAG_LANGUAGE) {
					// nothing to do
					myState = FE_DC_LANGUAGE;
				} else if (tagName == DC_TAG_ISSUED) {
					// nothing to do
					myState = FE_DC_ISSUED;
				} else if (tagName == DC_TAG_PUBLISHER) {
					// nothing to do
					myState = FE_DC_PUBLISHER;
				} 
			}
			break;
		default:
			processState(tag, false);
			myState = getNextState(tag, false);
			break;
	}

	myBuffer.clear();
}

void OPDSXMLParser::endElementHandler(const char *tag) {
	myAttributes.clear();

	std::string tagPrefix;
	std::string tagName = tag;
	const int index = tagName.find(':');
	if (index != -1) {
		tagPrefix = tagName.substr(0, index);
		tagName.erase(0, index + 1);
	}

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
		case FE_DC_LANGUAGE: 
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_LANGUAGE) {
				// FIXME: language can be lost: buffer will be truncated, if there are extension tags inside the <dc:language> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myEntry->setDCLanguage(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_ISSUED: 
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_ISSUED) {
				// FIXME: issued can be lost: buffer will be truncated, if there are extension tags inside the <dc:issued> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				DCDate *issued = new DCDate();
				ATOMDateConstruct::parse(myBuffer, *issued);
				myEntry->setDCIssued(issued);
				myState = F_ENTRY;
			}
			break;
		case FE_DC_PUBLISHER:
			if (tagPrefix == myDublinCoreNamespaceId && tagName == DC_TAG_PUBLISHER) {
				// FIXME: publisher can be lost: buffer will be truncated, if there are extension tags inside the <dc:publisher> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myEntry->setDCPublisher(myBuffer);
				myState = F_ENTRY;
			}
			break;
		case OPENSEARCH_TOTALRESULTS:
			if (tagPrefix == myOpenSearchNamespaceId &&
					tagName == OPENSEARCH_TAG_TOTALRESULTS) {
				ZLStringUtil::stripWhiteSpaces(myBuffer);
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
				ZLStringUtil::stripWhiteSpaces(myBuffer);
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
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				int number = atoi(myBuffer.c_str());
				if (!myFeed.isNull()) {
					myFeed->setOpensearchStartIndex(number);
				}
				myState = FEED;
			}
			break;
		default:
			processState(tag, true);
			myState = getNextState(tag, true);
			break;
	}

	myBuffer.clear();
}

void OPDSXMLParser::characterDataHandler(const char *data, size_t len) {
	myBuffer.append(data, len);
}

void OPDSXMLParser::processState(const std::string &tag, bool closed) {
	switch(myState) {
		case F_ID: 
			if (closed && checkAtomTag(tag, TAG_ID)) {
				// FIXME: uri can be lost: buffer will be truncated, if there are extension tags inside the <id> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myId->setUri(myBuffer);
				if (!myFeed.isNull()) {
					myFeed->setId(myId);
				}
				myId.reset();
			} 
			break;
		case F_LINK: 
			if (closed && checkAtomTag(tag, TAG_LINK)) {
				if (!myFeed.isNull()) {
					myFeed->links().push_back(myLink);
				}
				myLink.reset();
			} 
			break;
		case F_CATEGORY: 
			if (closed && checkAtomTag(tag, TAG_CATEGORY)) {
				if (!myFeed.isNull()) {
					myFeed->categories().push_back(myCategory);
				}
				myCategory.reset();
			} 
			break;
		case F_TITLE: 
			if (closed && checkAtomTag(tag, TAG_TITLE)) {
				// FIXME: title can be lost: buffer will be truncated, if there are extension tags inside the <title> tag
				// TODO: implement ATOMTextConstruct & ATOMTitle
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				if (!myFeed.isNull()) {
					myFeed->setTitle(myBuffer);
				}
			} 
			break;
		case F_UPDATED: 
			if (closed && checkAtomTag(tag, TAG_UPDATED)) {
				// FIXME: uri can be lost: buffer will be truncated, if there are extension tags inside the <id> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				ATOMDateConstruct::parse(myBuffer, *myUpdated);
				if (!myFeed.isNull()) {
					myFeed->setUpdated(myUpdated);
				}
				myUpdated.reset();
			} 
			break;
		case F_AUTHOR: 
			if (!closed) {
				if (checkAtomTag(tag, TAG_NAME)) {
					// nothing to do
				} else if (checkAtomTag(tag, TAG_URI)) {
					// nothing to do
				} else if (checkAtomTag(tag, TAG_EMAIL)) {
					// nothing to do
				} 
			} else {
				if (checkAtomTag(tag, TAG_AUTHOR)) {
					if (!myFeed.isNull()) {
						myFeed->authors().push_back(myAuthor);
					}
					myAuthor.reset();
				} 
			}
			break;
		case FA_NAME: 
		case FEA_NAME: 
			if (closed && checkAtomTag(tag, TAG_NAME)) {
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myAuthor->setName(myBuffer);
			}
			break;
		case FA_URI: 
		case FEA_URI: 
			if (closed && checkAtomTag(tag, TAG_URI)) {
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myAuthor->setUri(myBuffer);
			}
			break;
		case FA_EMAIL: 
		case FEA_EMAIL:
			if (closed && checkAtomTag(tag, TAG_EMAIL)) {
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myAuthor->setEmail(myBuffer);
			}
			break;
		case FE_AUTHOR: 
			if (!closed) {
				if (checkAtomTag(tag, TAG_NAME)) {
					// nothing to do
				} else if (checkAtomTag(tag, TAG_URI)) {
					// nothing to do
				} else if (checkAtomTag(tag, TAG_EMAIL)) {
					// nothing to do
				} 
			} else {
				if (checkAtomTag(tag, TAG_AUTHOR)) {
					myEntry->authors().push_back(myAuthor);
					myAuthor.reset();
				} 
			}
			break;
		case FE_ID: 
			if (closed && checkAtomTag(tag, TAG_ID)) {
				// FIXME: uri can be lost: buffer will be truncated, if there are extension tags inside the <id> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myId->setUri(myBuffer);
				myEntry->setId(myId);
				myId.reset();
			}
			break;
		case FE_CATEGORY: 
			if (closed && checkAtomTag(tag, TAG_CATEGORY)) {
				myEntry->categories().push_back(myCategory);
				myCategory.reset();
			}
			break;
		case FE_LINK: 
			if (closed && checkAtomTag(tag, TAG_LINK)) {
				myEntry->links().push_back(myLink);
				myLink.reset();
			}
			break;
		case FE_PUBLISHED: 
			if (closed && checkAtomTag(tag, TAG_PUBLISHED)) {
				// FIXME: uri can be lost: buffer will be truncated, if there are extension tags inside the <id> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				ATOMDateConstruct::parse(myBuffer, *myPublished);
				myEntry->setPublished(myPublished);
				myPublished.reset();
			}
			break;
		case FE_SUMMARY: 
			if (closed && checkAtomTag(tag, TAG_SUMMARY)) {
				// FIXME: summary can be lost: buffer will be truncated, if there are extension tags inside the <summary> tag
				// TODO: implement ATOMTextConstruct & ATOMSummary
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myEntry->setSummary(myBuffer);
				mySummaryTagFound = true;
			}
			break;
		case FE_CONTENT: 
			if (closed && checkAtomTag(tag, TAG_CONTENT)) {
				// TODO: check this accurately
				if (!mySummaryTagFound) {
					ZLStringUtil::stripWhiteSpaces(myBuffer);
					myEntry->setSummary(myBuffer);
				}
			}
			break;
		case FE_SUBTITLE: 
			if (closed && checkAtomTag(tag, TAG_SUBTITLE)) {
				// TODO: check this accurately
				if (!mySummaryTagFound) {
					ZLStringUtil::stripWhiteSpaces(myBuffer);
					myEntry->setSummary(myBuffer);
				}
			}
			break;
		case FE_TITLE: 
			if (closed && checkAtomTag(tag, TAG_TITLE)) {
				// FIXME: title can be lost: buffer will be truncated, if there are extension tags inside the <title> tag
				// TODO: implement ATOMTextConstruct & ATOMTitle
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				myEntry->setTitle(myBuffer);
			}
			break;
		case FE_UPDATED: 
			if (closed && checkAtomTag(tag, TAG_UPDATED)) {
				// FIXME: uri can be lost: buffer will be truncated, if there are extension tags inside the <id> tag
				ZLStringUtil::stripWhiteSpaces(myBuffer);
				ATOMDateConstruct::parse(myBuffer, *myUpdated);
				myEntry->setUpdated(myUpdated);
				myUpdated.reset();
			}
			break;
	}
}


OPDSXMLParser::State OPDSXMLParser::getNextState(const std::string &tag, bool closed) {
	switch(myState) {
		case F_ID: 
			if (closed && checkAtomTag(tag, TAG_ID)) {
				return FEED;
			} 
			break;
		case F_LINK: 
			if (closed && checkAtomTag(tag, TAG_LINK)) {
				return FEED;
			} 
			break;
		case F_CATEGORY: 
			if (closed && checkAtomTag(tag, TAG_CATEGORY)) {
				return FEED;
			} 
			break;
		case F_TITLE: 
			if (closed && checkAtomTag(tag, TAG_TITLE)) {
				return FEED;
			} 
			break;
		case F_UPDATED: 
			if (closed && checkAtomTag(tag, TAG_UPDATED)) {
				return FEED;
			} 
			break;
		case F_AUTHOR: 
			if (!closed) {
				if (checkAtomTag(tag, TAG_NAME)) {
					return FA_NAME;
				} else if (checkAtomTag(tag, TAG_URI)) {
					return FA_URI;
				} else if (checkAtomTag(tag, TAG_EMAIL)) {
					return FA_EMAIL;
				} 
			} else {
				if (checkAtomTag(tag, TAG_AUTHOR)) {
					return FEED;
				} 
			}
			break;
		case FA_NAME: 
			if (closed && checkAtomTag(tag, TAG_NAME)) {
				return F_AUTHOR;
			}
			break;
		case FA_URI: 
			if (closed && checkAtomTag(tag, TAG_URI)) {
				return F_AUTHOR;
			}
			break;
		case FA_EMAIL: 
			if (closed && checkAtomTag(tag, TAG_EMAIL)) {
				return F_AUTHOR;
			}
			break;
		case FE_AUTHOR: 
			if (!closed) {
				if (checkAtomTag(tag, TAG_NAME)) {
					return FEA_NAME;
				} else if (checkAtomTag(tag, TAG_URI)) {
					return FEA_URI;
				} else if (checkAtomTag(tag, TAG_EMAIL)) {
					return FEA_EMAIL;
				} 
			} else {
				if (checkAtomTag(tag, TAG_AUTHOR)) {
					return F_ENTRY;
				} 
			}
			break;
		case FE_ID: 
			if (closed && checkAtomTag(tag, TAG_ID)) {
				return F_ENTRY;
			}
			break;
		case FE_CATEGORY: 
			if (closed && checkAtomTag(tag, TAG_CATEGORY)) {
				return F_ENTRY;
			}
			break;
		case FE_LINK: 
			if (closed && checkAtomTag(tag, TAG_LINK)) {
				return F_ENTRY;
			}
			break;
		case FE_PUBLISHED: 
			if (closed && checkAtomTag(tag, TAG_PUBLISHED)) {
				return F_ENTRY;
			}
			break;
		case FE_SUMMARY: 
			if (closed && checkAtomTag(tag, TAG_SUMMARY)) {
				return F_ENTRY;
			}
			break;
		case FE_CONTENT: 
			if (closed && checkAtomTag(tag, TAG_CONTENT)) {
				return F_ENTRY;
			}
			break;
		case FE_SUBTITLE: 
			if (closed && checkAtomTag(tag, TAG_SUBTITLE)) {
				return F_ENTRY;
			}
			break;
		case FE_TITLE: 
			if (closed && checkAtomTag(tag, TAG_TITLE)) {
				return F_ENTRY;
			}
			break;
		case FE_UPDATED: 
			if (closed && checkAtomTag(tag, TAG_UPDATED)) {
				return F_ENTRY;
			}
			break;
		case FEA_NAME: 
			if (closed && checkAtomTag(tag, TAG_NAME)) {
				return FE_AUTHOR;
			}
			break;
		case FEA_URI: 
			if (closed && checkAtomTag(tag, TAG_URI)) {
				return FE_AUTHOR;
			}
			break;
		case FEA_EMAIL:
			if (closed && checkAtomTag(tag, TAG_EMAIL)) {
				return FE_AUTHOR;
			}
			break;
	}
	return myState;
}

