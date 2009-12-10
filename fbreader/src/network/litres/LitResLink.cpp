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

#include <ZLNetworkUtil.h>
#include <ZLNetworkXMLParserData.h>
#include <ZLNetworkManager.h>

#include "LitResLink.h"
#include "LitResDataParser.h"
#include "LitResAuthenticationManager.h"
#include "LitResUtil.h"

#include "../NetworkOperationData.h"
#include "../NetworkLibraryItems.h"
#include "../NetworkErrors.h"

static const std::string LITRES_SITENAME = "litres.ru";

LitResLink::LitResLink() : 
	NetworkLink(LITRES_SITENAME, "Каталог LitRes") {
	myAuthenticationManager = new LitResAuthenticationManager(LITRES_SITENAME);
}

shared_ptr<ZLExecutionData> LitResLink::simpleSearchData(NetworkOperationData &result, const std::string &pattern) {
	return new ZLNetworkXMLParserData(
		LitResUtil::litresLink("pages/catalit_browser/?checkpoint=2000-01-01&search=" + ZLNetworkUtil::htmlEncode(pattern)),
		new LitResDataParser(result.Items, LitResUtil::Instance().genres(), myAuthenticationManager)
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

	return new ZLNetworkXMLParserData(
		LitResUtil::litresLink("pages/catalit_browser/" + request),
		new LitResDataParser(result.Items, LitResUtil::Instance().genres(), myAuthenticationManager)
	);
}

shared_ptr<NetworkAuthenticationManager> LitResLink::authenticationManager() {
	return myAuthenticationManager;
}

class LitResRootSubCatalogLoader : public NetworkSubCatalogLoader {

public:
	std::string load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children);
};

class LitResSubCatalogLoader : public NetworkSubCatalogLoader {

public:
	std::string load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children);
};

class LitResMySubCatalogLoader : public NetworkSubCatalogLoader {

public:
	std::string load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children);
};

std::string LitResRootSubCatalogLoader::load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children) {
	children.push_back(new NetworkLibraryCatalogItem(
		item.link(),
		LitResUtil::litresLink("pages/catalit_browser/?rating=hot"),
		"",
		"Горячие новинки",
		"Новые поступления за неделю",
		"",
		new LitResSubCatalogLoader()
	));
	children.push_back(new NetworkLibraryCatalogItem(
		item.link(),
		LitResUtil::litresLink("pages/catalit_browser/?rating=books"),
		"",
		"Популярные книги",
		"50 самых популярных книг",
		"",
		new LitResSubCatalogLoader()
	));
	children.push_back(new NetworkLibraryCatalogItem(
		item.link(),
		"none",
		"",
		"Мои книги",
		"Мои приобретенные книги",
		"",
		new LitResMySubCatalogLoader(),
		/* dependsOnAccount = */true
	));

	return "";
}

std::string LitResSubCatalogLoader::load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children) {
	children.clear();

	std::string errorMessage;
	ZLExecutionData::Vector dataList;
	dataList.push_back(new ZLNetworkXMLParserData(
		item.url(), 
		new LitResDataParser(children, LitResUtil::Instance().genres(), item.link().authenticationManager())
	));

	std::string error = ZLNetworkManager::Instance().perform(dataList);
	if (errorMessage.empty()) {
		errorMessage = error;
	}

	/*for (NetworkLibraryItemList::iterator it = children.begin(); it != children.end(); ++it) {
		NetworkLibraryBookItem &book = (NetworkLibraryBookItem &) **it;
		book.setIndex(0);
	}*/

	return errorMessage;
}

std::string LitResMySubCatalogLoader::load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children) {
	LitResAuthenticationManager &mgr = (LitResAuthenticationManager&)*item.link().authenticationManager();
	if (mgr.isAuthorised() == B3_FALSE) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	mgr.collectPurchasedBooks(children);
	return "";
}

shared_ptr<NetworkLibraryItem> LitResLink::libraryItem() {
	return new NetworkLibraryCatalogItem(
		*this,
		"",
		"",
		Title,
		"Продажа электронных книг.",
		"feed-litres.png",
		new LitResRootSubCatalogLoader()
	);
}
