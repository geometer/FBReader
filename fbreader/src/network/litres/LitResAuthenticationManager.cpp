/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#include "LitResUtil.h"

#include "../NetworkErrors.h"
#include "../NetworkLibraryItems.h"

#include "../NetworkLink.h"
#include "../NetworkLinkCollection.h"


LitResAuthenticationManager::LitResAuthenticationManager(const std::string &siteName) :
	NetworkAuthenticationManager(siteName), 
	mySidChecked(false), 
	mySidUserNameOption(ZLCategoryKey::NETWORK, siteName, "sidUserName", ""), 
	mySidOption(ZLCategoryKey::NETWORK, siteName, "sid", "") {
}

const std::string &LitResAuthenticationManager::certificate() {
	if (myCertificate.empty()) {
		myCertificate = NetworkLink::CertificatesPathPrefix() + "litres.ru.crt";
	}
	return myCertificate;
}

ZLBoolean3 LitResAuthenticationManager::isAuthorised(bool useNetwork) {
	bool authState = !mySidUserNameOption.value().empty() && !mySidOption.value().empty();
	if (mySidChecked || !useNetwork) {
		return authState ? B3_TRUE : B3_FALSE;
	}

	if (!authState) {
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return B3_FALSE;
	}

	std::string firstName, lastName, newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	std::string query;
	ZLNetworkUtil::addParameter(query, "sid", mySidOption.value());

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_authorise/" + query),
		certificate(),
		xmlReader
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	if (!error.empty()) {
		if (error != NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED)) {
			return B3_UNDEFINED;
		}
		mySidChecked = true;
		mySidUserNameOption.setValue("");
		mySidOption.setValue("");
		return B3_FALSE;
	}
	mySidChecked = true;
	mySidOption.setValue(newSid);
	return B3_TRUE;
}

std::string LitResAuthenticationManager::authorise(const std::string &pwd) {
	std::string firstName, lastName, newSid;
	shared_ptr<ZLXMLReader> xmlReader = new LitResLoginDataParser(firstName, lastName, newSid);

	std::string query;
	ZLNetworkUtil::addParameter(query, "login", UserNameOption.value());
	ZLNetworkUtil::addParameter(query, "pwd", pwd);
	if (SkipIPOption.value()) {
		ZLNetworkUtil::addParameter(query, "skip_ip", "1");
	}

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_authorise/" + query),
		certificate(),
		xmlReader
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

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

std::string LitResAuthenticationManager::networkBookId(const NetworkLibraryBookItem &book) {
	return "http://robot.litres.ru/pages/catalit_download_book/?art=" + book.id();
}

NetworkLibraryBookItem::URLType LitResAuthenticationManager::downloadLinkType(const NetworkLibraryBookItem &) {
	return NetworkLibraryBookItem::BOOK_FB2_ZIP;
}

const std::string &LitResAuthenticationManager::currentUserName() {
	return mySidUserNameOption.value();
}

bool LitResAuthenticationManager::needPurchase(const NetworkLibraryBookItem &book) {
	return myPurchasedBooksIds.count(book.id()) == 0;
}

std::string LitResAuthenticationManager::purchaseBook(NetworkLibraryBookItem &book) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}

	std::string account, bookId;
	shared_ptr<ZLXMLReader> xmlReader = new LitResPurchaseDataParser(account, bookId);

	std::string query;
	ZLNetworkUtil::addParameter(query, "sid", sid);
	ZLNetworkUtil::addParameter(query, "art", book.id());

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/purchase_book/" + query),
		certificate(),
		xmlReader
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

	if (!account.empty()) {
		myAccount = account;
		myAccount.append(LitResUtil::CURRENCY_SUFFIX);
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
		if (bookId != book.id()) {
			return NetworkErrors::errorMessage(NetworkErrors::ERROR_SOMETHING_WRONG, SiteName);
		}
	}
	myPurchasedBooksIds.insert(book.id());
	NetworkLibraryBookItem *bookCopy = new NetworkLibraryBookItem(book);
	bookCopy->setIndex(0);
	myPurchasedBooksList.push_back(bookCopy);
	return error;
}

