/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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
#include <ZLUserData.h>
#include <ZLExecutionUtil.h>

#include "../../litres/LitResBooksFeedParser.h"
#include "../../litres/LitResUtil.h"
#include "LitResAuthenticationManager.h"
#include "LitResAuthenticationDataParser.h"

#include "../../NetworkErrors.h"
#include "../../NetworkLink.h"
#include "../../NetworkLinkCollection.h"

LitResAuthenticationManager::LitResAuthenticationManager(const NetworkLink &link) :
	NetworkAuthenticationManager(link),
	mySidChecked(false),
	mySidUserNameOption(ZLCategoryKey::NETWORK, link.getSiteName(), "sidUserName", ""),
	mySidOption(ZLCategoryKey::NETWORK, link.getSiteName(), "sid", "")
{
}

class LitResAuthorisationScope : public ZLUserData {
public:
	std::string firstName;
	std::string lastName;
	std::string newSid;
	shared_ptr<ZLNetworkRequest::Listener> listener;
};

NetworkAuthenticationManager::AuthenticationStatus LitResAuthenticationManager::isAuthorised(shared_ptr<ZLNetworkRequest::Listener> listener) {
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

	shared_ptr<ZLNetworkRequest> networkData = ZLNetworkManager::Instance().createXMLParserRequest(url, xmlReader);
	networkData->setListener(ZLExecutionUtil::createListener(scope, this, &LitResAuthenticationManager::onAuthorised));
	ZLNetworkManager::Instance().performAsync(networkData);
	return AuthenticationStatus(std::string());
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

std::string LitResAuthenticationManager::authorise(const std::string &pwd) {
	//TODO make async
	std::string firstName, lastName, newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	std::string url = Link.url(NetworkLink::URL_SIGN_IN);
	ZLNetworkUtil::appendParameter(url, "login", UserNameOption.value());
	ZLNetworkUtil::appendParameter(url, "pwd", pwd);
//	if (SkipIPOption.value()) {
		ZLNetworkUtil::appendParameter(url, "skip_ip", "1");
//	}

	shared_ptr<ZLNetworkRequest> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url,
			xmlReader
		);
	std::string error = ZLNetworkManager::Instance().perform(networkData);

	if (error.empty() && !xmlReader->errorMessage().empty()) {
		error = xmlReader->errorMessage();
	}

	mySidChecked = true;
	if (!error.empty()) {
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return error;
	}
	mySidOption.setValue(newSid);
	mySidUserNameOption.setValue(UserNameOption.value());
	return "";
}

void LitResAuthenticationManager::logOut() {
	//TODO make async?
	mySidChecked = true;
	mySidUserNameOption.setValue("");
	mySidOption.setValue("");
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
	shared_ptr<ZLNetworkRequest::Listener> listener;
};

std::string LitResAuthenticationManager::purchaseBook(const NetworkBookItem &book, shared_ptr<ZLNetworkRequest::Listener> listener) {
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
	shared_ptr<ZLNetworkRequest> networkData = ZLNetworkManager::Instance().createXMLParserRequest(query, xmlReader);
	networkData->setListener(ZLExecutionUtil::createListener(scope, this, &LitResAuthenticationManager::onBookPurchased));
	return ZLNetworkManager::Instance().performAsync(networkData);
}

void LitResAuthenticationManager::onBookPurchased(ZLUserDataHolder &data, const std::string &error) {
	LitResPurchaseBookScope &scope = static_cast<LitResPurchaseBookScope&>(*data.getUserData("scope"));
	shared_ptr<ZLNetworkRequest::Listener> listener = scope.listener;
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
			listener->finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_SOMETHING_WRONG, Link.getSiteName()));
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

const std::set<std::string> &LitResAuthenticationManager::getPurchasedIds() const {
	return myPurchasedBooksIds;
}

std::string LitResAuthenticationManager::topupAccountLink() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return std::string();
	}
	std::string url = Link.url(NetworkLink::URL_TOPUP);
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

class LitResInitializationScope : public ZLUserData {
public:
	std::string dummy;
	std::string error;
	shared_ptr<ZLNetworkRequest::Listener> listener;
};

