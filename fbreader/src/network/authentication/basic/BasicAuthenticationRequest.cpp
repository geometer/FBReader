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

#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "BasicAuthenticationRequest.h"

#include "../../NetworkErrors.h"

bool parseHTTPStatusLine(const std::string &line, std::string &httpVersion, std::string &statusCode, std::string &reasonPhrase) {
	static const std::string PREFIX = "HTTP/";

	if (line.size() < PREFIX.size() || ZLUnicodeUtil::toUpper(line.substr(0, PREFIX.size())) != PREFIX) {
		return false;
	}
	size_t index = PREFIX.size();

	httpVersion.clear();
	statusCode.clear();
	reasonPhrase.clear();

	while (index < line.size() && isdigit(line[index])) {
		httpVersion.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != '.') {
		return false;
	}
	httpVersion.append(1, '.');
	while (index < line.size() && isdigit(line[index])) {
		httpVersion.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != ' ') {
		return false;
	}
	while (index < line.size() && isdigit(line[index])) {
		statusCode.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != ' ') {
		return false;
	}
	while (index < line.size() && line[index] != '\r' && line[index] != '\n') {
		reasonPhrase.append(1, line[index++]);
	}
	return true;
}

BasicAuthenticationRequest::BasicAuthenticationRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate) : ZLNetworkGetRequest(url, sslCertificate) {
}

bool BasicAuthenticationRequest::handleHeader(void *ptr, size_t size) {
	std::string line((const char*)ptr, size);
	std::string version, code, phrase;
	if (myStatusCode.empty() && ::parseHTTPStatusLine(line, version, code, phrase)) {
		myStatusCode = code;
	}
	return true;
}

bool BasicAuthenticationRequest::handleContent(void*, size_t) {
	return true;
}

bool BasicAuthenticationRequest::doBefore() {
	return true;
}

bool BasicAuthenticationRequest::doAfter(bool success) {
	if (!success) {
		return true;
	}
	if (myStatusCode.size() != 3) {
		return false;
	}
	if (myStatusCode == "401") {
		setErrorMessage(NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED));
		return false;
	}
	if (myStatusCode[0] == '4' || myStatusCode[0] == '5') {
		return false;
	}
	return true;
}
