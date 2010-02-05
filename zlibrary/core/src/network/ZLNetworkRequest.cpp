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

const ZLTypeId ZLNetworkRequest::TYPE_ID(ZLExecutionData::TYPE_ID);
const ZLTypeId ZLNetworkGetRequest::TYPE_ID(ZLNetworkRequest::TYPE_ID);
const ZLTypeId ZLNetworkPostRequest::TYPE_ID(ZLNetworkRequest::TYPE_ID);


ZLNetworkRequest::ZLNetworkRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate) :
	myURL(url),
	mySSLCertificate(sslCertificate),
	myAuthenticationMethod(NO_AUTH),
	myRedirectionSupported(true) {
	ZLLogger::Instance().println("URL", url);
}

const std::string &ZLNetworkRequest::errorMessage() const {
	return myErrorMessage;
}

const std::string &ZLNetworkRequest::url() const {
	return myURL;
}

const ZLNetworkSSLCertificate &ZLNetworkRequest::sslCertificate() const {
	return mySSLCertificate;
}

void ZLNetworkRequest::setErrorMessage(const std::string &message) {
	myErrorMessage = message;
}

bool ZLNetworkRequest::handleHeader(void *, size_t) {
	return true;
}



ZLNetworkGetRequest::ZLNetworkGetRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate) :
	ZLNetworkRequest(url, sslCertificate) {
}

const ZLTypeId &ZLNetworkGetRequest::typeId() const {
	return TYPE_ID;
}



ZLNetworkPostRequest::ZLNetworkPostRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, 
		const std::vector<std::pair<std::string, std::string> > &postData) :
	ZLNetworkRequest(url, sslCertificate),
	myData(postData) {
}

const ZLTypeId &ZLNetworkPostRequest::typeId() const {
	return TYPE_ID;
}

const std::vector<std::pair<std::string, std::string> > &ZLNetworkPostRequest::postData() const {
	return myData;
}
