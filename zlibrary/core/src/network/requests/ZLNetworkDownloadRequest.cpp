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

#include <cstdlib>
#include <algorithm>

#include <ZLFile.h>
#include <ZLOutputStream.h>
#include <ZLStringUtil.h>

#include <ZLResource.h>

#include "ZLNetworkDownloadRequest.h"


ZLNetworkDownloadRequest::ZLNetworkDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate,
		const std::string &fileName) :
	ZLNetworkGetRequest(url, sslCertificate),
	myFileName(fileName),
	myFileSize(-1),
	myDownloadedSize(0) {
}

ZLNetworkDownloadRequest::ZLNetworkDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate,
		shared_ptr<ZLOutputStream> stream) :
	ZLNetworkGetRequest(url, sslCertificate),
	myFileSize(-1),
	myDownloadedSize(0),
	myOutputStream(stream) {
}

bool ZLNetworkDownloadRequest::doBefore() {
	if (myFileName.empty()) {
		return !myOutputStream.isNull() && myOutputStream->open();
	}
	myOutputStream = ZLFile(myFileName).outputStream(true);
	if (myOutputStream.isNull() || !myOutputStream->open()) {
		const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
		setErrorMessage(ZLStringUtil::printf(errorResource["couldntCreateFileMessage"].value(), myFileName));
		return false;
	}
	return true;
}

bool ZLNetworkDownloadRequest::doAfter(bool success) {
	myOutputStream->close();
	if (!success && !myFileName.empty()) {
		ZLFile(myFileName).remove();
	}
	return true;
}

bool ZLNetworkDownloadRequest::handleHeader(void *ptr, size_t size) {
	static const std::string prefix = "Content-Length: ";
	std::string headerData((const char *) ptr, size);
	if (ZLStringUtil::stringStartsWith(headerData, prefix)) {
		int len = atoi(headerData.c_str() + prefix.length());
		if (len > 0) {
			myFileSize = len;
		}
	}
	setPercent(0, myFileSize);
	return true;
}

bool ZLNetworkDownloadRequest::handleContent(void *ptr, size_t size) {
	myOutputStream->write((const char *) ptr, size);
	myDownloadedSize += size;
	setPercent(myDownloadedSize, myFileSize);
	return true;
}
