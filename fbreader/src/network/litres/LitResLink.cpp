/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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
#include "LitResUtil.h"
#include "LitResAuthorsParser.h"
#include "LitResGenre.h"

#include "../NetworkOperationData.h"
#include "../NetworkLibraryItems.h"
#include "../NetworkComparators.h"
#include "../NetworkErrors.h"

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
	myAuthenticationManager = new LitResAuthenticationManager(LITRES_SITENAME);
}

shared_ptr<ZLExecutionData> LitResLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) {
	return ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_browser/?checkpoint=2000-01-01&search=" + ZLNetworkUtil::htmlEncode(pattern)),
		new LitResDataParser(result.Items, myAuthenticationManager)
	);
}

shared_ptr<ZLExecutionData> LitResLink::advancedSearchData(NetworkOperationData &result, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) {
	std::string request = "?checkpoint=2000-01-01";
	ZLNetworkUtil::addParameter(request, "search_title", titleAndSeries);
	ZLNetworkUtil::addParameter(request, "search_person", author);
	if (!tag.empty()) {
		std::vector<std::string> genreIds;
		LitResUtil::Instance().fillGenreIds(tag, genreIds);
		if (!genreIds.empty()) {
			for (std::vector<std::string>::const_iterator it = genreIds.begin(); it != genreIds.end(); ++it) {
				ZLNetworkUtil::addParameter(request, "genre", *it);
			}
		}
	}
	ZLNetworkUtil::addParameter(request, "search", annotation); // if it is included, than annotation words also are searched in title, author, etc.

	if (request.empty()) {
		return 0;
	}

	return ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_browser/" + request),
		new LitResDataParser(result.Items, myAuthenticationManager)
	);
}

shared_ptr<NetworkAuthenticationManager> LitResLink::authenticationManager() {
	return myAuthenticationManager;
}



class LitResRootCatalogItem : public NetworkLibraryCatalogItem {

public:
	LitResRootCatalogItem(
		LitResLink &link,
		const std::string &title,
		const std::string &summary
	);

private:
	std::string loadChildren(NetworkLibraryItemList &children);
};

class LitResCatalogItem : public NetworkLibraryCatalogItem {

public:
	LitResCatalogItem(
		LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary,
		bool sortItems = false
	);

private:
	std::string loadChildren(NetworkLibraryItemList &children);

private:
	const bool mySortItems;
};

class LitResMyCatalogItem : public NetworkLibraryCatalogItem {

public:
	LitResMyCatalogItem(LitResLink &link);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkLibraryItemList &children);

private:
	bool myForceReload;
};

class LitResByAuthorsCatalogItem : public NetworkLibraryCatalogItem {

public:
	LitResByAuthorsCatalogItem(LitResLink &link);

private:
	std::string loadChildren(NetworkLibraryItemList &children);
};

class LitResAuthorsItem : public NetworkLibraryCatalogItem {

public:
	LitResAuthorsItem(
		LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary
	);

private:
	std::string loadChildren(NetworkLibraryItemList &children);
};

class LitResByGenresCatalogItem : public NetworkLibraryCatalogItem {

public:
	LitResByGenresCatalogItem(LitResLink &link);

private:
	std::string loadChildren(NetworkLibraryItemList &children);
};

class LitResGenresItem : public NetworkLibraryCatalogItem {

public:
	LitResGenresItem(
		LitResLink &link,
		const std::string &url,
		const std::string &title,
		const std::string &summary,
		const std::vector<shared_ptr<LitResGenre> > &genres
	);

private:
	std::string loadChildren(NetworkLibraryItemList &children);

private:
	const std::vector<shared_ptr<LitResGenre> > &myGenres;
};

LitResRootCatalogItem::LitResRootCatalogItem(
	LitResLink &link,
	const std::string &title,
	const std::string &summary
) : NetworkLibraryCatalogItem(link, "", "", title, summary, "feed-litres.png") {
}

std::string LitResRootCatalogItem::loadChildren(NetworkLibraryItemList &children) {
	children.push_back(new LitResCatalogItem(
		(LitResLink&)link(),
		LitResUtil::litresLink("pages/catalit_browser/?rating=hot"),
		"Горячие новинки",
		"Новые поступления за неделю"
	));
	children.push_back(new LitResCatalogItem(
		(LitResLink&)link(),
		LitResUtil::litresLink("pages/catalit_browser/?rating=books"),
		"Популярные книги",
		"50 самых популярных книг"
	));
	children.push_back(new LitResByAuthorsCatalogItem((LitResLink&)link()));
	children.push_back(new LitResGenresItem(
		(LitResLink&)link(),
		"none",
		"Книги по категориям",
		"Просмтор книг по категориям",
		LitResUtil::Instance().genresTree()
	));

	children.push_back(new LitResMyCatalogItem((LitResLink&)link()));

	return "";
}

