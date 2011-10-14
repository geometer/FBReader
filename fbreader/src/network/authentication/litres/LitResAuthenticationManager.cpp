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

#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>
#include <ZLTimeManager.h>

#include "LitResAuthenticationManager.h"
#include "LitResDataParser.h"
#include "LitResAuthenticationDataParser.h"
#include "LitResUtil.h"

#include "../../NetworkErrors.h"
#include "../../NetworkLink.h"
#include "../../NetworkLinkCollection.h"

LitResAuthenticationManager::LitResAuthenticationManager(const NetworkLink &link) :
	NetworkAuthenticationManager(link), 
	mySidChecked(false), 
	mySidUserNameOption(ZLCategoryKey::NETWORK, link.SiteName, "sidUserName", ""), 
	mySidOption(ZLCategoryKey::NETWORK, link.SiteName, "sid", ""),
	myCertificate(NetworkLink::CertificatesPathPrefix() + "litres.ru.crt") {
}

const ZLNetworkSSLCertificate &LitResAuthenticationManager::certificate() {
	return myCertificate;
}

class LitResAuthorisationScope : public ZLUserData {
public:
	std::string firstName;
	std::string lastName;
	std::string newSid;
	shared_ptr<ZLExecutionData::Listener> listener;
};

NetworkAuthenticationManager::AuthenticationStatus LitResAuthenticationManager::isAuthorised(shared_ptr<ZLExecutionData::Listener> listener) {
	const bool useNetwork = !listener.isNull();
	bool authState = !mySidUserNameOption.value().empty() && !mySidOption.value().empty();
	if (mySidChecked || !useNetwork) {
		if (!listener.isNull())
			listener->finished(authState ? std::string() : "Not authorised");
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		listener->finished("Not authorised");
		return AuthenticationStatus(false);
	}

	LitResAuthorisationScope *scope = new LitResAuthorisationScope;
	scope->listener = listener;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(scope->firstName, scope->lastName, scope->newSid);

	std::string url = Link.url(NetworkLink::URL_SIGN_IN);
	ZLNetworkUtil::appendParameter(url, "sid", mySidOption.value());

	shared_ptr<ZLExecutionData> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url, certificate(), xmlReader
		);
	networkData->setHandler(this, &LitResAuthenticationManager::onAuthorised);
	networkData->addUserData("scope", scope);
	ZLNetworkManager::Instance().perform(networkData);
	return AuthenticationStatus(std::string());
}

std::string LitResAuthenticationManager::authorise(const std::string &pwd, shared_ptr<ZLExecutionData::Listener> listener) {
	LitResAuthorisationScope *scope = new LitResAuthorisationScope;
	scope->listener = listener;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(scope->firstName, scope->lastName, scope->newSid);

	std::string url = Link.url(NetworkLink::URL_SIGN_IN);
	ZLNetworkUtil::appendParameter(url, "login", UserNameOption.value());
	ZLNetworkUtil::appendParameter(url, "pwd", pwd);
	if (SkipIPOption.value()) {
		ZLNetworkUtil::appendParameter(url, "skip_ip", "1");
	}

	shared_ptr<ZLExecutionData> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url,
			certificate(),
			xmlReader
		);
	networkData->setHandler(this, &LitResAuthenticationManager::onAuthorised);
	networkData->addUserData("scope", scope);
	return ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onAuthorised(ZLUserDataHolder &data, const std::string &error) {
	LitResAuthorisationScope &scope = static_cast<LitResAuthorisationScope&>(*data.getUserData("scope"));
	mySidChecked = true;
	if (!error.empty()) {
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
	} else {
		mySidOption.setValue(scope.newSid);
		mySidUserNameOption.setValue(UserNameOption.value());
	}
	
	scope.listener->finished(error);
}

void LitResAuthenticationManager::logOut(shared_ptr<ZLExecutionData::Listener> listener) {
	mySidChecked = true;
	mySidUserNameOption.setValue("");
	mySidOption.setValue("");
	listener->finished(std::string());
}

