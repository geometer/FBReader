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

#include <ZLFile.h>
#include <ZLOutputStream.h>
#include <ZLStringUtil.h>

#include "ZLCurlNetworkDownloadData.h"

size_t ZLCurlNetworkDownloadData::handleHeader(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkDownloadData *self) {
	static const std::string prefix = "Content-Length: ";
	std::string headerData((char*)ptr, size * nmemb);
	if (ZLStringUtil::stringStartsWith(headerData, prefix)) {
		int len = atoi(headerData.c_str() + prefix.length());
		if (len > 0) {
			self->myFileSize = len;
		}
	}
	self->setPercent(0, self->myFileSize);
	return size * nmemb;
}

size_t ZLCurlNetworkDownloadData::writeToStream(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkDownloadData *self) {
	self->myOutputStream->write((const char*)ptr, size * nmemb);
	self->myDownloadedSize += size * nmemb;
	self->setPercent(self->myDownloadedSize, self->myFileSize);
	return size * nmemb;
}

ZLCurlNetworkDownloadData::ZLCurlNetworkDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream) : ZLNetworkData(url), myFileName(fileName), myFileSize(-1), myDownloadedSize(0), myOutputStream(stream) {
}

ZLCurlNetworkDownloadData::ZLCurlNetworkDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream) : ZLNetworkData(url, sslCertificate), myFileName(fileName), myFileSize(-1), myDownloadedSize(0), myOutputStream(stream) {
}

bool ZLCurlNetworkDownloadData::doBefore() {
	if (myOutputStream.isNull()) {
		myOutputStream = ZLFile(myFileName).outputStream();
	}
	if (myOutputStream.isNull() || !myOutputStream->open()) {
		return false;
	}

	CURL *h = handle();
	if (h != 0) {
		curl_easy_setopt(h, CURLOPT_HEADERFUNCTION, handleHeader);
		curl_easy_setopt(h, CURLOPT_WRITEHEADER, this);
		curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, writeToStream);
		curl_easy_setopt(h, CURLOPT_WRITEDATA, this);
	}
	return true;
}

void ZLCurlNetworkDownloadData::doAfter(bool success) {
	myOutputStream->close();
	if (!success) {
		ZLFile(myFileName).remove();
	}
}

void ZLCurlNetworkDownloadData::onCancel() {
}
