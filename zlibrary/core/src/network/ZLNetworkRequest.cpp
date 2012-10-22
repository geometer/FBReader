/*
 * Copyright (C) 2010-2012 Geometer Plus <contact@geometerplus.com>
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

void ZLNetworkRequest::setPostParameters(const std::vector<std::pair<std::string, std::string> > &parameters) {
	myPostParameters = parameters;
}

bool ZLNetworkRequest::hasListener() const {
	return !myListener.isNull();
}

void ZLNetworkRequest::setListener(shared_ptr<Listener> listener) {
	myListener = listener;
}

void ZLNetworkRequest::setPercent(int ready, int full) {
	if (!myListener.isNull()) {
		myListener->showPercent(ready, full);
	}
}

void ZLNetworkRequest::finished(const std::string &error){
	if (!myListener.isNull()) {
		myListener->finished(error);
	}
}

ZLNetworkRequest::Listener::Listener() {
}

ZLNetworkRequest::Listener::~Listener() {
}
