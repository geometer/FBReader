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

#include "ZLNetworkPostFormRequest.h"


ZLNetworkPostFormRequest::ZLNetworkPostFormRequest(const std::string &url, const std::string &sslCertificate,
		const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader) :
	ZLNetworkPostRequest(url, sslCertificate, formData),
	myReader(reader) {
}

bool ZLNetworkPostFormRequest::handleHeader(void *ptr, size_t size) {
	if (size == 0) {
		return true;
	}
	std::string error = myReader->handleHeader(ptr, size);
	if (!error.empty()) {
		setErrorMessage(error);
		return false;
	}
	return true;
}

bool ZLNetworkPostFormRequest::handleContent(void *ptr, size_t size) {
	if (size == 0) {
		return true;
	}
	std::string error = myReader->handleContent(ptr, size);
	if (!error.empty()) {
		setErrorMessage(error);
		return false;
	}
	return true;
}

bool ZLNetworkPostFormRequest::doBefore() {
	return true;
}

void ZLNetworkPostFormRequest::doAfter(bool) {
}
