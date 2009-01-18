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

#include <ZLXMLReader.h>

#include "ZLNetworkXMLParserData.h"

static size_t parseXML(void *ptr, size_t size, size_t nmemb, void *data) {
	((ZLXMLReader*)data)->readFromBuffer((const char*)ptr, size * nmemb);
	return size * nmemb;
}

ZLNetworkXMLParserData::ZLNetworkXMLParserData(const std::string &url, shared_ptr<ZLXMLReader> reader) : ZLNetworkData(url), myReader(reader) {
	CURL *h = handle();
	if (h != 0) {
		curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, parseXML);
		curl_easy_setopt(h, CURLOPT_WRITEDATA, &*myReader);
		myReader->initialize();
	}
}

ZLNetworkXMLParserData::~ZLNetworkXMLParserData() {
	if (handle() != 0) {
		myReader->shutdown();
	}
}
