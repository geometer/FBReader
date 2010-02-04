/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLLogger.h>

#include "ZLNetworkRequest.h"


const std::string ZLNetworkRequest::TYPE_ID = "ZLNetworkRequest";

const std::string ZLNetworkGetRequest::REQUEST_TYPE = "ZLNetworkGetRequest";
const std::string ZLNetworkPostRequest::REQUEST_TYPE = "ZLNetworkPostRequest";


ZLNetworkRequest::ZLNetworkRequest(const std::string &url, const std::string &sslCertificate) : myURL(url), mySSLCertificate(sslCertificate) {
	ZLLogger::Instance().println("URL", url);
}

ZLNetworkRequest::~ZLNetworkRequest() {
}

const std::string &ZLNetworkRequest::type() const {
	return TYPE_ID;
}

const std::string &ZLNetworkRequest::errorMessage() const {
	return myErrorMessage;
}

const std::string &ZLNetworkRequest::url() const {
	return myURL;
}

const std::string &ZLNetworkRequest::sslCertificate() const {
	return mySSLCertificate;
}

void ZLNetworkRequest::setErrorMessage(const std::string &message) {
	myErrorMessage = message;
}

bool ZLNetworkRequest::handleHeader(void *, size_t) {
	return true;
}



ZLNetworkGetRequest::ZLNetworkGetRequest(const std::string &url, const std::string &sslCertificate) :
	ZLNetworkRequest(url, sslCertificate) {
}

const std::string &ZLNetworkGetRequest::requestType() const {
	return REQUEST_TYPE;
}


ZLNetworkPostRequest::ZLNetworkPostRequest(const std::string &url, const std::string &sslCertificate, const std::vector<std::pair<std::string, std::string> > &postData) :
	ZLNetworkRequest(url, sslCertificate),
	myData(postData) {
}

const std::string &ZLNetworkPostRequest::requestType() const {
	return REQUEST_TYPE;
}

const std::vector<std::pair<std::string, std::string> > &ZLNetworkPostRequest::postData() const {
	return myData;
}
