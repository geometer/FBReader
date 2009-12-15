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

#ifndef __LITRESAUTHENTICATIONMANAGER_H__
#define __LITRESAUTHENTICATIONMANAGER_H__

#include <set>

#include "../NetworkAuthenticationManager.h"
#include "../NetworkLibraryItems.h"

class LitResAuthenticationManager : public NetworkAuthenticationManager {

public:
	LitResAuthenticationManager(const std::string &siteName);

public:
	ZLBoolean3 isAuthorised(bool useNetwork = true);
	std::string authorise(const std::string &pwd);
	void logOut();
	bool skipIPSupported();

	std::string networkBookId(const NetworkLibraryBookItem &book); 
	NetworkLibraryBookItem::URLType downloadLinkType(const NetworkLibraryBookItem &book);

	const std::string &currentUserName();
	bool needsInitialization();
	std::string initialize();
	bool needPurchase(const NetworkLibraryBookItem &book);
	std::string purchaseBook(NetworkLibraryBookItem &book);
	std::string downloadLink(const NetworkLibraryBookItem &book);

	std::string refillAccountLink();
	std::string currentAccount();

	std::string reloadPurchasedBooks();
	void collectPurchasedBooks(NetworkLibraryItemList &list);

private:
	shared_ptr<ZLExecutionData> loadPurchasedBooks(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList);
	void loadPurchasedBooksOnError(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList);
	void loadPurchasedBooksOnSuccess(std::set<std::string> &purchasedBooksIds, NetworkLibraryItemList &purchasedBooksList);

	shared_ptr<ZLExecutionData> loadAccount(std::string &dummy1);
	void loadAccountOnError();
	void loadAccountOnSuccess();

	const std::string &certificate();

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
	NetworkLibraryItemList myPurchasedBooksList;
	std::string myAccount;

	std::string myCertificate;
};

#endif /* __LITRESAUTHENTICATIONMANAGER_H__ */
