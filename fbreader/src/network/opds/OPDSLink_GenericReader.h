/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
 */


#ifndef __OPDSLINK_GENERICREADER_H__
#define __OPDSLINK_GENERICREADER_H__

#include <vector>
#include <map>

#include <ZLXMLReader.h>

#include "OPDSLink.h"

struct URLRewritingRule;

class OPDSLink::GenericReader : public ZLXMLReader {

public:
	GenericReader();
	std::vector<shared_ptr<NetworkLink> > myNetworkLinks;


private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);
	shared_ptr<NetworkLink> link();
	void reset();

private:
	enum {
		READ_NOTHING,
		READ_SITENAME,
		READ_TITLE,
		READ_SUMMARY,
		READ_ENTRY,
	} myState;

	std::string mySiteName;
	std::string myTitle;
	std::string mySummary;
	std::string myIcon;

	std::map<std::string,std::string> myLinks;

	std::map<RelationAlias,std::string> myRelationAliases;

	std::string mySearchType;
	std::map<std::string,std::string> mySearchFields;

	std::map<std::string,OPDSLink::FeedCondition> myFeedConditions;
	std::string myAuthenticationType;
	std::vector<shared_ptr<URLRewritingRule> > myUrlRewritingRules;
};

#endif /* __OPDSLINK_GENERICREADER_H__ */
