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

#ifndef __OPDSLINK_H__
#define __OPDSLINK_H__

#include <map>
#include <vector>
#include <string>

#include "../NetworkLink.h"

class ZLFile;

class NetworkAuthenticationManager;
struct URLRewritingRule;

class OPDSLink : public NetworkLink {

public:
	enum FeedCondition {
		CONDITION_NEVER,
		CONDITION_SIGNED_IN,
	};

private:
	class AdvancedSearch;

public:
	class GenericReader;
	class FeedReader;
	class GenericFeedReader;
	class GenericXMLParser;

	OPDSLink(
		const std::string &siteName
	);

public:
	~OPDSLink();

private:
	struct RelationAlias;

public:
	void setUrlRewritingRules(std::vector<shared_ptr<URLRewritingRule> > rules);
	void setAuthenticationManager(shared_ptr<NetworkAuthenticationManager> manager);
	void setAdvancedSearch(shared_ptr<OPDSLink::AdvancedSearch> advancedSearch);
	void setRelationAliases(std::map<RelationAlias, std::string> relationAliases);

private:
	const std::string searchURL(const std::string &pattern) const;

	shared_ptr<ZLNetworkRequest> createNetworkData(const std::string &url, NetworkOperationData &result) const;

	shared_ptr<ZLNetworkRequest> simpleSearchData(
		NetworkOperationData &result,
		const std::string &pattern) const;
	shared_ptr<ZLNetworkRequest> advancedSearchData(
		NetworkOperationData &result,
		const std::string &titleAndSeries,
		const std::string &author,
		const std::string &tag,
		const std::string &annotation) const;
	shared_ptr<ZLNetworkRequest> resume(NetworkOperationData &result) const;

	shared_ptr<NetworkItem> libraryItem() const;
	shared_ptr<NetworkAuthenticationManager> authenticationManager() const;

	void rewriteUrl(std::string &url, bool isUrlExternal = false) const;

	const std::string &relation(const std::string &rel, const std::string &type) const;

private:
	shared_ptr<AdvancedSearch> myAdvancedSearch;

	struct RelationAlias {
		std::string Alias;
		std::string Type;

		RelationAlias(const std::string &alias, const std::string &type);
		bool operator < (const RelationAlias &other) const;
	};
	std::map<RelationAlias, std::string> myRelationAliases;

	std::map<std::string,FeedCondition> myFeedConditions;
	std::vector<shared_ptr<URLRewritingRule> > myUrlRewritingRules;

	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;

friend class NetworkOPDSFeedReader;
friend class OPDSCatalogItem;
friend class OPDSBookItem;
};

#endif /* __OPDSLINK_H__ */