const std::string &LitResAuthenticationManager::currentUserName() {
	return mySidUserNameOption.value();
}

bool LitResAuthenticationManager::needPurchase(const NetworkBookItem &book) {
	return myPurchasedBooksIds.count(book.Id) == 0;
}

class LitResPurchaseBookScope : public ZLUserData {
public:
	std::string account;
	std::string bookId;
	const NetworkBookItem *book;
	shared_ptr<ZLExecutionData::Listener> listener;
};

std::string LitResAuthenticationManager::purchaseBook(const NetworkBookItem &book, shared_ptr<ZLExecutionData::Listener> listener) {
	const std::string &sid = mySidOption.value();
	std::string error;
	if (sid.empty()) {
		error = NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
		listener->finished(error);
		return error;
	}

	shared_ptr<BookReference> reference = book.reference(BookReference::BUY);
	if (reference.isNull()) {
		// TODO: add correct error message
		error = "Oh, that's impossible";
		listener->finished(error);
		return error;
	}
	std::string query = reference->URL;
	ZLNetworkUtil::appendParameter(query, "sid", sid);

	LitResPurchaseBookScope *scope = new LitResPurchaseBookScope;
	scope->book = &book;
	scope->listener = listener;
	shared_ptr<ZLXMLReader> xmlReader = new LitResPurchaseDataParser(scope->account, scope->bookId);

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		query, certificate(), xmlReader
	);
	networkData->addUserData("scope", scope);
	networkData->setHandler(this, &LitResAuthenticationManager::onBookPurchased);
	return ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onBookPurchased(ZLUserDataHolder &data, const std::string &error) {
	LitResPurchaseBookScope &scope = static_cast<LitResPurchaseBookScope&>(*data.getUserData("scope"));
	shared_ptr<ZLExecutionData::Listener> listener = scope.listener;
	if (!scope.account.empty()) {
		myAccount = BuyBookReference::price(scope.account, "RUB");
	}
	if (error == NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
	}
	const std::string alreadyPurchasedError = NetworkErrors::errorMessage(NetworkErrors::ERROR_PURCHASE_ALREADY_PURCHASED);
	if (error != alreadyPurchasedError) {
		if (!error.empty()) {
			listener->finished(error);
			return;
		}
		if (scope.bookId != scope.book->Id) {
			listener->finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_SOMETHING_WRONG, Link.SiteName));
			return;
		}
	}
	myPurchasedBooksIds.insert(scope.book->Id);
	myPurchasedBooksList.push_back(new NetworkBookItem(*scope.book, 0));
	listener->finished(error);
}

shared_ptr<BookReference> LitResAuthenticationManager::downloadReference(const NetworkBookItem &book) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return 0;
	}
	shared_ptr<BookReference> reference =
		book.reference(BookReference::DOWNLOAD_FULL_CONDITIONAL);
	if (reference.isNull()) {
		return 0;
	}
	std::string url = reference->URL;
	ZLNetworkUtil::appendParameter(url, "sid", sid);
	return new DecoratedBookReference(*reference, url);
}

void LitResAuthenticationManager::collectPurchasedBooks(NetworkItem::List &list) {
	list.assign(myPurchasedBooksList.begin(), myPurchasedBooksList.end());
}

std::string LitResAuthenticationManager::refillAccountLink() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return std::string();
	}
	std::string url = Link.url(NetworkLink::URL_REFILL_ACCOUNT);
	ZLNetworkUtil::appendParameter(url, "sid", sid);
	return url;
}

std::string LitResAuthenticationManager::currentAccount() {
	return myAccount;
}



bool LitResAuthenticationManager::needsInitialization() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return false;
	}
	return sid != myInitializedDataSid;
}

std::string LitResAuthenticationManager::initialize(shared_ptr<ZLExecutionData::Listener> listener) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		listener->finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED));
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	if (sid == myInitializedDataSid) {
		listener->finished(std::string());
		return std::string();
	}
	
	shared_ptr<ZLExecutionData> data = loadPurchasedBooks(myPurchasedBooksIds, myPurchasedBooksList);
	data->setHandler(this, &LitResAuthenticationManager::onBooksLoaded);
	data->addUserData("listener", listener.staticCast<ZLUserData>());
	return ZLNetworkManager::Instance().perform(data);
}

