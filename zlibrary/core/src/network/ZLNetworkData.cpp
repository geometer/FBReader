/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include "ZLNetworkData.h"

ZLNetworkData::ZLNetworkData(const std::string &url) : myURL(url) {
	myHandle = curl_easy_init();
	if (myHandle != 0) {
		curl_easy_setopt(myHandle, CURLOPT_URL, myURL.c_str());
	}
}

ZLNetworkData::~ZLNetworkData() {
	if (myHandle != 0) {
		curl_easy_cleanup(myHandle);
	}
}

const std::string &ZLNetworkData::url() const {
	return myURL;
}

CURL *ZLNetworkData::handle() {
	return myHandle;
}
