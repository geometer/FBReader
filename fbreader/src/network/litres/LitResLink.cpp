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

#include <cctype>
#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

#include "LitResLink.h"
#include "LitResDataParser.h"
#include "LitResAuthenticationManager.h"
#include "LitResAuthorsParser.h"
#include "LitResGenre.h"

#include "../NetworkOperationData.h"
#include "../NetworkItems.h"
#include "../NetworkComparators.h"
#include "../NetworkErrors.h"

const std::string LitResLink::CURRENCY_SUFFIX = " р.";

static const std::string LITRES_SITENAME = "litres.ru";
static const std::string RUSSIAN_LETTERS = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЭЮЯ";

static void appendToAnnotation(std::string &anno, const std::string str) {
	if (str.empty()) {
		return;
	}
	if (!anno.empty()) {
		anno.append(" ");
	}
	anno.append(str);
}


LitResLink::LitResLink() : 
	NetworkLink(LITRES_SITENAME, "Каталог LitRes") {
	myAuthenticationManager = new LitResAuthenticationManager(*this);
	myGenresValid = false;
}

shared_ptr<ZLExecutionData> LitResLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) const {
	return ZLNetworkManager::Instance().createXMLParserRequest(
		litresUrl("pages/catalit_browser/?checkpoint=2000-01-01&search=" + ZLNetworkUtil::htmlEncode(pattern)),
		new LitResDataParser(*this, result.Items)
	);
}

shared_ptr<ZLExecutionData> LitResLink::advancedSearchData(NetworkOperationData &result, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) const {
	std::string request = "?checkpoint=2000-01-01";
	ZLNetworkUtil::appendParameter(request, "search_title", titleAndSeries);
	ZLNetworkUtil::appendParameter(request, "search_person", author);
	if (!tag.empty()) {
		std::vector<std::string> genreIds;
		fillGenreIds(tag, genreIds);
		if (!genreIds.empty()) {
			for (std::vector<std::string>::const_iterator it = genreIds.begin(); it != genreIds.end(); ++it) {
				ZLNetworkUtil::appendParameter(request, "genre", *it);
			}
		}
	}
	ZLNetworkUtil::appendParameter(request, "search", annotation); // if it is included, than annotation words also are searched in title, author, etc.

	if (request.empty()) {
		return 0;
	}

	return ZLNetworkManager::Instance().createXMLParserRequest(
		litresUrl("pages/catalit_browser/" + request),
		new LitResDataParser(*this, result.Items)
	);
}

shared_ptr<NetworkAuthenticationManager> LitResLink::authenticationManager() const {
	return myAuthenticationManager;
}



class LitResRootCatalogItem : public NetworkCatalogItem {

public:
	LitResRootCatalogItem(
		const LitResLink &link,
		const std::string &title,
		const std::string &summary
	);

private:
	std::string loadChildren(NetworkItem::List &children);
};

class LitResCatalogItem : public NetworkCatalogItem {

public:
	LitResCatalogItem(
		const LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary,
		bool sortItems = false,
		CatalogType catalogType = NetworkCatalogItem::OTHER
	);

private:
	std::string loadChildren(NetworkItem::List &children);
	CatalogType catalogType() const;

private:
	const bool mySortItems;
	const CatalogType myCatalogType;
};

class LitResMyCatalogItem : public NetworkCatalogItem {

public:
	LitResMyCatalogItem(const LitResLink &link);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkItem::List &children);

private:
	bool myForceReload;
};

class LitResByAuthorsCatalogItem : public NetworkCatalogItem {

public:
	LitResByAuthorsCatalogItem(const LitResLink &link);

private:
	std::string loadChildren(NetworkItem::List &children);
};

class LitResAuthorsItem : public NetworkCatalogItem {

public:
	LitResAuthorsItem(
		const LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary
	);

private:
	std::string loadChildren(NetworkItem::List &children);
};

class LitResGenresItem : public NetworkCatalogItem {

public:
	LitResGenresItem(
		const LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary,
		const std::vector<shared_ptr<LitResGenre> > &genres
	);

private:
	std::string loadChildren(NetworkItem::List &children);

private:
	const std::vector<shared_ptr<LitResGenre> > &myGenres;
};

LitResRootCatalogItem::LitResRootCatalogItem(
	const LitResLink &link,
	const std::string &title,
	const std::string &summary
) : NetworkCatalogItem(link, "", "", title, summary, "feed-litres.png", std::map<URLType,std::string>()) {
}

std::string LitResRootCatalogItem::loadChildren(NetworkItem::List &children) {
	const LitResLink &link = (const LitResLink&)Link;
	children.push_back(new LitResCatalogItem(
		link,
		link.litresUrl("pages/catalit_browser/?rating=hot"),
		"Горячие новинки",
		"Новые поступления за неделю"
	));
	children.push_back(new LitResCatalogItem(
		link,
		link.litresUrl("pages/catalit_browser/?rating=books"),
		"Популярные книги",
		"50 самых популярных книг"
	));
	children.push_back(new LitResByAuthorsCatalogItem(link));
	children.push_back(new LitResGenresItem(
		link,
		"none",
		"Книги по жанрам",
		"Просмотр книг по жанрам",
		link.genresTree()
	));
	children.push_back(new LitResMyCatalogItem(link));

	return "";
}

LitResCatalogItem::LitResCatalogItem(
	const LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary,
	bool sortItems,
	CatalogType catalogType
) : NetworkCatalogItem(link, url, "", title, summary, "", std::map<URLType,std::string>()), mySortItems(sortItems), myCatalogType(catalogType) {
}

