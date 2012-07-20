/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSLINK_GENERICFEEDREADER_H__
#define __OPDSLINK_GENERICFEEDREADER_H__

#include <map>
#include <string>

#include "URLRewritingRule.h"
#include "OPDSFeedReader.h"
#include "OPDSLink.h"
#include "OPDSLink_AdvancedSearch.h"

class OPDSLink::GenericFeedReader : public OPDSFeedReader {

public:
	GenericFeedReader(
		std::vector<shared_ptr<NetworkLink> >& links
	);

public:
	void processFeedEntry(shared_ptr<OPDSEntry> entry);
	void processFeedStart();
	void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed);
	void processFeedEnd();

public:
	void clear();
	void setAdvancedSearch(shared_ptr<OPDSLink::AdvancedSearch> advancedSearch);
	void setAuthenticationType(std::string type);
	void addUrlRewritingRule(shared_ptr<URLRewritingRule> rewritingRule);
	void addRelationAlias(const OPDSLink::RelationAlias&, std::string name);

private:
	std::vector<shared_ptr<NetworkLink> >& myLinks;

private:
	std::string myAuthenticationType;
	std::vector<shared_ptr<URLRewritingRule> > myUrlRewritingRules;
	shared_ptr<OPDSLink::AdvancedSearch> myAdvancedSearch;
	std::map<OPDSLink::RelationAlias,std::string> myRelationAliases;
};

#endif /* __OPDSLINK_GENERICFEEDREADER_H__ */
