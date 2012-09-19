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

#include "LitResUtil.h"

#include "../../NetworkLink.h"

std::string LitResUtil::url(const std::string &path) {
	std::string url = "://robot.litres.ru/" + path;
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

std::string LitResUtil::generateTrialUrl(std::string bookId) {
	size_t len = bookId.length();
	if (len < 8) {
		bookId = std::string(8 - len, '0') + bookId;
	}
	std::string url = "http://robot.litres.ru/static/trials/";
	static const std::string DELIMETER = "/";
	url += bookId.substr(0, 2) + DELIMETER;
	url += bookId.substr(2, 2) + DELIMETER;
	url += bookId.substr(4, 2) + DELIMETER;
	url += bookId + ".fb2.zip";
	return url;
}

std::string LitResUtil::generatePurchaseUrl(const std::string &bookId) {
	return "https://robot.litres.ru/pages/purchase_book/?art=" + bookId;
}

std::string LitResUtil::generateDownloadUrl(const std::string &bookId) {
	return "https://robot.litres.ru/pages/catalit_download_book/?art=" + bookId;
}

std::string LitResUtil::generateBooksByGenreUrl(const std::string &genreId) {
	return "http://robot.litres.ru/pages/catalit_browser/?checkpoint=2000-01-01&genre=" + genreId;
}
