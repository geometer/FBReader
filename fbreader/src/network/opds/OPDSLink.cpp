/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

#include "OPDSLink.h"
#include "OPDSLink_AdvancedSearch.h"
#include "OPDSCatalogItem.h"
#include "OPDSXMLParser.h"
#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"
#include "../authentication/NetworkAuthenticationManager.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "URLRewritingRule.h"

OPDSLink::AdvancedSearch::AdvancedSearch(
	const std::string &type,
	const std::string &titleParameter,
	const std::string &authorParameter,
	const std::string &tagParameter,
	const std::string &annotationParameter
) : myType(type), myTitleParameter(titleParameter), myAuthorParameter(authorParameter), myTagParameter(tagParameter), myAnnotationParameter(annotationParameter) {
}

void OPDSLink::AdvancedSearch::addSubQuery(std::string &query, const std::string &name, const std::string &value) const {
	if (value.empty()) {
		return;
	}

	if (myType == "separateWords") {
		std::size_t start = 0, end;
		do {
			end = value.find(' ', start);
			std::string ss = value.substr(start, end - start);
			ZLUnicodeUtil::utf8Trim(ss);
			if (!ss.empty()) {
				if (!query.empty()) {
					query.append("+");
				}
				query.append(name + ':');
				query.append(ZLNetworkUtil::htmlEncode(ss));
			}
			start = end + 1;
		} while (end != std::string::npos);
	} else if (myType == "quoted") {
		std::string encodedValue = value;
		ZLUnicodeUtil::utf8Trim(encodedValue);

		if (encodedValue.empty()) {
			return;
		}
		encodedValue = '"' + encodedValue + '"';
		std::replace(encodedValue.begin(), encodedValue.end(), ' ', '+');

		if (!query.empty()) {
			query += '+';
		}
		query += name + ':' + ZLNetworkUtil::htmlEncode(encodedValue);
	}
}

std::string OPDSLink::AdvancedSearch::query(
	const std::string &titleOrSeries,
	const std::string &author,
	const std::string &tag,
	const std::string &annotation
) const {
	std::string query;
	addSubQuery(query, myTitleParameter, titleOrSeries);
	addSubQuery(query, myAuthorParameter, author);
	addSubQuery(query, myTagParameter, tag);
	addSubQuery(query, myAnnotationParameter, annotation);
	return query;
}

//shared_ptr<NetworkLink> OPDSLink::read(const ZLFile &file) {
//	Reader reader;
//	reader.readDocument(file);
//	return reader.link();
//}

shared_ptr<ZLNetworkRequest> OPDSLink::createNetworkRequest(const std::string &url, NetworkOperationData &result) const {
	if (url.empty()) {
		return 0;
	}
	std::string modifiedUrl(url);
	rewriteUrl(modifiedUrl);
	return ZLNetworkManager::Instance().createXMLParserRequest(modifiedUrl, new OPDSXMLParser(new NetworkOPDSFeedReader(*this, url, result))	);
}

OPDSLink::OPDSLink(
		const std::string &siteName
) : NetworkLink(siteName) {
}

OPDSLink::~OPDSLink() {
}

shared_ptr<NetworkItem> OPDSLink::libraryItem() const {
	NetworkItem::UrlInfoCollection urlMap;
	urlMap[NetworkItem::URL_COVER] = getIcon();
	urlMap[NetworkItem::URL_CATALOG] = url(URL_MAIN);
	return new OPDSCatalogItem(*this, getTitle(), getSummary(), urlMap);
}

const std::string OPDSLink::searchURL(const std::string &query) const {
	return ZLStringUtil::printf(url(URL_SEARCH), query);
}

shared_ptr<ZLNetworkRequest> OPDSLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) const {
	return createNetworkRequest(
		searchURL(ZLNetworkUtil::htmlEncode(pattern)),
		result
	);
}

shared_ptr<ZLNetworkRequest> OPDSLink::advancedSearchData(
	NetworkOperationData &result,
	const std::string &titleAndSeries,
	const std::string &author,
	const std::string &tag,
	const std::string &annotation
) const {
	if (myAdvancedSearch.isNull()) {
		return 0;
	}
	std::string query = myAdvancedSearch->query(
		titleAndSeries, author, tag, annotation
	);
	return query.empty() ? 0 : createNetworkRequest(searchURL(query), result);
}

shared_ptr<ZLNetworkRequest> OPDSLink::resume(NetworkOperationData &data) const {
	const std::string url = data.ResumeURI;
	return createNetworkRequest(url, data);
}

shared_ptr<NetworkAuthenticationManager> OPDSLink::authenticationManager() const {
	return myAuthenticationManager;
}

void OPDSLink::setUrlRewritingRules(std::vector<shared_ptr<URLRewritingRule> > rules) {
	myUrlRewritingRules = rules;
}

void OPDSLink::setAuthenticationManager(shared_ptr<NetworkAuthenticationManager> manager) {
	myAuthenticationManager = manager;
}

void OPDSLink::setAdvancedSearch(shared_ptr<OPDSLink::AdvancedSearch> advancedSearch) {
	myAdvancedSearch = advancedSearch;
}

void OPDSLink::setRelationAliases(std::map<RelationAlias, std::string> relationAliases) {
	myRelationAliases = relationAliases;
}

void OPDSLink::rewriteUrl(std::string &url, bool isUrlExternal) const {
	URLRewritingRule::RuleApply apply = isUrlExternal ? URLRewritingRule::EXTERNAL : URLRewritingRule::INTERNAL;
	for (std::vector<shared_ptr<URLRewritingRule> >::const_iterator it = myUrlRewritingRules.begin(); it != myUrlRewritingRules.end(); ++it) {
		const URLRewritingRule &rule = **it;
		if (rule.whereToApply() == apply) {
			url = rule.apply(url);
		}
	}
}

OPDSLink::RelationAlias::RelationAlias(const std::string &alias, const std::string &type) : Alias(alias), Type(type) {
}

bool OPDSLink::RelationAlias::operator < (const RelationAlias &alias) const {
	int cmp = Alias.compare(alias.Alias);
	if (cmp != 0) {
		return cmp < 0;
	}
	return Type < alias.Type;
}

const std::string &OPDSLink::relation(const std::string &rel, const std::string &type) const {
	RelationAlias alias(rel, type);
	std::map<RelationAlias,std::string>::const_iterator it = myRelationAliases.find(alias);
	if (it != myRelationAliases.end()) {
		return it->second;
	}
	if (!type.empty()) {
		alias.Type.erase();
		it = myRelationAliases.find(alias);
		if (it != myRelationAliases.end()) {
			return it->second;
		}
	}
	return rel;
}
