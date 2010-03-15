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

#ifndef __OPDSLINK_READER_H__
#define __OPDSLINK_READER_H__

#include <vector>
#include <map>

#include <ZLXMLReader.h>

#include "OPDSLink.h"

class URLRewritingRule;

class OPDSLink::Reader : public ZLXMLReader {

public:
	Reader();

	shared_ptr<NetworkLink> link();

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

private:
	enum {
		READ_NOTHING,
		READ_SITENAME,
		READ_TITLE,
		READ_SUMMARY,
		READ_ICON_NAME,
		READ_LINK,
		READ_SEARCH_DESCRIPTION,
		READ_SEARCH_FIELD,
		READ_FEEDS,
		READ_FEEDS_CONDITION,
		READ_URL_REWRITING_RULES,
		READ_RELATION_ALIASES,
	} myState;

	std::string mySiteName;
	std::string myTitle;
	std::string mySummary;
	std::string myIcon;

	std::string myAttrBuffer;
	std::string myBuffer;

	std::map<std::string,std::string> myLinks;

	std::map<RelationAlias,std::string> myRelationAliases;

	std::string mySearchType;
	std::map<std::string,std::string> mySearchFields;

	std::map<std::string,OPDSLink::FeedCondition> myFeedConditions;
	std::string myAuthenticationType;
	std::vector<shared_ptr<URLRewritingRule> > myUrlRewritingRules;
};

#endif /* __OPDSLINK_READER_H__ */
