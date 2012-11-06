/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLDialogManager.h>
#include <ZLProgressDialog.h>
#include <ZLNetworkManager.h>

#include "NetworkOperationRunnable.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

NetworkOperationRunnable::NetworkOperationRunnable(const std::string &uiMessageKey) {
	myDialog =
		ZLDialogManager::Instance().createProgressDialog(ZLResourceKey(uiMessageKey), true);
}

NetworkOperationRunnable::~NetworkOperationRunnable() {
}

void NetworkOperationRunnable::executeWithUI() {
	myDialog->run(*this);
}

bool NetworkOperationRunnable::hasErrors() const {
	return !myErrorMessage.empty();
}

void NetworkOperationRunnable::showErrorMessage(const std::string &message) {
	ZLDialogManager::Instance().errorBox(
		ZLResourceKey("networkError"),
		message
	);
}

const std::string &NetworkOperationRunnable::errorMessage() const {
	return myErrorMessage;
}

bool NetworkOperationRunnable::tryConnect() {
	if (!ZLNetworkManager::Instance().connect()) {
		showErrorMessage(
			ZLResource::resource("dialog")
				["networkError"]
				["couldntConnectToNetworkMessage"].value()
		);
		return false;
	}
	return true;
}

void NetworkOperationRunnable::showErrorMessage() const {
	if (!myErrorMessage.empty()) {
		showErrorMessage(myErrorMessage);
	}
}

DownloadBookRunnable::DownloadBookRunnable(shared_ptr<BookReference> reference, shared_ptr<NetworkAuthenticationManager> authManager) : NetworkOperationRunnable("downloadBook") {
	myReference = reference;
	myAuthManager = authManager;
}

DownloadBookRunnable::DownloadBookRunnable(const std::string &url) : NetworkOperationRunnable("downloadBook") {
	myReference = new BookReference(url, BookReference::NONE, BookReference::DOWNLOAD_FULL);
}

DownloadBookRunnable::~DownloadBookRunnable() {
}

void DownloadBookRunnable::run() {
	NetworkLinkCollection::Instance().downloadBook(
		*myReference, myFileName,
		myDialog->listener()
	);
	myErrorMessage = NetworkLinkCollection::Instance().errorMessage();
}

const std::string &DownloadBookRunnable::fileName() const {
	return myFileName;
}


IsAuthorisedRunnable::IsAuthorisedRunnable(NetworkAuthenticationManager &mgr) :
	NetworkOperationRunnable("authenticationCheck"),
	myManager(mgr),
	myResult(B3_UNDEFINED) {
}

void IsAuthorisedRunnable::run() {
	NetworkAuthenticationManager::AuthenticationStatus auth = myManager.isAuthorised(true);
	myErrorMessage = auth.Message;
	myResult = auth.Status;
}

ZLBoolean3 IsAuthorisedRunnable::result() {
	return myResult;
}


AuthoriseRunnable::AuthoriseRunnable(NetworkAuthenticationManager &mgr, const std::string &password) :
	NetworkOperationRunnable("authentication"),
	myManager(mgr),
	myPassword(password) {
}

void AuthoriseRunnable::run() {
	myErrorMessage = myManager.authorise(myPassword);
}


InitializeAuthenticationManagerRunnable::InitializeAuthenticationManagerRunnable(NetworkAuthenticationManager &mgr) :
	NetworkOperationRunnable("initializeAuthenticationManager"),
	myManager(mgr) {
}

void InitializeAuthenticationManagerRunnable::run() {
	myErrorMessage = myManager.initialize();
}


LogOutRunnable::LogOutRunnable(NetworkAuthenticationManager &mgr) :
	NetworkOperationRunnable("signOut"),
	myManager(mgr) {
}

void LogOutRunnable::run() {
	if (myManager.isAuthorised(false).Status != B3_FALSE) {
		myManager.logOut();
	}
}

PasswordRecoveryRunnable::PasswordRecoveryRunnable(NetworkAuthenticationManager &mgr, const std::string &email) :
	NetworkOperationRunnable("passwordRecovery"),
	myManager(mgr),
	myEMail(email) {
}

void PasswordRecoveryRunnable::run() {
	myErrorMessage = myManager.recoverPassword(myEMail);
}

RegisterUserRunnable::RegisterUserRunnable(NetworkAuthenticationManager &mgr, const std::string &login, const std::string &password, const std::string &email) :
	NetworkOperationRunnable("registerUser"),
	myManager(mgr),
	myLogin(login),
	myPassword(password),
	myEMail(email) {
}

void RegisterUserRunnable::run() {
	myErrorMessage = myManager.registerUser(myLogin, myPassword, myEMail);
}


SearchRunnable::SearchRunnable() : NetworkOperationRunnable("downloadBookList") {
}


SimpleSearchRunnable::SimpleSearchRunnable(const std::string &pattern) : myPattern(pattern) {
}

void SimpleSearchRunnable::run() {
	mySearchResult = NetworkLinkCollection::Instance().simpleSearch(myPattern);
	myErrorMessage = NetworkLinkCollection::Instance().errorMessage();
}


AdvancedSearchRunnable::AdvancedSearchRunnable(const std::string &titleAndSeries, const std::string &author, const std::string &category, const std::string &description) : myTitleAndSeries(titleAndSeries), myAuthor(author), myCategory(category), myDescription(description) {
}

void AdvancedSearchRunnable::run() {
	mySearchResult = NetworkLinkCollection::Instance().advancedSearch(myTitleAndSeries, myAuthor, myCategory, myDescription);
	myErrorMessage = NetworkLinkCollection::Instance().errorMessage();
}


LoadSubCatalogRunnable::LoadSubCatalogRunnable(NetworkCatalogItem &item, NetworkItem::List &children) :
	NetworkOperationRunnable("loadSubCatalog"),
	myItem(item),
	myChildren(children) {
}

void LoadSubCatalogRunnable::run() {
	myErrorMessage = myItem.loadChildren(myChildren);
}

DownloadBookRunnableAsync::DownloadBookRunnableAsync(shared_ptr<BookReference> reference, shared_ptr<NetworkAuthenticationManager> authManager) {
	myReference = reference;
	myAuthManager = authManager;
}

void DownloadBookRunnableAsync::run() {
}

void DownloadBookRunnableAsync::showPercent(int /*ready*/, int /*full*/) {}

void DownloadBookRunnableAsync::finished(const std::string &error) {
}