std::string LitResAuthenticationManager::downloadLink(const NetworkLibraryBookItem &book) {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return "";
	}
	std::string query;
	ZLNetworkUtil::addParameter(query, "sid", sid);
	ZLNetworkUtil::addParameter(query, "art", book.id());
	return LitResUtil::litresLink("pages/catalit_download_book/" + query);
}

void LitResAuthenticationManager::collectPurchasedBooks(NetworkLibraryItemList &list) {
	list.assign(myPurchasedBooksList.begin(), myPurchasedBooksList.end());
}

std::string LitResAuthenticationManager::refillAccountLink() {
	const std::string &sid = mySidOption.value();
	if (sid.empty()) {
		return "";
	}
	return LitResUtil::appendLFrom("https://www.litres.ru/pages/put_money_on_account/?sid=" + sid);
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

shared_ptr<ZLExecutionData> LitResAuthenticationManager::loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList) {
	const std::string &sid = mySidOption.value();
	purchasedBooksIds.clear();
	purchasedBooksList.clear();

	std::string query;
	ZLNetworkUtil::addParameter(query, "my", "1");
	ZLNetworkUtil::addParameter(query, "sid", sid);

	shared_ptr<NetworkAuthenticationManager> mgr;
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();
	for (size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
		if (link.SiteName == this->SiteName) {
			mgr = link.authenticationManager();
			break;
		}
	}

	return ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_browser/" + query), 
		certificate(),
		new LitResDataParser(purchasedBooksList, mgr)
	);
}

void LitResAuthenticationManager::loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList) {
	purchasedBooksIds.clear();
	purchasedBooksList.clear();
}

void LitResAuthenticationManager::loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList) {
	for (NetworkLibraryItemList::iterator it = purchasedBooksList.begin(); it != purchasedBooksList.end(); ++it) {
		NetworkLibraryBookItem &book = (NetworkLibraryBookItem &) **it;
		book.setIndex(0);
		purchasedBooksIds.insert(book.id());
	}
}

shared_ptr<ZLExecutionData> LitResAuthenticationManager::loadAccount(std::string &dummy1) {
	const std::string &sid = mySidOption.value();

	myAccount.clear();

	std::string query;
	ZLNetworkUtil::addParameter(query, "sid", sid);
	ZLNetworkUtil::addParameter(query, "art", "0");

	return ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/purchase_book/" + query),
		certificate(),
		new LitResPurchaseDataParser(myAccount, dummy1)
	);
}

void LitResAuthenticationManager::loadAccountOnError() {
	myAccount.clear();
}

void LitResAuthenticationManager::loadAccountOnSuccess() {
	if (!myAccount.empty()) {
		myAccount.append(LitResUtil::CURRENCY_SUFFIX);
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
	ZLNetworkUtil::addParameter(query, "new_login", login);
	ZLNetworkUtil::addParameter(query, "new_pwd1", password);
	ZLNetworkUtil::addParameter(query, "mail", email);

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_register_user/" + query),
		certificate(),
		xmlReader
	));
	std::string error = ZLNetworkManager::Instance().perform(dataList);

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
	ZLNetworkUtil::addParameter(query, "mail", email);

	ZLExecutionData::Vector dataList;
	dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
		LitResUtil::litresLink("pages/catalit_recover_pass/" + query),
		certificate(),
		xmlReader
	));
	return ZLNetworkManager::Instance().perform(dataList);
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
	NetworkLibraryItemList purchasedBooksList;

	ZLExecutionData::Vector dataList;
	dataList.push_back(loadPurchasedBooks(purchasedBooksIds, purchasedBooksList));

	std::string error = ZLNetworkManager::Instance().perform(dataList);
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