LitResCatalogItem::LitResCatalogItem(
	LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary,
	bool sortItems
) : NetworkLibraryCatalogItem(link, url, "", title, summary, ""), mySortItems(sortItems) {
}

std::string LitResCatalogItem::loadChildren(NetworkLibraryItemList &children) {
	children.clear();

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		url(), 
		new LitResDataParser(children, link().authenticationManager())
	));

	const std::string error = ZLNetworkManager::Instance().perform(dataList);
	
	if (mySortItems) {
		std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	}

	return error;
}

LitResMyCatalogItem::LitResMyCatalogItem(LitResLink &link) : NetworkLibraryCatalogItem(
	link,
	"none",
	"",
	"Мои книги",
	"Мои приобретенные книги",
	"",
	true
) {
	myForceReload = false;
}

void LitResMyCatalogItem::onDisplayItem() {
	myForceReload = false;
}

std::string LitResMyCatalogItem::loadChildren(NetworkLibraryItemList &children) {
	LitResAuthenticationManager &mgr = (LitResAuthenticationManager&)*link().authenticationManager();
	if (mgr.isAuthorised() == B3_FALSE) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	std::string error;
	if (myForceReload) {
		error = mgr.reloadPurchasedBooks();
	}
	myForceReload = true;
	mgr.collectPurchasedBooks(children);
	return error;
}

LitResByAuthorsCatalogItem::LitResByAuthorsCatalogItem(LitResLink &link) : NetworkLibraryCatalogItem(
	link,
	"none",
	"",
	"Книги по авторам",
	"Просмтор книг по авторам",
	""
) {
}
	
std::string LitResByAuthorsCatalogItem::loadChildren(NetworkLibraryItemList &children) {
	children.clear();

	children.push_back(new LitResAuthorsItem(
		(LitResLink&)link(),
		LitResUtil::litresLink("pages/catalit_persons/?rating=1"),
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
			(LitResLink&)link(),
			LitResUtil::litresLink("pages/catalit_persons/?search_last_name=" + ZLNetworkUtil::htmlEncode(letter + "%")),
			letter,
			"Авторы на букву " + letter
		));
	}
	return "";
}

LitResAuthorsItem::LitResAuthorsItem(
	LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary
) : NetworkLibraryCatalogItem(link, url, "", title, summary, "") {
}

std::string LitResAuthorsItem::loadChildren(NetworkLibraryItemList &children) {
	children.clear();

	std::vector<LitResAuthor> authors;

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		url(), 
		new LitResAuthorsParser(authors)
	));

	std::string error = ZLNetworkManager::Instance().perform(dataList);
	if (!error.empty()) {
		return error;
	}

	std::sort(authors.begin(), authors.end());

	for (std::vector<LitResAuthor>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		std::string anno;
		appendToAnnotation(anno, it->FirstName);
		appendToAnnotation(anno, it->MiddleName);
		appendToAnnotation(anno, it->LastName);

		children.push_back(new LitResCatalogItem(
			(LitResLink&)link(),
			LitResUtil::litresLink("pages/catalit_browser/?checkpoint=2000-01-01&person=" + ZLNetworkUtil::htmlEncode(it->Id)),
			it->Title,
			anno,
			true
		));
	}
	return "";
}

LitResGenresItem::LitResGenresItem(
	LitResLink &link,
	const std::string &url,
	const std::string &title,
	const std::string &summary,
	const std::vector<shared_ptr<LitResGenre> > &genres
) : NetworkLibraryCatalogItem(link, url, "", title, summary, ""), myGenres(genres) {
}

std::string LitResGenresItem::loadChildren(NetworkLibraryItemList &children) {
	for (std::vector<shared_ptr<LitResGenre> >::const_iterator it = myGenres.begin(); it != myGenres.end(); ++it) {
		const LitResGenre &genre = **it;
		if (genre.Id.empty()) {
			children.push_back(new LitResGenresItem(
				(LitResLink&)link(),
				"none",
				genre.Title,
				"",
				genre.Children
			));
		} else {
			children.push_back(new LitResCatalogItem(
				(LitResLink&)link(),
				LitResUtil::litresLink("pages/catalit_browser/?checkpoint=2000-01-01&genre=" + ZLNetworkUtil::htmlEncode(genre.Id)),
				genre.Title,
				""
			));
		}
	}
	return "";
}


shared_ptr<NetworkLibraryItem> LitResLink::libraryItem() {
	return new LitResRootCatalogItem(*this, Title, "Продажа электронных книг.");
}
