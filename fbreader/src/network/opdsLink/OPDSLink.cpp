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
#include "OPDSCatalogItem.h"

#include "../NetworkOperationData.h"
#include "../NetworkAuthenticationManager.h"
#include "../opds/OPDSXMLParser.h"
#include "../opds/NetworkOPDSFeedReader.h"

class OPDSLink::AdvancedSearch {

public:
	AdvancedSearch(
		const std::string &type,
		const std::string &titleParameter,
		const std::string &authorParameter,
		const std::string &tagParameter,
		const std::string &annotationParameter
	);

	std::string query(
		const std::string &titleOrSeries,
		const std::string &author,
		const std::string &tag,
		const std::string &annotation
	) const;

private:
	void addSubQuery(std::string &query, const std::string &name, const std::string &value) const;

private:
	const std::string myType;
	const std::string myTitleParameter;
	const std::string myAuthorParameter;
	const std::string myTagParameter;
	const std::string myAnnotationParameter;
};

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

shared_ptr<ZLExecutionData> OPDSLink::createNetworkData(const std::string &url, NetworkOperationData &result) {
	if (url.empty()) {
		return 0;
	}
	return ZLNetworkManager::Instance().createXMLParserRequest(
		url,
		new OPDSXMLParser(new NetworkOPDSFeedReader(url, result, myIgnoredFeeds, myAccountDependentFeeds))
	);
}

OPDSLink::OPDSLink(
	const std::string &siteName,
	const std::string &catalogURL,
	const std::string &searchURL,
	const std::string &title,
	const std::string &summary,
	const std::string &iconName
) : NetworkLink(siteName, title),
	myCatalogURL(catalogURL),
	mySearchURL(searchURL),
	mySummary(summary),
	myIconName(iconName) {
}

OPDSLink::~OPDSLink() {
}

void OPDSLink::setupAdvancedSearch(
	const std::string &type,
	const std::string &titleParameter,
	const std::string &authorParameter,
	const std::string &tagParameter,
	const std::string &annotationParameter
) {
	myAdvancedSearch = new AdvancedSearch(type, titleParameter, authorParameter, tagParameter, annotationParameter);
}

void OPDSLink::setIgnoredFeeds(const std::set<std::string> &ignoredFeeds) {
	myIgnoredFeeds = ignoredFeeds;
}

void OPDSLink::setAccountDependentFeeds(const std::set<std::string> &accountDependentFeeds) {
	myAccountDependentFeeds = accountDependentFeeds;
}

void OPDSLink::setAuthenticationManager(shared_ptr<NetworkAuthenticationManager> mgr) {
	myAuthenticationManager = mgr;
}


shared_ptr<NetworkLibraryItem> OPDSLink::libraryItem() {
	return new OPDSCatalogItem(
		*this,
		myCatalogURL,
		"",
		Title,
		mySummary,
		myIconName
	);
}

const std::string OPDSLink::searchURL(const std::string &query) const {
	return ZLStringUtil::printf(mySearchURL, query);
}

shared_ptr<ZLExecutionData> OPDSLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) {
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
) {
	if (myAdvancedSearch.isNull()) {
		return 0;
	}
	std::string query = myAdvancedSearch->query(
		titleAndSeries, author, tag, annotation
	);
	return query.empty() ? 0 : createNetworkData(searchURL(query), result);
}

shared_ptr<ZLExecutionData> OPDSLink::resume(NetworkOperationData &data) {
	if (++data.ResumeCount >= 10) {
		return 0;
	}
	const std::string url = data.ResumeURI;
	data.clear();
	return createNetworkData(url, data);
}

shared_ptr<NetworkAuthenticationManager> OPDSLink::authenticationManager() {
	return myAuthenticationManager;
}
