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

#include <ZLNetworkManager.h>
#include <ZLNetworkRequest.h>

#include "OPDSBasicAuthenticationManager.h"
#include "OPDSNetworkAuthenticationReader.h"

#include "../NetworkLibraryItems.h"
#include "../NetworkErrors.h"


OPDSBasicAuthenticationManager::OPDSBasicAuthenticationManager(
		const std::string &siteName,
		const std::string &signInUrl,
		const std::string &signOutUrl) :
	NetworkAuthenticationManager(siteName),
	mySignInUrl(signInUrl),
	mySignOutUrl(signOutUrl),
	myAccountChecked(false),
	myAccountUserNameOption(ZLCategoryKey::NETWORK, siteName, "accountUserName", "") {
}


NetworkAuthenticationManager::AuthenticationStatus OPDSBasicAuthenticationManager::isAuthorised(bool useNetwork) {
	bool authState = !myAccountUserNameOption.value().empty();
	if (myAccountChecked || !useNetwork) {
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		return AuthenticationStatus(false);
	}

	shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createReadResponseRequest(
		mySignInUrl,
		certificate(),
		new OPDSNetworkAuthenticationReader(SiteName)
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);

	ZLExecutionData::Vector dataList;
	dataList.push_back(data);
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	if (!error.empty()) {
		if (error != NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			return AuthenticationStatus(error);
		}
		myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		return AuthenticationStatus(false);
	}
	myAccountChecked = true;
	return AuthenticationStatus(true);
}

std::string OPDSBasicAuthenticationManager::authorise(const std::string &pwd) {

	shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createReadResponseRequest(
		mySignInUrl,
		certificate(),
		new OPDSNetworkAuthenticationReader(SiteName)
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);
	request.setupAuthentication(ZLNetworkRequest::BASIC, UserNameOption.value(), pwd);

	ZLExecutionData::Vector dataList;
	dataList.push_back(data);
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	myAccountChecked = true;
	if (!error.empty()) {
		myAccountUserNameOption.setValue("");
		return error;
	}
	myAccountUserNameOption.setValue(UserNameOption.value());
	return "";
}

void OPDSBasicAuthenticationManager::logOut() {
	myAccountChecked = true;
	myAccountUserNameOption.setValue("");

	if (!mySignOutUrl.empty()) {
		// TODO: is it so necessary to clean up cookies???
		ZLExecutionData::Vector dataList;
		dataList.push_back(ZLNetworkManager::Instance().createNoActionRequest(
			mySignOutUrl,
			certificate()
		));
		ZLNetworkManager::Instance().perform(dataList);
	}
}






std::string OPDSBasicAuthenticationManager::networkBookId(const NetworkLibraryBookItem &) {
	return "";
}

NetworkLibraryBookItem::URLType OPDSBasicAuthenticationManager::downloadLinkType(const NetworkLibraryBookItem &) {
	return NetworkLibraryBookItem::NONE;
}

const std::string &OPDSBasicAuthenticationManager::currentUserName() {
	return myAccountUserNameOption.value();
}


/*const std::string &OPDSBasicAuthenticationManager::certificate() {
	static const std::string _empty = "";
	return _empty;
}*/
