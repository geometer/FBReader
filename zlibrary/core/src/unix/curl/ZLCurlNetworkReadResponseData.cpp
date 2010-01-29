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

#include <ZLNetworkReader.h>

#include <ZLStringUtil.h>

#include "ZLCurlNetworkReadResponseData.h"


static size_t handleHeader(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadResponseData *data) {
	return data->parseHeader(ptr, size, nmemb);
}

static size_t handleData(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadResponseData *data) {
	return data->parseData(ptr, size, nmemb);
}


ZLCurlNetworkReadResponseData::ZLCurlNetworkReadResponseData(const std::string &url, shared_ptr<ZLNetworkReader> reader) :
	ZLCurlNetworkData(url),
	myReader(reader) {
}

ZLCurlNetworkReadResponseData::ZLCurlNetworkReadResponseData(const std::string &url, const std::string &sslCertificate, shared_ptr<ZLNetworkReader> reader) :
	ZLCurlNetworkData(url, sslCertificate),
	myReader(reader) {
}

bool ZLCurlNetworkReadResponseData::doBefore() {
	CURL *h = handle();
	if (h != 0) {
		curl_easy_setopt(h, CURLOPT_HEADERFUNCTION, handleHeader);
		curl_easy_setopt(h, CURLOPT_WRITEHEADER, this);
		curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, handleData);
		curl_easy_setopt(h, CURLOPT_WRITEDATA, this);
	}
	return true;
}

void ZLCurlNetworkReadResponseData::doAfter(bool) {
}

size_t ZLCurlNetworkReadResponseData::parseHeader(void *ptr, size_t size, size_t nmemb) {
	const size_t len = size * nmemb;
	if (len == 0) {
		return 0;
	}

	std::string key((const char *) ptr, len);
	std::string data;

	const size_t index = key.find(':');

	if (index != std::string::npos) {
		if (index + 1 < key.size()) {
			data.assign(key, index + 1, std::string::npos);
		}
		key.erase(index);
	}

	ZLStringUtil::stripWhiteSpaces(key);
	ZLStringUtil::stripWhiteSpaces(data);

	if (key.empty() && data.empty()) {
		return len;
	}

	std::string error = myReader->handleHeader(key, data);
	if (!error.empty()) {
		setErrorMessage(error);
		return 0;
	}

	return len;
}

size_t ZLCurlNetworkReadResponseData::parseData(void *ptr, size_t size, size_t nmemb) {
	const size_t len = size * nmemb;
	if (len == 0) {
		return 0;
	}

	const std::string data((const char *) ptr, len);
	std::string error = myReader->handleData(data);
	if (!error.empty()) {
		setErrorMessage(error);
		return 0;
	}
	return len;	
}
