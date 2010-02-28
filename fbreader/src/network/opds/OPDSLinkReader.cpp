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

#include "OPDSLinkReader.h"
#include "OPDSLink.h"
#include "../authentication/basic/BasicAuthenticationManager.h"
#include "../litres/LitResAuthenticationManager.h"

#include "URLRewritingRule.h"


OPDSLinkReader::OPDSLinkReader() : myState(READ_NOTHING) {
}

shared_ptr<NetworkLink> OPDSLinkReader::link() {
	if (mySiteName.empty() || myTitle.empty() || myLinks[OPDSLink::URL_MAIN].empty()) {
		return 0;
	}
	OPDSLink *opdsLink = new OPDSLink(
		mySiteName,
		myTitle,
		myLinks,
		mySummary,
		myIconName
	);
	if (!mySearchType.empty()) {
		opdsLink->setupAdvancedSearch(
			mySearchType,
			mySearchFields["titleOrSeries"],
			mySearchFields["author"],
			mySearchFields["tag"],
			mySearchFields["annotation"]
		);
	}
	opdsLink->setIgnoredFeeds(myIgnoredFeeds);
	opdsLink->setAccountDependentFeeds(myAccountDependentFeeds);

	shared_ptr<NetworkAuthenticationManager> authManager;
	if (myAuthenticationType == "basic") {
		authManager = new BasicAuthenticationManager(
			mySiteName, myLinks["signIn"], myLinks["signOut"]
		);
	} else if (myAuthenticationType == "litres") {
		authManager = new LitResAuthenticationManager(*opdsLink);
	}
	opdsLink->setAuthenticationManager(authManager);

	for (std::set<shared_ptr<URLRewritingRule> >::const_iterator it = myUrlRewritingRules.begin(); it != myUrlRewritingRules.end(); ++it) {
		opdsLink->addUrlRewritingRule(*it);
	}
	return opdsLink;
}

static const std::string TAG_SITE = "site";
static const std::string TAG_LINK = "link";
static const std::string TAG_TITLE = "title";
static const std::string TAG_SUMMARY = "summary";
static const std::string TAG_ICON = "icon";
static const std::string TAG_SEARCH_DESCRIPTION = "advancedSearch";
static const std::string TAG_FEEDS = "feeds";
static const std::string TAG_AUTHENTICATION = "authentication";
static const std::string TAG_URL_REWRITING_RULES = "urlRewritingRules";
static const std::string TAG_FIELD = "field";
static const std::string TAG_CONDITION = "condition";
static const std::string TAG_RULE = "rule";

void OPDSLinkReader::startElementHandler(const char *tag, const char **attributes) {
	if (TAG_SITE == tag) {
		myState = READ_SITENAME;
	} else if (TAG_TITLE == tag) {
		myState = READ_TITLE;
	} else if (TAG_SUMMARY == tag) {
		myState = READ_SUMMARY;
	} else if (TAG_ICON == tag) {
		myState = READ_ICON_NAME;
	} else if (TAG_LINK == tag) {
		const char *linkType = attributeValue(attributes, "rel");
		if (linkType != 0) {
			myAttrBuffer = linkType;
			myLinks[myAttrBuffer].clear();
			myState = READ_LINK;
		}
	} else if (TAG_SEARCH_DESCRIPTION == tag) {
		const char *searchType = attributeValue(attributes, "style");
		if (searchType != 0) {
			mySearchType = searchType;
			myState = READ_SEARCH_DESCRIPTION;
		}
	} else if (myState == READ_SEARCH_DESCRIPTION && TAG_FIELD == tag) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			myAttrBuffer = name;
			mySearchFields[myAttrBuffer].clear();
			myState = READ_SEARCH_FIELD;
		}
	} else if (TAG_FEEDS == tag) {
		myState = READ_FEEDS;
	} else if (myState == READ_FEEDS && TAG_CONDITION == tag) {
		const char *show = attributeValue(attributes, "show");
		if (show != 0) {
			myAttrBuffer = show;
			myBuffer.clear();
			myState = READ_FEEDS_CONDITION;
		}
	} else if (TAG_AUTHENTICATION == tag) {
		const char *authenticationType = attributeValue(attributes, "type");
		if (authenticationType != 0) {
			myAuthenticationType = authenticationType;
		}
	} else if (TAG_URL_REWRITING_RULES == tag) {
		myState = READ_URL_REWRITING_RULES;
	} else if (myState == READ_URL_REWRITING_RULES && TAG_RULE == tag) {
		const char *type  = attributeValue(attributes, "type");
		const char *apply = attributeValue(attributes, "apply");
		const char *name  = attributeValue(attributes, "name");
		const char *value = attributeValue(attributes, "value");

		URLRewritingRule::RuleApply ruleApply = URLRewritingRule::ALWAYS;
		if (apply != 0) {
			const std::string applyStr = apply;
			if (applyStr == "external") {
				ruleApply = URLRewritingRule::EXTERNAL;
			} else if (applyStr == "internal") {
				ruleApply = URLRewritingRule::INTERNAL;
			} else if (applyStr != "always") {
				type = 0;
			}
		}

		if (type != 0 && name != 0 && value != 0) {
			std::string typeStr = type;
			if (typeStr == "addUrlParameter") {
				myUrlRewritingRules.insert(new URLRewritingRule(URLRewritingRule::ADD_URL_PARAMETER, ruleApply, name, value));
			}
		}
	}
}

void OPDSLinkReader::endElementHandler(const char *tag) {
	if (myState == READ_SEARCH_FIELD) {
		myState = READ_SEARCH_DESCRIPTION;
	} else if (myState == READ_FEEDS_CONDITION) {
		if (myAttrBuffer == "signedIn") {
			myAccountDependentFeeds.insert(myBuffer);
		} else if (myAttrBuffer == "never") {
			myIgnoredFeeds.insert(myBuffer);
		}
		myState = READ_FEEDS;
	} else if (myState == READ_URL_REWRITING_RULES && TAG_RULE == tag) {
		//myState = myState;
	} else {
		myState = READ_NOTHING;
	}
}

void OPDSLinkReader::characterDataHandler(const char *text, size_t len) {
	switch (myState) {
		case READ_NOTHING:
		case READ_SEARCH_DESCRIPTION:
		case READ_FEEDS:
		case READ_URL_REWRITING_RULES:
			break;
		case READ_SITENAME:
			mySiteName.append(text, len);
			break;
		case READ_TITLE:
			myTitle.append(text, len);
			break;
		case READ_SUMMARY:
			mySummary.append(text, len);
			break;
		case READ_ICON_NAME:
			myIconName.append(text, len);
			break;
		case READ_LINK:
			myLinks[myAttrBuffer].append(text, len);
			break;
		case READ_SEARCH_FIELD:
			mySearchFields[myAttrBuffer].append(text, len);
			break;
		case READ_FEEDS_CONDITION:
			myBuffer.append(text, len);
			break;
	}
}
