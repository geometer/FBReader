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

#include <cstdlib>
#include <algorithm>

#include <ZLStringUtil.h>

#include "ZLCurlNetworkReadToStringData.h"

size_t ZLCurlNetworkReadToStringData::handleHeader(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadToStringData *self) {
	static const std::string prefix = "Content-Length: ";
	std::string headerData((char*)ptr, size * nmemb);
	if (ZLStringUtil::stringStartsWith(headerData, prefix)) {
		int len = atoi(headerData.c_str() + prefix.length());
		if (len > 0) {
			self->myDataSize = len;
		}
	}
	self->setPercent(0, self->myDataSize);
	return size * nmemb;
}

size_t ZLCurlNetworkReadToStringData::writeToString(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadToStringData *self) {
	self->myBuffer.append((const char*)ptr, size * nmemb);
	return size * nmemb;
}

ZLCurlNetworkReadToStringData::ZLCurlNetworkReadToStringData(const std::string &url, std::string &buffer, const std::string &sslCertificate) : ZLCurlNetworkData(url, sslCertificate), myBuffer(buffer), myDataSize(0) {
}

bool ZLCurlNetworkReadToStringData::doBefore() {
	CURL *h = handle();
	if (h != 0) {
		curl_easy_setopt(h, CURLOPT_HEADERFUNCTION, handleHeader);
		curl_easy_setopt(h, CURLOPT_WRITEHEADER, this);
		curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, writeToString);
		curl_easy_setopt(h, CURLOPT_WRITEDATA, this);
	}
	return true;
}

void ZLCurlNetworkReadToStringData::doAfter(bool success) {
}

void ZLCurlNetworkReadToStringData::onCancel() {
}