std::string LitResAuthenticationManager::initialize(shared_ptr<ZLNetworkRequest::Listener> listener) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		listener->finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED));
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	if (sid == myInitializedDataSid) {
		listener->finished(std::string());
		return std::string();
	}

	LitResInitializationScope *scope = new LitResInitializationScope;
	scope->listener = listener;

	shared_ptr<ZLNetworkRequest> request = loadPurchasedBooks(myPurchasedBooksIds, myPurchasedBooksList);
	request->setListener(ZLExecutionUtil::createListener(scope, this, &LitResAuthenticationManager::onBooksLoaded));
	return ZLNetworkManager::Instance().performAsync(request);
}

void LitResAuthenticationManager::onBooksLoaded(ZLUserDataHolder &data, const std::string &error) {
	LitResInitializationScope &scope = static_cast<LitResInitializationScope&>(*data.getUserData("scope"));
	scope.error = error;
	shared_ptr<ZLNetworkRequest> request = loadAccount(scope.dummy);
	request->setListener(ZLExecutionUtil::createListener(new ZLUserDataHolder(data), this, &LitResAuthenticationManager::onAccountReceived));
	ZLNetworkManager::Instance().performAsync(request);
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
	scope.listener->finished();
}

shared_ptr<ZLNetworkRequest> LitResAuthenticationManager::loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	const std::string &sid = mySidOption.value();
	purchasedBooksIds.clear();
	purchasedBooksList.clear();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "my", "1");
	ZLNetworkUtil::appendParameter(query, "sid", sid);

	return ZLNetworkManager::Instance().createXMLParserRequest(
		LitResUtil::url(Link, "pages/catalit_browser/" + query),
		new LitResBooksFeedParser(Link, purchasedBooksList)
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

shared_ptr<ZLNetworkRequest> LitResAuthenticationManager::loadAccount(std::string &dummy1) {
	const std::string &sid = mySidOption.value();

	myAccount.clear();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "sid", sid);
	ZLNetworkUtil::appendParameter(query, "art", "0");

	return ZLNetworkManager::Instance().createXMLParserRequest(
		LitResUtil::url(Link, "pages/purchase_book/" + query),
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

std::string LitResAuthenticationManager::registerUser(const std::string &login, const std::string &password, const std::string &email) {
	std::string newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResRegisterUserDataParser(newSid);

	std::string url = Link.url(NetworkLink::URL_SIGN_UP);
	ZLNetworkUtil::appendParameter(url, "new_login", login);
	ZLNetworkUtil::appendParameter(url, "new_pwd1", password);
	ZLNetworkUtil::appendParameter(url, "mail", email);

	shared_ptr<ZLNetworkRequest> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url, xmlReader
		);
	std::string error = ZLNetworkManager::Instance().perform(networkData);

	mySidChecked = true;
	if (!error.empty()) {
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return error;
	}
	mySidOption.setValue(newSid);
	mySidUserNameOption.setValue(login);
	return "";
}

bool LitResAuthenticationManager::passwordRecoverySupported() {
	return true;
}

std::string LitResAuthenticationManager::recoverPassword(const std::string &email) {
	std::string url = Link.url(NetworkLink::URL_RECOVER_PASSWORD);
	ZLNetworkUtil::appendParameter(url, "mail", email);

	shared_ptr<ZLNetworkRequest> networkData =
		ZLNetworkManager::Instance().createXMLParserRequest(
			url, new LitResPasswordRecoveryDataParser()
		);
	return ZLNetworkManager::Instance().perform(networkData);
}

std::string LitResAuthenticationManager::reloadPurchasedBooks() {
	//TODO make async
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	if (sid != myInitializedDataSid) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}

	std::set<std::string> purchasedBooksIds;
	NetworkItem::List purchasedBooksList;

	shared_ptr<ZLNetworkRequest> networkData = loadPurchasedBooks(purchasedBooksIds, purchasedBooksList);

	std::string error = ZLNetworkManager::Instance().perform(networkData);
	if (!error.empty()) {
		//loadPurchasedBooksOnError(purchasedBooksIds, purchasedBooksList);
		if (error == NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			mySidChecked = true;
			mySidUserNameOption.setValue("");
			mySidOption.setValue("");
		}
		return error;
	}
	loadPurchasedBooksOnSuccess(purchasedBooksIds, purchasedBooksList);
	myPurchasedBooksIds = purchasedBooksIds;
	myPurchasedBooksList = purchasedBooksList;
	return "";
}
