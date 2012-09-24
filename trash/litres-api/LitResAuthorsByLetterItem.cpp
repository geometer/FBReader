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

#include <algorithm>

#include <ZLNetworkManager.h>
#include <ZLStringUtil.h>

#include "../../NetworkLink.h"
#include "../../NetworkComparators.h"
#include "../../NetworkErrors.h"
#include "../../NetworkItems.h"

#include "LitResUtil.h"
#include "LitResAuthorsParser.h"
#include "LitResAuthorsByLetterItem.h"

#include "LitResAuthorsByLetterItem.h"

LitResAuthorsByLetterItem::LitResAuthorsByLetterItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const std::map<URLType,std::string> &urlByType,
	VisibilityType visibility,
	int flags
) : LitResAuthorsItem(
	link,
	title,
	summary,
	urlByType,
	visibility,
	flags
) {
}

std::string LitResAuthorsByLetterItem::loadChildren(NetworkItem::List &children) {
	//TODO maybe add sid parameter if possible
	//(at LitRes API documentation it said that's adding sid _always_ is a good practice)
	LitResAuthorsParser::AuthorsList authors;
	shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createXMLParserRequest(
		getCatalogUrl(),
		new LitResAuthorsParser(authors)
	);
	std::string error = ZLNetworkManager::Instance().perform(data);

	if (!error.empty()) {
		return error;
	}

	std::sort(authors.begin(), authors.end());

	if (authors.size() <= 30) {
		fillChildrenWithAuthors(children, authors);
		return error;
	}

	std::map<std::string, int> letterCounter = getLetterCounter(authors);
	static std::string alph[] =
		{"",  "а", "б", "в", "г", "д",
		"е", "ё", "ж", "з", "и", "й",
		"к", "л", "м", "н", "о", "п",
		"р", "с", "т", "у", "ф", "х",
		"ц", "ч", "ш", "щ", "ъ", "ы",
		"ь", "э", "ю", "я"};
	std::vector<std::string> letters(alph, alph + 34);

	unsigned int sum = 0;
	std::string start = letters.front();
	std::string end = start;

	for (size_t i = 0; i < letters.size(); ++i) {
		std::string l = letters.at(i);
		if (letterCounter.find(l) == letterCounter.end()) {
			continue;
		}
		int count = letterCounter[l];
		if (count > 25) {
			if (sum > 0) {
				addSubCatalog(start, end, sum, children, authors);
				sum = 0;
			}
			addSubCatalog(l, l, count, children, authors);
		} else {
			if (sum == 0) {
				start = l;
			}
			end = l;
			sum += count;
			if (sum >= 25) {
				addSubCatalog(start, end, sum, children, authors);
				sum = 0;
			}
		}
	}
	if (sum > 0) {
		addSubCatalog(start, end, sum, children, authors);
	}

	return error;
}

std::map<std::string, int> LitResAuthorsByLetterItem::getLetterCounter(const LitResAuthorsParser::AuthorsList &authors) {
	std::map<std::string, int> letterCounter;
	//TODO rewrite to work with unicode values
	for (size_t i = 0; i < authors.size(); ++i) {
		const LitResAuthorsParser::LitresAuthorData &author = authors.at(i);
		std::string letter = author.SortKey.substr(2,2);
		if (letterCounter.find(letter) == letterCounter.end()) {
			letterCounter[letter] = 1;
		} else {
			++letterCounter[letter];
		}
	}
	return letterCounter;
}

void LitResAuthorsByLetterItem::addSubCatalog(std::string start, std::string end, unsigned int sum, NetworkItem::List &children, const LitResAuthorsParser::AuthorsList &authors) {
	std::string firstLetter = authors.at(0).LastName.substr(0,2);
	std::string title;
	if (start == end) {
		title = firstLetter + start;
	} else {
		title = firstLetter + start + "-" + firstLetter + end;
	}

	std::string subtitle;
	ZLStringUtil::appendNumber(subtitle, sum);
	subtitle += " ";
	if (sum % 100 <= 19 && sum % 100 >= 10) {
		subtitle += "авторов";
	} else if (sum % 10 == 1) {
		subtitle += "автор";
	} else if (sum % 10 == 2 || sum % 10 == 3 || sum % 10 == 4) {
		subtitle += "автора";
	} else {
		subtitle += "авторов";
	}

	LitResAuthorsParser::AuthorsList filteredAuthors;
	for (size_t i = 0; i < authors.size(); ++i) {
		const LitResAuthorsParser::LitresAuthorData &author = authors.at(i);
		std::string secondLetter = author.SortKey.substr(2,2);
		int compareStart = secondLetter.compare(start);
		int compareEnd = secondLetter.compare(end);
		if (compareStart >= 0 && compareEnd <= 0) {
			filteredAuthors.push_back(author);
		}
	}

	children.push_back(new LitResAuthorsByTwoLetterItem(filteredAuthors, Link, title, subtitle, URLByType, Always));
}

LitResAuthorsByTwoLetterItem::LitResAuthorsByTwoLetterItem(
	const LitResAuthorsParser::AuthorsList &authors,
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const std::map<URLType,std::string> &urlByType,
	VisibilityType visibility,
	int flags
) : LitResAuthorsItem(
	link,
	title,
	summary,
	urlByType,
	visibility,
	flags
), myAuthors(authors) {
}

std::string LitResAuthorsByTwoLetterItem::loadChildren(NetworkItem::List &children) {
	fillChildrenWithAuthors(children, myAuthors);
	return std::string();
}