std::string LitResCatalogItem::loadChildren(NetworkItem::List &children) {
	children.clear();

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		URL,
		new LitResDataParser((const LitResLink&)Link, children)
	);

	const std::string error = ZLNetworkManager::Instance().perform(networkData);
	
	if (mySortItems) {
		std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	}

	return error;
}

NetworkCatalogItem::CatalogType LitResCatalogItem::catalogType() const {
	return myCatalogType;
}


LitResMyCatalogItem::LitResMyCatalogItem(const LitResLink &link) : NetworkCatalogItem(
	link,
	"none",
	"",
	"Мои книги",
	"Купленные книги",
	"",
	std::map<URLType,std::string>(),
	LoggedUsers
) {
	myForceReload = false;
}

void LitResMyCatalogItem::onDisplayItem() {
	myForceReload = false;
}

std::string LitResMyCatalogItem::loadChildren(NetworkItem::List &children) {
	LitResAuthenticationManager &mgr =
		(LitResAuthenticationManager&)*Link.authenticationManager();
	if (mgr.isAuthorised().Status == B3_FALSE) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	std::string error;
	if (myForceReload) {
		error = mgr.reloadPurchasedBooks();
	}
	myForceReload = true;
	mgr.collectPurchasedBooks(children);
	std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	return error;
}

LitResByAuthorsCatalogItem::LitResByAuthorsCatalogItem(const LitResLink &link) : NetworkCatalogItem(
	link,
	"none",
	"",
	"Книги по авторам",
	"Просмотр книг по авторам",
	"",
	std::map<URLType,std::string>()
) {
}
	
std::string LitResByAuthorsCatalogItem::loadChildren(NetworkItem::List &children) {
	children.clear();

	const LitResLink &link = (const LitResLink&)Link;

	children.push_back(new LitResAuthorsItem(
		link,
		link.litresUrl("pages/catalit_persons/?rating=1"),
		"Популярные авторы",
		"50 самых популярных за последнюю неделю авторов"
	));

	const char *ptr = RUSSIAN_LETTERS.data();
	const char *end = ptr + RUSSIAN_LETTERS.size();
	while (ptr < end) {
		ZLUnicodeUtil::Ucs4Char ch;
		const int len = ZLUnicodeUtil::firstChar(ch, ptr);
		const std::string letter(ptr, len);
		ptr += len;
		children.push_back(new LitResAuthorsItem(
			link,
			link.litresUrl("pages/catalit_persons/?search_last_name=" + ZLNetworkUtil::htmlEncode(letter + "%")),
			letter,
			"Авторы на букву " + letter
		));
	}
	return "";
}

LitResAuthorsItem::LitResAuthorsItem(
	const LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary
) : NetworkCatalogItem(link, url, "", title, summary, "", std::map<URLType,std::string>()) {
}

std::string LitResAuthorsItem::loadChildren(NetworkItem::List &children) {
	children.clear();

	std::vector<LitResAuthor> authors;

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		URL,
		new LitResAuthorsParser(authors)
	);

	std::string error = ZLNetworkManager::Instance().perform(networkData);
	if (!error.empty()) {
		return error;
	}

	std::sort(authors.begin(), authors.end());

	const LitResLink &link = (const LitResLink&)Link;

	for (std::vector<LitResAuthor>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		std::string anno;
		appendToAnnotation(anno, it->FirstName);
		appendToAnnotation(anno, it->MiddleName);
		appendToAnnotation(anno, it->LastName);

		children.push_back(new LitResCatalogItem(
			link,
			link.litresUrl("pages/catalit_browser/?checkpoint=2000-01-01&person=" + ZLNetworkUtil::htmlEncode(it->Id)),
			it->Title,
			anno,
			true,
			NetworkCatalogItem::BY_AUTHORS
		));
	}
	return "";
}


LitResGenresItem::LitResGenresItem(
	const LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary,
	const std::vector<shared_ptr<LitResGenre> > &genres
) : NetworkCatalogItem(
	link,
	url,
	"",
	title,
	summary,
	"",
	std::map<URLType,std::string>()
),
myGenres(genres) {
}

std::string LitResGenresItem::loadChildren(NetworkItem::List &children) {
	const LitResLink &link = (const LitResLink&)Link;
	for (std::vector<shared_ptr<LitResGenre> >::const_iterator it = myGenres.begin(); it != myGenres.end(); ++it) {
		const LitResGenre &genre = **it;
		if (genre.Id.empty()) {
			children.push_back(new LitResGenresItem(
				link,
				"none",
				genre.Title,
				"",
				genre.Children
			));
		} else {
			children.push_back(new LitResCatalogItem(
				link,
				link.litresUrl("pages/catalit_browser/?checkpoint=2000-01-01&genre=" + ZLNetworkUtil::htmlEncode(genre.Id)),
				genre.Title,
				"",
				true
			));
		}
	}
	return "";
}

shared_ptr<NetworkItem> LitResLink::libraryItem() const {
	return new LitResRootCatalogItem(*this, Title, "Продажа электронных книг.");
}

void LitResLink::rewriteUrl(std::string &url, bool) const {
	ZLNetworkUtil::appendParameter(url, "lfrom", "51");
}

std::string LitResLink::litresUrl(const std::string &path) const {
	std::string url = "://robot.litres.ru/" + path;
	if (ZLNetworkUtil::hasParameter(url, "sid") ||
			ZLNetworkUtil::hasParameter(url, "pwd")) {
		url = "https" + url;
	} else {
		url = "http" + url;
	}
	rewriteUrl(url);
	return url;
}
