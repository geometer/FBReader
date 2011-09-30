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

#ifndef __LITRESAUTHENTICATIONMANAGER_H__
#define __LITRESAUTHENTICATIONMANAGER_H__

#include <set>

#include <ZLNetworkSSLCertificate.h>

#include "../NetworkAuthenticationManager.h"
#include "../../NetworkItems.h"

class LitResAuthenticationDataParser;

class LitResAuthenticationManager : public NetworkAuthenticationManager {

public:
	LitResAuthenticationManager(const NetworkLink &link);

public:
	AuthenticationStatus isAuthorised(bool useNetwork = true, shared_ptr<ZLExecutionData::Listener> listener = 0);
	std::string authorise(const std::string &pwd, shared_ptr<ZLExecutionData::Listener> listener);
	void logOut(shared_ptr<ZLExecutionData::Listener> listener);
	bool skipIPSupported();

	const std::string &currentUserName();
	bool needsInitialization();
	std::string initialize(shared_ptr<ZLExecutionData::Listener> listener);
	bool needPurchase(const NetworkBookItem &book);
	std::string purchaseBook(const NetworkBookItem &book, shared_ptr<ZLExecutionData::Listener> listener);
	shared_ptr<BookReference> downloadReference(const NetworkBookItem &book);

	std::string refillAccountLink(shared_ptr<ZLExecutionData::Listener> listener);
	std::string currentAccount();

	std::string reloadPurchasedBooks();
	void collectPurchasedBooks(NetworkItem::List &list);

private:
	shared_ptr<ZLExecutionData> loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);
	void loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);
	void loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkItem::List &purchasedBooksList);

	shared_ptr<ZLExecutionData> loadAccount(std::string &dummy1);
	void loadAccountOnError();
	void loadAccountOnSuccess();
	
	void onRequestFinished(LitResAuthenticationDataParser *data);

	const ZLNetworkSSLCertificate &certificate();

public: // new User Registration
	bool registrationSupported();
	std::string registerUser(const std::string &login, const std::string &password, const std::string &email, shared_ptr<ZLExecutionData::Listener> listener);

public: // Password Recovery
	bool passwordRecoverySupported();
	std::string recoverPassword(const std::string &email, shared_ptr<ZLExecutionData::Listener> listener);

private:
	bool mySidChecked;

	ZLStringOption mySidUserNameOption;
	ZLStringOption mySidOption;

	std::string myInitializedDataSid;
	std::set<std::string> myPurchasedBooksIds;
	NetworkItem::List myPurchasedBooksList;
	std::string myAccount;

	ZLNetworkSSLCertificate myCertificate;
	
	friend class LitResAuthenticationDataParser;
};

#endif /* __LITRESAUTHENTICATIONMANAGER_H__ */
