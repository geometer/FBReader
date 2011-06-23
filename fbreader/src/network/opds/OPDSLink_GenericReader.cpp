/*
 * aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 */

#include "OPDSLink.h"
#include "OPDSLink_GenericReader.h"
#include "OPDSLink_AdvancedSearch.h"
#include "../authentication/basic/BasicAuthenticationManager.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "URLRewritingRule.h"
#include <stdio.h>

OPDSLink::GenericReader::GenericReader() : myState(READ_NOTHING) {
}

static const std::string TAG_SITE = "id";
static const std::string TAG_LINK = "link";
static const std::string TAG_TITLE = "title";
static const std::string TAG_SUMMARY = "content";
static const std::string TAG_ENTRY = "entry";
static const std::string TAG_SEARCH_DESCRIPTION = "fbreader:advancedSearch";
static const std::string TAG_AUTHENTICATION = "fbreader:authentication";
static const std::string TAG_URL_REWRITING_RULES = "fbreader:urlRewritingRule";
static const std::string TAG_RELATION_ALIASES = "fbreader:relationAlias";

shared_ptr<NetworkLink> OPDSLink::GenericReader::link() {
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

	if (myAuthenticationType == "litres") {
		opdsLink->myAuthenticationManager =
			new LitResAuthenticationManager(*opdsLink);
	} else if (myLinks.count("signIn")) {
		opdsLink->myAuthenticationManager =
			new BasicAuthenticationManager(*opdsLink);
	}

	return opdsLink;
}

void OPDSLink::GenericReader::startElementHandler(const char *tag, const char **attributes) {
	if (myState == READ_ENTRY) {
		if (TAG_SITE == tag) {
			myState = READ_SITENAME;
		} else if (TAG_TITLE == tag) {
			myState = READ_TITLE;
		} else if (TAG_SUMMARY == tag) {
			myState = READ_SUMMARY;
		} else if (TAG_LINK == tag) {
			const char *linkType = attributeValue(attributes, "type");
			const char *linkRel = attributeValue(attributes, "rel");
			const char *linkHref = attributeValue(attributes, "href");
			if (std::string(linkType) == "image/png") {
				myIcon = linkHref;
			} else  {
				if (linkRel == 0) {
					myLinks["main"] = linkHref;
				} else if (std::string(linkRel) == "search") {
					std::string s = linkHref;
					s = s.substr(0, s.length() - 13) + "%s";
					myLinks["search"] = s;
				} else if (std::string(linkRel) == "http://data.fbreader.org/catalog/sign-in") {
					myLinks["signIn"] = linkHref;
				} else if (std::string(linkRel) == "http://data.fbreader.org/catalog/sign-out") {
					myLinks["signOut"] = linkHref;
				} else if (std::string(linkRel) == "http://data.fbreader.org/catalog/refill-account") {
					myLinks["refillAccount"] = linkHref;
				} else if (std::string(linkRel) == "http://data.fbreader.org/catalog/recover-password") {
					myLinks["recoverPassword"] = linkHref;
				}
			}
		} else if (TAG_SEARCH_DESCRIPTION == tag) {
			const char *searchType = attributeValue(attributes, "style");
			if (searchType != 0) {
				mySearchType = searchType;
			}
			const char **attrsTmp = attributes;
			while (*attrsTmp != 0) {
				const char* temp = *attrsTmp;
				++attrsTmp;
				mySearchFields[temp] = *attrsTmp;
				++attrsTmp;
			}
		} else if (TAG_AUTHENTICATION == tag) {
			const char *authenticationType = attributeValue(attributes, "type");
			if (authenticationType != 0) {
				myAuthenticationType = authenticationType;
			}
		} else if (TAG_URL_REWRITING_RULES == tag) {
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
			} else if (TAG_RELATION_ALIASES == tag) {
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
	} else if (TAG_ENTRY == tag) {
		myState = READ_ENTRY;
	}
}

void OPDSLink::GenericReader::reset() {
	myState = READ_NOTHING;
	mySiteName = "";
	myTitle = "";
	mySummary = "";
	myIcon = "";

	myAttrBuffer = "";
	myBuffer = "";

	myLinks.clear();

	myRelationAliases.clear();

	mySearchType = "";
	mySearchFields.clear();

	myFeedConditions.clear();
	myAuthenticationType = "";
	myUrlRewritingRules.clear();
}

void OPDSLink::GenericReader::endElementHandler(const char *tag) {
	if (myState == READ_SITENAME) {
		mySiteName = mySiteName.substr(25, -1);
	}
	if (TAG_ENTRY == tag) {
		myNetworkLinks.push_back(link());
		reset();
		myState = READ_NOTHING;
	} else if (myState != READ_NOTHING) {
		myState = READ_ENTRY;
	}
}

void OPDSLink::GenericReader::characterDataHandler(const char *text, size_t len) {
	switch (myState) {
		case READ_NOTHING:
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
	}
}
