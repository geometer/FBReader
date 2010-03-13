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
