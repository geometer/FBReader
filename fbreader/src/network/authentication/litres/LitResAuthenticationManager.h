/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESAUTHENTICATIONMANAGER_H__
#define __LITRESAUTHENTICATIONMANAGER_H__

#include <set>

#include <ZLNetworkRequest.h>
#include <ZLUserData.h>

#include "../NetworkAuthenticationManager.h"
#include "../../NetworkItems.h"

class LitResAuthenticationManager : public NetworkAuthenticationManager {

public:
	LitResAuthenticationManager(const NetworkLink &link);

public:
	AuthenticationStatus isAuthorised(shared_ptr<ZLNetworkRequest::Listener> listener = 0);
	std::string authorise(const std::string &pwd, shared_ptr<ZLNetworkRequest::Listener> listener);
	void logOut();
	bool skipIPSupported();

	const std::string &currentUserName();
	bool needsInitialization();
	std::string initialize(shared_ptr<ZLNetworkRequest::Listener> listener);
	bool needPurchase(const NetworkBookItem &book);
	std::string purchaseBook(const NetworkBookItem &, shared_ptr<ZLNetworkRequest::Listener> listener);
	shared_ptr<BookReference> downloadReference(const NetworkBookItem &book);

	std::string topupAccountLink();
	std::string currentAccount();

	std::string reloadPurchasedBooks(shared_ptr<ZLNetworkRequest::Listener> listener);
	void collectPurchasedBooks(NetworkItem::List &list);
	const std::set<std::string> &getPurchasedIds() const;
	const NetworkItem::List &purchasedBooks() const;

private:
	void onAuthorised(ZLUserDataHolder &data, const std::string &error);
	void onBookPurchased(ZLUserDataHolder &data, const std::string &error);
	void onBooksLoaded(ZLUserDataHolder &data, const std::string &error);
	void onAccountReceived(ZLUserDataHolder &data, const std::string &error);
	void onBooksReloaded(ZLUserDataHolder &data, const std::string &error);

private:
	shared_ptr<ZLNetworkRequest> loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);
	void loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);
	void loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);

	shared_ptr<ZLNetworkRequest> loadAccount(std::string &dummy1);
	void loadAccountOnError();
	void loadAccountOnSuccess();

public: // new User Registration
	bool registrationSupported();
	std::string registerUser(const std::string &login, const std::string &password, const std::string &email);

public: // Password Recovery
	bool passwordRecoverySupported();
	std::string recoverPassword(const std::string &email);

private:
	bool mySidChecked;

	ZLStringOption mySidUserNameOption;
	ZLStringOption mySidOption;

	std::string myInitializedDataSid;
	std::set<std::string> myPurchasedBooksIds;
	NetworkItem::List myPurchasedBooksList;
	std::string myAccount;
};

#endif /* __LITRESAUTHENTICATIONMANAGER_H__ */
