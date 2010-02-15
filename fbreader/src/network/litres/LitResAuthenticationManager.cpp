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

#include "LitResAuthenticationManager.h"

#include "LitResDataParser.h"
#include "LitResAuthenticationDataParser.h"

#include "LitResLink.h"

#include "../NetworkErrors.h"
#include "../NetworkItems.h"

#include "../NetworkLink.h"
#include "../NetworkLinkCollection.h"


LitResAuthenticationManager::LitResAuthenticationManager(const LitResLink &link) :
	NetworkAuthenticationManager(link.SiteName), 
	myLink(link),
	mySidChecked(false), 
	mySidUserNameOption(ZLCategoryKey::NETWORK, link.SiteName, "sidUserName", ""), 
	mySidOption(ZLCategoryKey::NETWORK, link.SiteName, "sid", ""),
	myCertificate(NetworkLink::CertificatesPathPrefix() + "litres.ru.crt") {
}

const ZLNetworkSSLCertificate &LitResAuthenticationManager::certificate() {
	return myCertificate;
}

NetworkAuthenticationManager::AuthenticationStatus LitResAuthenticationManager::isAuthorised(bool useNetwork) {
	bool authState = !mySidUserNameOption.value().empty() && !mySidOption.value().empty();
	if (mySidChecked || !useNetwork) {
		return AuthenticationStatus(authState);
	}

	if (!authState) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return AuthenticationStatus(false);
	}

	std::string firstName, lastName, newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	std::string query;
	ZLNetworkUtil::appendParameter(query, "sid", mySidOption.value());

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/catalit_authorise/" + query),
		certificate(),
		xmlReader
	);
	std::string error = ZLNetworkManager::Instance().perform(networkData);

	if (!error.empty()) {
		if (error != NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			return AuthenticationStatus(error);
		}
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return AuthenticationStatus(false);
	}
	mySidChecked = true;
	mySidOption.setValue(newSid);
	return AuthenticationStatus(true);
}

std::string LitResAuthenticationManager::authorise(const std::string &pwd) {
	std::string firstName, lastName, newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	std::string query;
	ZLNetworkUtil::appendParameter(query, "login", UserNameOption.value());
	ZLNetworkUtil::appendParameter(query, "pwd", pwd);
	if (SkipIPOption.value()) {
		ZLNetworkUtil::appendParameter(query, "skip_ip", "1");
	}

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/catalit_authorise/" + query),
		certificate(),
		xmlReader
	);
	std::string error = ZLNetworkManager::Instance().perform(networkData);

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
	mySidChecked = true;
	mySidUserNameOption.setValue("");
	mySidOption.setValue("");
}

std::string LitResAuthenticationManager::networkBookId(const NetworkBookItem &book) {
	return "http://robot.litres.ru/pages/catalit_download_book/?art=" + book.Id;
}

NetworkItem::URLType LitResAuthenticationManager::downloadLinkType(const NetworkBookItem &) {
	return NetworkItem::URL_BOOK_FB2_ZIP;
}

const std::string &LitResAuthenticationManager::currentUserName() {
	return mySidUserNameOption.value();
}

bool LitResAuthenticationManager::needPurchase(const NetworkBookItem &book) {
	return myPurchasedBooksIds.count(book.Id) == 0;
}

std::string LitResAuthenticationManager::purchaseBook(NetworkBookItem &book) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}

	std::string account, bookId;
	shared_ptr<ZLXMLReader> xmlReader = new LitResPurchaseDataParser(account, bookId);

	std::string query;
	ZLNetworkUtil::appendParameter(query, "sid", sid);
	ZLNetworkUtil::appendParameter(query, "art", book.Id);

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/purchase_book/" + query),
		certificate(),
		xmlReader
	);
	std::string error = ZLNetworkManager::Instance().perform(networkData);

	if (!account.empty()) {
		myAccount = account;
		myAccount.append(LitResLink::CURRENCY_SUFFIX);
	}
	if (error == NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
	}
	const std::string alreadyPurchasedError = NetworkErrors::errorMessage(NetworkErrors::ERROR_PURCHASE_ALREADY_PURCHASED);
	if (error != alreadyPurchasedError) {
		if (!error.empty()) {
			return error;
		}
		if (bookId != book.Id) {
			return NetworkErrors::errorMessage(NetworkErrors::ERROR_SOMETHING_WRONG, SiteName);
		}
	}
	myPurchasedBooksIds.insert(book.Id);
	NetworkBookItem *bookCopy = new NetworkBookItem(book);
	bookCopy->setIndex(0);
	myPurchasedBooksList.push_back(bookCopy);
	return error;
}

