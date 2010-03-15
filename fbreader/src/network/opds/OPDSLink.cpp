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

#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

#include "OPDSLink.h"
#include "OPDSLink_Reader.h"
#include "OPDSLink_AdvancedSearch.h"
#include "OPDSCatalogItem.h"
#include "OPDSXMLParser.h"
#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"
#include "../authentication/NetworkAuthenticationManager.h"

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
		size_t start = 0, end;
		do {
			end = value.find(' ', start);
			std::string ss = value.substr(start, end - start);
			ZLStringUtil::stripWhiteSpaces(ss);
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
		ZLStringUtil::stripWhiteSpaces(encodedValue);

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

shared_ptr<NetworkLink> OPDSLink::read(const std::string &path) {
	Reader reader;
	reader.readDocument(path);
	return reader.link();
}

shared_ptr<ZLExecutionData> OPDSLink::createNetworkData(const std::string &url, NetworkOperationData &result) const {
	if (url.empty()) {
		return 0;
	}
	std::string modifiedUrl(url);
	rewriteUrl(modifiedUrl);
	return ZLNetworkManager::Instance().createXMLParserRequest(
		modifiedUrl,
		new OPDSXMLParser(new NetworkOPDSFeedReader(*this, url, result))
	);
}

OPDSLink::OPDSLink(
	const std::string &siteName,
	const std::string &title,
	const std::string &summary,
	const std::string &icon,
	const std::map<std::string,std::string> &links
) : NetworkLink(siteName, title, summary, icon, links) {
}

OPDSLink::~OPDSLink() {
}

shared_ptr<NetworkItem> OPDSLink::libraryItem() const {
	std::map<NetworkItem::URLType,std::string> urlMap;
	urlMap[NetworkItem::URL_COVER] = Icon;
	urlMap[NetworkItem::URL_CATALOG] = url(URL_MAIN);
	return new OPDSCatalogItem(*this, Title, Summary, urlMap);
}

const std::string OPDSLink::searchURL(const std::string &query) const {
	return ZLStringUtil::printf(url(URL_SEARCH), query);
}

shared_ptr<ZLExecutionData> OPDSLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) const {
	return createNetworkData(
		searchURL(ZLNetworkUtil::htmlEncode(pattern)),
		result
	);
}

shared_ptr<ZLExecutionData> OPDSLink::advancedSearchData(
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
	return query.empty() ? 0 : createNetworkData(searchURL(query), result);
}

shared_ptr<ZLExecutionData> OPDSLink::resume(NetworkOperationData &data) const {
	const std::string url = data.ResumeURI;
	data.clear();
	return createNetworkData(url, data);
}

shared_ptr<NetworkAuthenticationManager> OPDSLink::authenticationManager() const {
	return myAuthenticationManager;
}

void OPDSLink::rewriteUrl(std::string &url, bool isUrlExternal) const {
	for (std::vector<shared_ptr<URLRewritingRule> >::const_iterator it = myUrlRewritingRules.begin(); it != myUrlRewritingRules.end(); ++it) {
		const URLRewritingRule &rule = **it;

		if (rule.Apply != URLRewritingRule::ALWAYS) {
			if ((rule.Apply == URLRewritingRule::EXTERNAL && !isUrlExternal)
				|| (rule.Apply == URLRewritingRule::INTERNAL && isUrlExternal)) {
				continue;
			}
		}

		switch (rule.Type) {
		case URLRewritingRule::ADD_URL_PARAMETER:
			ZLNetworkUtil::appendParameter(url, rule.Name, rule.Value);
			break;
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
