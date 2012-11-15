/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLNetworkUtil.h>
#include <ZLStringUtil.h>

#include "../NetworkLink.h"
#include "../opds/OPDSMetadata.h"

#include "LitResBookshelfItem.h"
#include "LitResBooksFeedItem.h"
#include "LitResRecommendationsItem.h"
#include "LitResByGenresItem.h"
#include "LitResAuthorsItem.h"

#include "LitResUtil.h"


static std::string LITRES_API_URL = "://robot.litres.ru/";

std::string LitResUtil::url(const std::string &path) {
	std::string url = LITRES_API_URL + path;
	if (ZLNetworkUtil::hasParameter(url, "sid") ||
			ZLNetworkUtil::hasParameter(url, "pwd")) {
		url = "https" + url;
	} else {
		url = "http" + url;
	}
	return url;
}

std::string LitResUtil::url(const NetworkLink &link, const std::string &path) {
	std::string urlString = url(path);
	link.rewriteUrl(urlString);
	return urlString;
}

std::string LitResUtil::url(bool secure, const std::string &path) {
	std::string url = LITRES_API_URL + path;
	if (secure) {
		url = "https" + url;
	} else {
		url = "http" + url;
	}
	return url;
}

std::string LitResUtil::url(const NetworkLink &link, bool secure, const std::string &path) {
	std::string urlString = url(secure, path);
	link.rewriteUrl(urlString, true);
	return urlString;
}

std::string LitResUtil::generateTrialUrl(std::string bookId) {
	std::size_t len = bookId.length();
	if (len < 8) {
		bookId = std::string(8 - len, '0') + bookId;
	}
	std::string query = "static/trials/%s/%s/%s/%s.fb2.zip";
	query = ZLStringUtil::printf(query, bookId.substr(0,2));
	query = ZLStringUtil::printf(query, bookId.substr(2,2));
	query = ZLStringUtil::printf(query, bookId.substr(4,2));
	query = ZLStringUtil::printf(query, bookId);
	return url(false, query);
}

std::string LitResUtil::generatePurchaseUrl(const NetworkLink &link, const std::string &bookId) {
	std::string query;
	ZLNetworkUtil::appendParameter(query, "art", bookId);
	return url(link, true, "pages/purchase_book/" + query);
}

std::string LitResUtil::generateDownloadUrl(const std::string &bookId) {
	std::string query;
	ZLNetworkUtil::appendParameter(query, "art", bookId);
	return url(true, "pages/catalit_download_book/" + query);
}

std::string LitResUtil::generateAlsoReadUrl(const std::string &bookId) {
	std::string query;
	ZLNetworkUtil::appendParameter(query, "rating", "with");
	ZLNetworkUtil::appendParameter(query, "art", bookId);
	return url(false, "pages/catalit_browser/" + query);
}

std::string LitResUtil::generateBooksByGenreUrl(const std::string &genreId) {
	std::string query;
	ZLNetworkUtil::appendParameter(query, "checkpoint", "2000-01-01");
	ZLNetworkUtil::appendParameter(query, "genre", genreId);
	return url(false, "pages/catalit_browser/" + query);
}

std::string LitResUtil::generateBooksByAuthorUrl(const std::string &authorId) {
	std::string query;
	ZLNetworkUtil::appendParameter(query, "checkpoint", "2000-01-01");
	ZLNetworkUtil::appendParameter(query, "person", authorId);
	return url(false, "pages/catalit_browser/" + query);
}

shared_ptr<NetworkItem> LitResUtil::createLitResNode(shared_ptr<ZLMimeType> type, std::string rel, const NetworkLink &link, std::string title,
	std::string annotation, std::map<NetworkItem::URLType,std::string> urlMap,	bool dependsOnAccount) {
	static const std::string TYPE = "type";
	static const std::string GROUP_SERIES = "groupSeries";

	std::string litresType = type->getParameter(TYPE);

	if (rel == OPDSConstants::REL_BOOKSHELF) {
		return new LitResBookshelfItem(
			link,
			title,
			annotation,
			urlMap,
			NetworkCatalogItem::SIGNED_IN
		);
	} else if (rel == OPDSConstants::REL_RECOMMENDATIONS) {
		return new LitResRecommendationsItem(
			(OPDSLink&)link,
			title,
			annotation,
			urlMap,
			NetworkCatalogItem::HAS_BOOKS
		);
	} else if (litresType == ZLMimeType::APPLICATION_LITRES_XML_BOOKS->getParameter(TYPE)) {
		int flags = NetworkCatalogItem::FLAGS_DEFAULT;
		if (type->getParameter(GROUP_SERIES) == "no") {
			flags &= ~NetworkCatalogItem::FLAG_GROUP_MORE_THAN_1_BOOK_BY_SERIES;
		}
		return new LitResBooksFeedItem(
			false,
			link,
			title,
			annotation,
			urlMap,
			dependsOnAccount ? NetworkCatalogItem::SIGNED_IN : NetworkCatalogItem::ALWAYS,
			flags
		);
	} else if (litresType == ZLMimeType::APPLICATION_LITRES_XML_GENRES->getParameter(TYPE)) {
		return new LitResByGenresItem(
			LitResGenreMap::Instance().genresTree(),
			link,
			title,
			annotation,
			urlMap,
			NetworkCatalogItem::ALWAYS,
			NetworkCatalogItem::FLAG_SHOW_AUTHOR
		);
	} else if (litresType == ZLMimeType::APPLICATION_LITRES_XML_AUTHORS->getParameter(TYPE)) {
		return new LitResAuthorsItem(
			link,
			title,
			annotation,
			urlMap,
			NetworkCatalogItem::ALWAYS
		);
	} else {
		return 0;
	}
}