class LitResInitializationScope : public ZLUserData {
public:
	std::string dummy;
	std::string error;
	shared_ptr<ZLExecutionData::Listener> listener;
};

void LitResAuthenticationManager::onBooksLoaded(ZLUserDataHolder &data, const std::string &error) {
	LitResInitializationScope *scope = new LitResInitializationScope;
	scope->error = error;
	scope->listener = data.getUserData("listener").staticCast<ZLExecutionData::Listener>();
	shared_ptr<ZLExecutionData> networkData = loadAccount(scope->dummy);
	networkData->setHandler(this, &LitResAuthenticationManager::onAccountReceived);
	networkData->addUserData("scope", scope);
	ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onAccountReceived(ZLUserDataHolder &data, const std::string &error) {
	LitResInitializationScope &scope = static_cast<LitResInitializationScope&>(*data.getUserData("scope"));
	if (!error.empty() && !scope.error.empty()) {
		scope.error.append("\n").append(error);
	} else if (!error.empty()) {
		scope.error = error;
	}
	if (!scope.error.empty()) {
		myInitializedDataSid.clear();
		loadPurchasedBooksOnError(myPurchasedBooksIds, myPurchasedBooksList);
		loadAccountOnError();
		scope.listener->finished(scope.error);
		return;
	}
	myInitializedDataSid = mySidOption.value();
	loadPurchasedBooksOnSuccess(myPurchasedBooksIds, myPurchasedBooksList);
	loadAccountOnSuccess();
	scope.listener->finished(std::string());
}

shared_ptr<ZLExecutionData> LitResAuthenticationManager::loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	const std::string &sid = mySidOption.value();
	purchasedBooksIds.clear();
	purchasedBooksList.clear();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "my", "1");
	ZLNetworkUtil::appendParameter(query, "sid", sid);

	return ZLNetworkManager::Instance().createXMLParserRequest(
		LitResUtil::url(Link, "pages/catalit_browser/" + query), 
		certificate(),
		new LitResDataParser(Link, purchasedBooksList)
	);
}

void LitResAuthenticationManager::loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	purchasedBooksIds.clear();
	purchasedBooksList.clear();
}

void LitResAuthenticationManager::loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	for (NetworkItem::List::iterator it = purchasedBooksList.begin(); it != purchasedBooksList.end(); ++it) {
		NetworkBookItem &book = (NetworkBookItem&)**it;
		book.Index = 0;
		purchasedBooksIds.insert(book.Id);
	}
}

shared_ptr<ZLExecutionData> LitResAuthenticationManager::loadAccount(std::string &dummy1) {
	const std::string &sid = mySidOption.value();

	myAccount.clear();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "sid", sid);
	ZLNetworkUtil::appendParameter(query, "art", "0");

	return ZLNetworkManager::Instance().createXMLParserRequest(
		LitResUtil::url(Link, "pages/purchase_book/" + query),
		certificate(),
		new LitResPurchaseDataParser(myAccount, dummy1)
	);
}

void LitResAuthenticationManager::loadAccountOnError() {
	myAccount.clear();
}

void LitResAuthenticationManager::loadAccountOnSuccess() {
	myAccount = BuyBookReference::price(myAccount, "RUB");
}

bool LitResAuthenticationManager::skipIPSupported() {
	return true;
}

bool LitResAuthenticationManager::registrationSupported() {
	return true;
}

class LitResUserRegisterScope : public ZLUserData {
public:
	std::string newSid;
	std::string login;
	shared_ptr<ZLExecutionData::Listener> listener;
};

