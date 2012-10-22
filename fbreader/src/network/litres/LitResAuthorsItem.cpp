/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLNetworkManager.h>

#include "../NetworkLink.h"
#include "../NetworkComparators.h"
#include "../NetworkErrors.h"
#include "../NetworkItems.h"

#include "LitResUtil.h"
#include "LitResAuthorsParser.h"
#include "LitResBooksFeedItem.h"

#include "LitResAuthorsItem.h"

LitResAuthorsItem::LitResAuthorsItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility,
	int flags
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	accessibility,
	flags
) {
}

std::string LitResAuthorsItem::loadChildren(NetworkItem::List &children) {
	//TODO maybe add sid parameter if possible
	//(at LitRes API documentation it said that's adding sid _always_ is a good practice)
	LitResAuthorsParser::AuthorsList authors;
	shared_ptr<ZLNetworkRequest> data = ZLNetworkManager::Instance().createXMLParserRequest(
		getCatalogUrl(),
		new LitResAuthorsParser(authors)
	);
	std::string error = ZLNetworkManager::Instance().perform(data);
	if (error.empty()) {
		fillChildrenWithAuthors(children, authors);
	}
	return error;
}

void LitResAuthorsItem::fillChildrenWithAuthors(NetworkItem::List &children, const LitResAuthorsParser::AuthorsList &authors) {
	for (size_t i = 0; i < authors.size(); ++i) {
		const LitResAuthorsParser::LitresAuthorData &author = authors.at(i);

		UrlInfoCollection urlByType = URLByType;
		urlByType[NetworkItem::URL_CATALOG] = LitResUtil::generateBooksByAuthorUrl(author.Id);
		//TODO add icon change for one author here
		//urlByType[NetworkItem::URL_COVER] =
		children.push_back(new LitResBooksFeedItem(
			true,
			Link,
			author.DisplayName,
			getSubtitle(author),
			urlByType,
			NetworkCatalogItem::AlWAYS,
			NetworkCatalogItem::FLAG_GROUP_MORE_THAN_1_BOOK_BY_SERIES
		));
	}
}

std::string LitResAuthorsItem::getSubtitle(const LitResAuthorsParser::LitresAuthorData &author) {
	static const std::string SPACE = " ";
	std::string subtitle = author.FirstName;
	if (!author.MiddleName.empty()) {
		if (!subtitle.empty()) {
			subtitle += SPACE;
		}
		subtitle += author.MiddleName;
	}
	if (!author.LastName.empty()) {
		if (!subtitle.empty()) {
			subtitle += SPACE;
		}
		subtitle += author.LastName;
	}
	return subtitle;
}
