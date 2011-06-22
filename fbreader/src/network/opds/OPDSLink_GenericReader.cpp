/*
 * aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 */

#include "OPDSLink.h"
#include "OPDSLink_GenericReader.h"
#include "OPDSLink_AdvancedSearch.h"
#include "../authentication/basic/BasicAuthenticationManager.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "URLRewritingRule.h"


OPDSLink::GenericReader::GenericReader() : myState(READ_NOTHING) {
}

static const std::string TAG_SITE = "id";
static const std::string TAG_LINK = "link";
static const std::string TAG_TITLE = "title";
static const std::string TAG_SUMMARY = "content";
static const std::string TAG_ENTRY "entry";

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

	if (myAuthenticationType == "basic") {
		opdsLink->myAuthenticationManager =
			new BasicAuthenticationManager(*opdsLink);
	} else if (myAuthenticationType == "litres") {
		opdsLink->myAuthenticationManager =
			new LitResAuthenticationManager(*opdsLink);
	}

	return opdsLink;
}

void OPDSLink::GenericReader::startElementHandler(const char *tag, const char **attributes) {
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
		if (linkType == "application/atom+xml") {
			if (linkRel == 0) {
				myLinks["main"] = linkHref;
			}
			if (linkRel == "search") {
				std::string s = linkHref;
				s = s.substr(0, s.length() - 13) + "%s";
				myLinks["search"] = s;
			}
		}
		if (linkType == "image/png") {
			//TODO
		}
	} else if (TAG_ENTRY == tag) {
		//
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
	} if (TAG_ENTRY == tag) {
		myNetworkLinks.append(link());
		reset();
	}
}

void OPDSLink::GenericReader::characterDataHandler(const char *text, size_t len) {
	switch (myState) {
		case READ_NOTHING:
		case READ_LINK:
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