std::string LitResAuthenticationManager::registerUser(const std::string &login, const std::string &password, const std::string &email, shared_ptr<ZLExecutionData::Listener> listener) {
	LitResUserRegisterScope *scope = new LitResUserRegisterScope;
	scope->login = login;
	scope->listener = listener;
	shared_ptr<ZLXMLReader> xmlReader = new LitResRegisterUserDataParser(scope->newSid);

	std::string url = Link.url(NetworkLink::URL_SIGN_UP);
	ZLNetworkUtil::appendParameter(url, "new_login", login);
	ZLNetworkUtil::appendParameter(url, "new_pwd1", password);
	ZLNetworkUtil::appendParameter(url, "mail", email);

	shared_ptr<ZLExecutionData> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url, certificate(), xmlReader
		);
	networkData->addUserData("scope", scope);
	networkData->setHandler(this, &LitResAuthenticationManager::onUserRegistered);
	return ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onUserRegistered(ZLUserDataHolder &data, const std::string &error) {
	LitResUserRegisterScope &scope = static_cast<LitResUserRegisterScope&>(*data.getUserData("scope"));
	mySidChecked = true;
	if (!error.empty()) {
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		scope.listener->finished(error);
		return;
	}
	mySidOption.setValue(scope.newSid);
	mySidUserNameOption.setValue(scope.login);
	scope.listener->finished(std::string());
}

bool LitResAuthenticationManager::passwordRecoverySupported() {
	return true;
}

std::string LitResAuthenticationManager::recoverPassword(const std::string &email, shared_ptr<ZLExecutionData::Listener> listener) {
	std::string url = Link.url(NetworkLink::URL_RECOVER_PASSWORD);
	ZLNetworkUtil::appendParameter(url, "mail", email);

	shared_ptr<ZLExecutionData> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url, certificate(), new LitResPasswordRecoveryDataParser()
		);
	networkData->setHandler(this, &LitResAuthenticationManager::onPasswordRecovered);
	networkData->addUserData("listener", listener.staticCast<ZLUserData>());
	return ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onPasswordRecovered(ZLUserDataHolder &data, const std::string &error) {
	data.getUserData("listener").staticCast<ZLExecutionData::Listener>()->finished(error);
}

class LitResReloadPurchasedBooksScope : public ZLUserData {
public:
	std::set<std::string> purchasedBooksIds;
	NetworkItem::List purchasedBooksList;
};

std::string LitResAuthenticationManager::reloadPurchasedBooks(shared_ptr<ZLExecutionData::Listener> listener) {
	const std::string &sid = mySidOption.value();
	std::string error;
	if (sid.empty()) {
		error = NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
		listener->finished(error);
		return error;
	}
	if (sid != myInitializedDataSid) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		error = NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
		listener->finished(error);
		return error;
	}

	LitResReloadPurchasedBooksScope *scope = new LitResReloadPurchasedBooksScope;
	shared_ptr<ZLExecutionData> networkData = loadPurchasedBooks(scope->purchasedBooksIds, scope->purchasedBooksList);
	networkData->addUserData("listener", listener.staticCast<ZLUserData>());
	networkData->addUserData("scope", scope);
	networkData->setHandler(this, &LitResAuthenticationManager::onBooksReloaded);

	return ZLNetworkManager::Instance().perform(networkData);
}

void LitResAuthenticationManager::onBooksReloaded(ZLUserDataHolder &data, const std::string &error) {
	shared_ptr<ZLExecutionData::Listener> listener = data.getUserData("listener").staticCast<ZLExecutionData::Listener>();
	LitResReloadPurchasedBooksScope &scope = static_cast<LitResReloadPurchasedBooksScope&>(*data.getUserData("scope"));
	if (!error.empty()) {
		//loadPurchasedBooksOnError(purchasedBooksIds, purchasedBooksList);
		if (error == NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			mySidChecked = true;
			mySidUserNameOption.setValue("");
			mySidOption.setValue("");
		}
		listener->finished(error);
		return;
	}
	loadPurchasedBooksOnSuccess(scope.purchasedBooksIds, scope.purchasedBooksList);
	myPurchasedBooksIds = scope.purchasedBooksIds;
	myPurchasedBooksList = scope.purchasedBooksList;
	listener->finished(std::string());
}
