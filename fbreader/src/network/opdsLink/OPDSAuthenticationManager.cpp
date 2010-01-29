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

#include "OPDSAuthenticationManager.h"

#include "../NetworkLibraryItems.h"
#include "../NetworkErrors.h"


OPDSAuthenticationManager::OPDSAuthenticationManager(
	const std::string &siteName,
	const std::string &postLogin,
	const std::string &postPassword,
	const std::string &postSignInUrl,
	const std::string &signOutUrl,
	const std::string &accountUrl) : 
	NetworkAuthenticationManager(siteName),
	myPostLogin(postLogin),
	myPostPassword(postPassword),
	myPostSignInUrl(postSignInUrl),
	mySignOutUrl(signOutUrl),
	myAccountUrl(accountUrl),
	myAccountChecked(false),
	myAccountUserNameOption(ZLCategoryKey::NETWORK, siteName, "accountUserName", "") {
}


NetworkAuthenticationManager::AuthenticationStatus OPDSAuthenticationManager::isAuthorised(bool useNetwork) {
	bool authState = !myAccountUserNameOption.value().empty();
	if (myAccountChecked || !useNetwork) {
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		return AuthenticationStatus(false);
	}

	//std::string firstName, lastName, newSid;
	//shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	ZLExecutionData::Vector dataList;
	//dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
	dataList.push_back(ZLNetworkManager::Instance().createNoActionData(
		myAccountUrl,
		certificate()
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	if (!error.empty()) {
		//if (error != NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			return AuthenticationStatus(error);
		//}
		/*myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		return AuthenticationStatus(false);*/
	}
	myAccountChecked = true;
	return AuthenticationStatus(true);
}

std::string OPDSAuthenticationManager::authorise(const std::string &pwd) {

	std::vector<std::pair<std::string, std::string> > formData;
	formData.push_back(std::make_pair(myPostLogin, UserNameOption.value()));
	formData.push_back(std::make_pair(myPostPassword, pwd));

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createPostFormData(
		myPostSignInUrl,
		certificate(),
		formData
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	myAccountChecked = true;
	if (!error.empty()) {
		myAccountUserNameOption.setValue("");
		return error;
	}
	myAccountUserNameOption.setValue(UserNameOption.value());
	return "";
}

void OPDSAuthenticationManager::logOut() {
	myAccountChecked = true;
	myAccountUserNameOption.setValue("");

	if (!mySignOutUrl.empty()) {
		// TODO: is it so necessary to clean up cookies???
		ZLExecutionData::Vector dataList;
		dataList.push_back(ZLNetworkManager::Instance().createNoActionData(
			mySignOutUrl,
			certificate()
		));
		ZLNetworkManager::Instance().perform(dataList);
	}
}






std::string OPDSAuthenticationManager::networkBookId(const NetworkLibraryBookItem &) {
	return "";
}

NetworkLibraryBookItem::URLType OPDSAuthenticationManager::downloadLinkType(const NetworkLibraryBookItem &) {
	return NetworkLibraryBookItem::NONE;
}

const std::string &OPDSAuthenticationManager::currentUserName() {
	return myAccountUserNameOption.value();
}


/*const std::string &OPDSAuthenticationManager::certificate() {
	static const std::string _empty = "";
	return _empty;
}*/
