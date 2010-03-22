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

#include "BasicAuthenticationManager.h"
#include "BasicAuthenticationRequest.h"

#include "../../NetworkLink.h"
#include "../../NetworkItems.h"
#include "../../NetworkErrors.h"

BasicAuthenticationManager::BasicAuthenticationManager(const NetworkLink &link) :
	NetworkAuthenticationManager(link),
	myCertificate(link.SiteName != "feedbooks.com" ? ZLNetworkSSLCertificate::NULL_CERTIFICATE : ZLNetworkSSLCertificate::DONT_VERIFY_CERTIFICATE),
	myAccountChecked(false),
	myAccountUserNameOption(ZLCategoryKey::NETWORK, link.SiteName, "accountUserName", "") {
}


NetworkAuthenticationManager::AuthenticationStatus BasicAuthenticationManager::isAuthorised(bool useNetwork) {
	bool authState = !myAccountUserNameOption.value().empty();
	if (myAccountChecked || !useNetwork) {
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		return AuthenticationStatus(false);
	}

	shared_ptr<ZLExecutionData> data = new BasicAuthenticationRequest(
		Link.url(NetworkLink::URL_SIGN_IN),
		certificate()
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);

	std::string error = ZLNetworkManager::Instance().perform(data);

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

std::string BasicAuthenticationManager::authorise(const std::string &pwd) {
	shared_ptr<ZLExecutionData> data = new BasicAuthenticationRequest(
		Link.url(NetworkLink::URL_SIGN_IN),
		certificate()
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);
	request.setupAuthentication(ZLNetworkRequest::BASIC, UserNameOption.value(), pwd);

	std::string error = ZLNetworkManager::Instance().perform(data);

	myAccountChecked = true;
	if (!error.empty()) {
		myAccountUserNameOption.setValue("");
		return error;
	}
	myAccountUserNameOption.setValue(UserNameOption.value());
	return "";
}

void BasicAuthenticationManager::logOut() {
	myAccountChecked = true;
	myAccountUserNameOption.setValue("");

	const std::string signOutUrl = Link.url(NetworkLink::URL_SIGN_OUT);
	if (!signOutUrl.empty()) {
		// TODO: is it so necessary to clean up cookies???
		shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createNoActionRequest(
			signOutUrl,
			certificate()
		);
		ZLNetworkManager::Instance().perform(data);
	}
}

const std::string &BasicAuthenticationManager::currentUserName() {
	return myAccountUserNameOption.value();
}

const ZLNetworkSSLCertificate &BasicAuthenticationManager::certificate() {
	return myCertificate;
}

shared_ptr<BookReference> BasicAuthenticationManager::downloadReference(const NetworkBookItem &book) {
	return 0;
}
