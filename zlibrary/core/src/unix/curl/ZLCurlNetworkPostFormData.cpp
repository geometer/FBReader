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

#include <ZLNetworkReader.h>
#include <ZLNetworkUtil.h>

#include "ZLCurlNetworkPostFormData.h"


ZLCurlNetworkPostFormData::ZLCurlNetworkPostFormData(const std::string &url, const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader) : 
	ZLCurlNetworkReadResponseData(url, reader) {
	init(formData);
}

ZLCurlNetworkPostFormData::ZLCurlNetworkPostFormData(const std::string &url, const std::string &sslCertificate, const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader) : 
	ZLCurlNetworkReadResponseData(url, sslCertificate, reader) {
	init(formData);
}

void ZLCurlNetworkPostFormData::init(const std::vector<std::pair<std::string, std::string> > &formData) {
	myPostItem = 0;
	myLastItem = 0;
	for (size_t i = 0; i < formData.size(); ++i) {
		CURLFORMcode code = curl_formadd(&myPostItem, &myLastItem, CURLFORM_COPYNAME, formData[i].first.c_str(), CURLFORM_COPYCONTENTS, formData[i].second.c_str(), CURLFORM_END);
		if (code != 0) {
			setErrorMessage("Invalid form data for " + ZLNetworkUtil::hostFromUrl(url())); // TODO: localize
			return;
		}
	}
}

ZLCurlNetworkPostFormData::~ZLCurlNetworkPostFormData() {
	if (myPostItem != 0) {
		curl_formfree(myPostItem);
		myPostItem = 0;
		myLastItem = 0;
	}
}

bool ZLCurlNetworkPostFormData::doBefore() {
	if (!errorMessage().empty()) {
		return false;
	}

	CURL *h = handle();

	if (myPostItem != 0 && h != 0) {
		curl_easy_setopt(h, CURLOPT_HTTPPOST, myPostItem);
	}

	return ZLCurlNetworkReadResponseData::doBefore();
}
