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

#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include <ZLXMLReader.h>

#include "ZLNetworkXMLParserRequest.h"

#include "../ZLPlainAsynchronousInputStream.h"
#include "../ZLGzipAsynchronousInputStream.h"


static const std::string CONTENT_ENCODING = "content-encoding:";


ZLNetworkXMLParserRequest::ZLNetworkXMLParserRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLXMLReader> reader) :
	ZLNetworkGetRequest(url, sslCertificate),
	myReader(reader) {
}

ZLNetworkXMLParserRequest::~ZLNetworkXMLParserRequest() {
	if (!myInputStream.isNull() && myInputStream->initialized() && !myInputStream->eof()) {
		myInputStream->setEof();
		myReader->readDocument(myInputStream);
	}
}

bool ZLNetworkXMLParserRequest::doBefore() {
	return true;
}

bool ZLNetworkXMLParserRequest::doAfter(bool) {
	return true;
}

bool ZLNetworkXMLParserRequest::handleHeader(void *ptr, size_t size) {
	std::string header = ZLUnicodeUtil::toLower(std::string((const char *) ptr, size));

	if (ZLStringUtil::stringStartsWith(header, CONTENT_ENCODING)) {
		std::string encoding = header.substr(CONTENT_ENCODING.size());
		ZLStringUtil::stripWhiteSpaces(encoding);
		myHttpEncoding = encoding;
	}

	return true;
}

bool ZLNetworkXMLParserRequest::handleContent(void *ptr, size_t size) {
	if (myInputStream.isNull()) {
		if (myHttpEncoding == "gzip") {
			myInputStream = new ZLGzipAsynchronousInputStream();
		} else {
			myInputStream = new ZLPlainAsynchronousInputStream();
		}
	}
	myInputStream->setBuffer((const char *) ptr, size);
	bool result = true;
	if (!myReader->readDocument(myInputStream)) {
		result = false;
	}
	if (!myReader->errorMessage().empty()) {
		setErrorMessage(myReader->errorMessage());
		result = false;
	}
	return result;
}
