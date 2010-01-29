/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLCurlNetworkNoActionData.h"

static size_t handleData(void *ptr, size_t size, size_t nmemb, void *data) {
	return size * nmemb;
}


ZLCurlNetworkNoActionData::ZLCurlNetworkNoActionData(const std::string &url, const std::string &sslCertificate) : ZLCurlNetworkData(url, sslCertificate) {
}

bool ZLCurlNetworkNoActionData::doBefore() {
	CURL *h = handle();
	if (h != 0) {
		curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, handleData);
		curl_easy_setopt(h, CURLOPT_WRITEDATA, this);
	}
	return true;
}

void ZLCurlNetworkNoActionData::doAfter(bool success) {
}

void ZLCurlNetworkNoActionData::onCancel() {
}