std::string LitResAuthenticationManager::downloadLink(const NetworkBookItem &book) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return "";
	}
	std::string query;
	ZLNetworkUtil::appendParameter(query, "sid", sid);
	ZLNetworkUtil::appendParameter(query, "art", book.Id);
	return myLink.litresUrl("pages/catalit_download_book/" + query);
}

void LitResAuthenticationManager::collectPurchasedBooks(NetworkItem::List &list) {
	list.assign(myPurchasedBooksList.begin(), myPurchasedBooksList.end());
}

std::string LitResAuthenticationManager::refillAccountLink() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return "";
	}
	std::string url =
		"https://www.litres.ru/pages/put_money_on_account/?sid=" + sid;
	myLink.rewriteUrl(url);
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

std::string LitResAuthenticationManager::initialize() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	if (sid == myInitializedDataSid) {
		return "";
	}

	std::string dummy1;

	ZLExecutionData::Vector dataList;
	dataList.push_back(loadPurchasedBooks(myPurchasedBooksIds, myPurchasedBooksList));
	dataList.push_back(loadAccount(dummy1));

	std::string error = ZLNetworkManager::Instance().perform(dataList);
	if (!error.empty()) {
		myInitializedDataSid.clear();
		loadPurchasedBooksOnError(myPurchasedBooksIds, myPurchasedBooksList);
		loadAccountOnError();
		return error;
	}
	myInitializedDataSid = sid;
	loadPurchasedBooksOnSuccess(myPurchasedBooksIds, myPurchasedBooksList);
	loadAccountOnSuccess();
	return "";
}

shared_ptr<ZLExecutionData> LitResAuthenticationManager::loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	const std::string &sid = mySidOption.value();
	purchasedBooksIds.clear();
	purchasedBooksList.clear();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "my", "1");
	ZLNetworkUtil::appendParameter(query, "sid", sid);

	shared_ptr<NetworkAuthenticationManager> mgr;
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();
	for (size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
		if (link.SiteName == this->SiteName) {
			mgr = link.authenticationManager();
			break;
		}
	}

	return ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/catalit_browser/" + query), 
		certificate(),
		new LitResDataParser(myLink, purchasedBooksList)
	);
}

void LitResAuthenticationManager::loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	purchasedBooksIds.clear();
	purchasedBooksList.clear();
}

void LitResAuthenticationManager::loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList) {
	for (NetworkItem::List::iterator it = purchasedBooksList.begin(); it != purchasedBooksList.end(); ++it) {
		NetworkBookItem &book = (NetworkBookItem&)**it;
		book.setIndex(0);
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
		myLink.litresUrl("pages/purchase_book/" + query),
		certificate(),
		new LitResPurchaseDataParser(myAccount, dummy1)
	);
}

void LitResAuthenticationManager::loadAccountOnError() {
	myAccount.clear();
}

void LitResAuthenticationManager::loadAccountOnSuccess() {
	if (!myAccount.empty()) {
		myAccount.append(LitResLink::CURRENCY_SUFFIX);
	}
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

	std::string query;
	ZLNetworkUtil::appendParameter(query, "new_login", login);
	ZLNetworkUtil::appendParameter(query, "new_pwd1", password);
	ZLNetworkUtil::appendParameter(query, "mail", email);

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/catalit_register_user/" + query),
		certificate(),
		xmlReader
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
	std::string newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResPasswordRecoveryDataParser();

	std::string query;
	ZLNetworkUtil::appendParameter(query, "mail", email);

	shared_ptr<ZLExecutionData> networkData = ZLNetworkManager::Instance().createXMLParserRequest(
		myLink.litresUrl("pages/catalit_recover_pass/" + query),
		certificate(),
		xmlReader
	);
	return ZLNetworkManager::Instance().perform(networkData);
}

std::string LitResAuthenticationManager::reloadPurchasedBooks() {
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

	shared_ptr<ZLExecutionData> networkData = loadPurchasedBooks(purchasedBooksIds, purchasedBooksList);

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
