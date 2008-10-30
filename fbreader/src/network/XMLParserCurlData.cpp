/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLXMLReader.h>

#include "XMLParserCurlData.h"

static size_t parseXML(void *ptr, size_t size, size_t nmemb, void *data) {
	((ZLXMLReader*)data)->readFromBuffer((const char*)ptr, size * nmemb);
	return size * nmemb;
}

XMLParserCurlData::XMLParserCurlData(const std::string &url, shared_ptr<ZLXMLReader> reader) : myURL(url), myReader(reader) {
	myHandle = curl_easy_init();

	if (myHandle == 0) {
		return;
	}

	curl_easy_setopt(myHandle, CURLOPT_URL, myURL.c_str());

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, parseXML);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &*myReader);
	curl_easy_setopt(myHandle, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(myHandle, CURLOPT_CONNECTTIMEOUT, 5);
	myReader->initialize();
}

XMLParserCurlData::~XMLParserCurlData() {
	if (myHandle == 0) {
		return;
	}

	myReader->shutdown();
	curl_easy_cleanup(myHandle);
}

CURL *XMLParserCurlData::handle() {
	return myHandle;
}
