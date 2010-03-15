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

#include "OPDSLink.h"
#include "OPDSLink_Reader.h"
#include "OPDSLink_AdvancedSearch.h"
#include "../authentication/basic/BasicAuthenticationManager.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "URLRewritingRule.h"


OPDSLink::Reader::Reader() : myState(READ_NOTHING) {
}

shared_ptr<NetworkLink> OPDSLink::Reader::link() {
	if (mySiteName.empty() || myTitle.empty() || myLinks[NetworkLink::URL_MAIN].empty()) {
		return 0;
	}
	OPDSLink *opdsLink = new OPDSLink(
		mySiteName,
		myTitle,
		mySummary,
		myIcon,
		myLinks
	);
	if (!mySearchType.empty()) {
		opdsLink->myAdvancedSearch = new AdvancedSearch(
			mySearchType,
			mySearchFields["titleOrSeries"],
			mySearchFields["author"],
			mySearchFields["tag"],
			mySearchFields["annotation"]
		);
	}
	myRelationAliases.swap(opdsLink->myRelationAliases);
	myFeedConditions.swap(opdsLink->myFeedConditions);
	myUrlRewritingRules.swap(opdsLink->myUrlRewritingRules);

	if (myAuthenticationType == "basic") {
		opdsLink->myAuthenticationManager =
			new BasicAuthenticationManager(*opdsLink);
	} else if (myAuthenticationType == "litres") {
		opdsLink->myAuthenticationManager =
			new LitResAuthenticationManager(*opdsLink);
	}

	return opdsLink;
}

static const std::string TAG_SITE = "site";
static const std::string TAG_LINK = "link";
static const std::string TAG_TITLE = "title";
static const std::string TAG_SUMMARY = "summary";
static const std::string TAG_ICON = "icon";
static const std::string TAG_RELATION_ALIASES = "relationAliases";
static const std::string TAG_ALIAS = "alias";
static const std::string TAG_SEARCH_DESCRIPTION = "advancedSearch";
static const std::string TAG_FEEDS = "feeds";
static const std::string TAG_AUTHENTICATION = "authentication";
static const std::string TAG_URL_REWRITING_RULES = "urlRewritingRules";
static const std::string TAG_FIELD = "field";
static const std::string TAG_CONDITION = "condition";
static const std::string TAG_RULE = "rule";

void OPDSLink::Reader::startElementHandler(const char *tag, const char **attributes) {
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
				myUrlRewritingRules.push_back(new URLRewritingRule(URLRewritingRule::ADD_URL_PARAMETER, ruleApply, name, value));
			}
		}
	} else if (TAG_RELATION_ALIASES == tag) {
		myState = READ_RELATION_ALIASES;
	} else if (myState == READ_RELATION_ALIASES && TAG_ALIAS == tag) {
		const char *alias = attributeValue(attributes, "alias");
		const char *name  = attributeValue(attributes, "name");
		const char *type  = attributeValue(attributes, "type");
		if (alias != 0 && name != 0) {
			myRelationAliases[
				RelationAlias(alias, (type != 0) ? type : std::string())
			] = name;
		}
	}
}

void OPDSLink::Reader::endElementHandler(const char *tag) {
	if (myState == READ_SEARCH_FIELD) {
		myState = READ_SEARCH_DESCRIPTION;
	} else if (myState == READ_FEEDS_CONDITION) {
		myFeedConditions[myBuffer] = (myAttrBuffer == "signedIn") ?
			OPDSLink::CONDITION_SIGNED_IN : OPDSLink::CONDITION_NEVER;
		myState = READ_FEEDS;
	} else if (myState == READ_URL_REWRITING_RULES && TAG_RULE == tag) {
		// myState = myState
	} else if (myState == READ_RELATION_ALIASES && TAG_ALIAS == tag) {
		// myState = myState
	} else {
		myState = READ_NOTHING;
	}
}

void OPDSLink::Reader::characterDataHandler(const char *text, size_t len) {
	switch (myState) {
		case READ_NOTHING:
		case READ_SEARCH_DESCRIPTION:
		case READ_FEEDS:
		case READ_URL_REWRITING_RULES:
		case READ_RELATION_ALIASES:
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
			myIcon.append(text, len);
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
