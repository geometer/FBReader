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

#include <ZLExecutionData.h>

#include <ZLibrary.h>
#include <ZLFile.h>

#include "NetworkLink.h"
#include "NetworkOperationData.h"

const std::string NetworkLink::URL_MAIN = "main";
const std::string NetworkLink::URL_SEARCH = "search";
const std::string NetworkLink::URL_SIGN_IN = "signIn";
const std::string NetworkLink::URL_SIGN_OUT = "signOut";
const std::string NetworkLink::URL_SIGN_UP = "signUp";
const std::string NetworkLink::URL_REFILL_ACCOUNT = "refillAccount";
const std::string NetworkLink::URL_RECOVER_PASSWORD = "recoverPassword";

std::string NetworkLink::NetworkDataDirectory() {
	return ZLFile(ZLibrary::ApplicationDirectory() + ZLibrary::FileNameDelimiter + "network").path();
}

std::string NetworkLink::CertificatesPathPrefix() {
	return NetworkDataDirectory() + ZLibrary::FileNameDelimiter + "certificates" + ZLibrary::FileNameDelimiter;
}

NetworkLink::NetworkLink(
	const std::string &siteName,
	const std::string &title,
	const std::string &summary,
	const std::string &icon,
	const std::map<std::string,std::string> &links
) : 
	SiteName(siteName), 
	Title(title),
	Summary(summary),
	Icon(icon),
	OnOption(ZLCategoryKey::NETWORK, siteName, "on", true),
	myLinks(links) {
}

NetworkLink::~NetworkLink() {
}

std::string NetworkLink::url(const std::string &urlId) const {
	std::map<std::string,std::string>::const_iterator it = myLinks.find(urlId);
	return (it != myLinks.end()) ? it->second : std::string();
}

shared_ptr<ZLExecutionData> NetworkLink::resume(NetworkOperationData &result) const {
	result.clear();
	return 0;
}
