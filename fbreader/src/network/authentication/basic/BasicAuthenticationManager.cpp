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
#include <ZLTimeManager.h>

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


NetworkAuthenticationManager::AuthenticationStatus BasicAuthenticationManager::isAuthorised(shared_ptr<ZLExecutionData::Listener> listener) {
	const bool useNetwork = !listener.isNull();
	bool authState = !myAccountUserNameOption.value().empty();
	if (myAccountChecked || !useNetwork) {
		if (!listener.isNull())
			listener->finished(authState ? std::string() : "Not authorised");
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		myAccountChecked = true;
		myAccountUserNameOption.setValue("");
		listener->finished("Not authorised");
		return AuthenticationStatus(false);
	}

	shared_ptr<ZLExecutionData> data = new BasicAuthenticationRequest(
		Link.url(NetworkLink::URL_SIGN_IN),
		certificate()
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);
	request.setHandler(this, &BasicAuthenticationManager::onAuthorisationCheck);
	request.addUserData("listener", listener.staticCast<ZLUserData>());

	return ZLNetworkManager::Instance().perform(data);
}

void BasicAuthenticationManager::onAuthorisationCheck(ZLUserDataHolder &data, const std::string &error) {
	shared_ptr<ZLExecutionData::Listener> listener = data.getUserData("listener").staticCast<ZLExecutionData::Listener>();
	if (!error.empty()) {
		if (error == NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			myAccountChecked = true;
			myAccountUserNameOption.setValue("");
		}
		listener->finished(error);
		return;
	}
	myAccountChecked = true;
	listener->finished(std::string());
}

std::string BasicAuthenticationManager::authorise(const std::string &pwd, shared_ptr<ZLExecutionData::Listener> listener) {
	shared_ptr<ZLExecutionData> data = new BasicAuthenticationRequest(
		Link.url(NetworkLink::URL_SIGN_IN),
		certificate()
	);
	ZLNetworkRequest &request = (ZLNetworkRequest &)*data;

	request.setRedirectionSupported(false);
	request.setupAuthentication(ZLNetworkRequest::BASIC, UserNameOption.value(), pwd);
	request.setHandler(this, &BasicAuthenticationManager::onAuthorised);
	request.addUserData("listener", listener.staticCast<ZLUserData>());

	return ZLNetworkManager::Instance().perform(data);
}


void BasicAuthenticationManager::onAuthorised(ZLUserDataHolder &data, const std::string &error) {
	myAccountChecked = true;
	if (!error.empty())
		myAccountUserNameOption.setValue("");
	else
		myAccountUserNameOption.setValue(UserNameOption.value());
	data.getUserData("listener").staticCast<ZLExecutionData::Listener>()->finished(error);
}

void BasicAuthenticationManager::logOut(shared_ptr<ZLExecutionData::Listener> listener) {
	myAccountChecked = true;
	myAccountUserNameOption.setValue("");

	const std::string signOutUrl = Link.url(NetworkLink::URL_SIGN_OUT);
	if (!signOutUrl.empty()) {
		// TODO: is it so necessary to clean up cookies???
		shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createNoActionRequest(
			signOutUrl,
			certificate()
		);
		data->setListener(listener);
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
        (void)book;
	return 0;
}
