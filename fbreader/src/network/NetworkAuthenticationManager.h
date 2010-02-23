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

#ifndef __NETWORKAUTHENTICATIONMANAGER_H__
#define __NETWORKAUTHENTICATIONMANAGER_H__

#include <shared_ptr.h>
#include <ZLOptions.h>
#include <ZLBoolean3.h>

#include "NetworkItems.h"

struct ZLNetworkSSLCertificate;

class NetworkAuthenticationManager {

public:
	NetworkAuthenticationManager(const std::string &siteName);
	virtual ~NetworkAuthenticationManager();

public:
	const std::string SiteName;
	ZLStringOption UserNameOption;
	ZLBooleanOption SkipIPOption;

public:
	struct AuthenticationStatus {
		const ZLBoolean3 Status;
		const std::string Message;

		AuthenticationStatus(bool status);
		AuthenticationStatus(const std::string &msg);
	};

	virtual AuthenticationStatus isAuthorised(bool useNetwork = true) = 0;
	virtual std::string authorise(const std::string &pwd) = 0; // returns error message
	virtual void logOut() = 0;

	virtual bool skipIPSupported();

	/* 
	 * Returns analog for downloadLink(book), but this link doesn't depend on account;
	 * This value is used in calls for BooksDB's Network database.
	 */
	virtual std::string networkBookId(const NetworkBookItem &book); 

	virtual NetworkItem::URLType downloadLinkType(const NetworkBookItem &book); // returns type of download link

public: // Account specific methods (can be called only if authorised!!!)
	virtual const std::string &currentUserName() = 0;
	virtual bool needsInitialization();
	virtual std::string initialize(); // returns error message
	virtual bool needPurchase(const NetworkBookItem &book); // returns true if link must be purchased before downloading
	virtual std::string purchaseBook(NetworkBookItem &book); // returns error message
	virtual std::string downloadLink(const NetworkBookItem &book); // returns link to download book

	virtual std::string refillAccountLink();
	virtual std::string currentAccount();

	virtual const ZLNetworkSSLCertificate &certificate() = 0;

public: // new User Registration
	virtual bool registrationSupported();
	virtual std::string registerUser(const std::string &login, const std::string &password, const std::string &email);

public: // Password Recovery
	virtual bool passwordRecoverySupported();
	virtual std::string recoverPassword(const std::string &email);

private: // disable copying
	NetworkAuthenticationManager(const NetworkAuthenticationManager &);
	const NetworkAuthenticationManager &operator = (const NetworkAuthenticationManager &);
};

inline NetworkAuthenticationManager::AuthenticationStatus::AuthenticationStatus(bool status) : Status(status ? B3_TRUE : B3_FALSE) {}
inline NetworkAuthenticationManager::AuthenticationStatus::AuthenticationStatus(const std::string &msg) : Status(B3_UNDEFINED), Message(msg) {}

#endif /* __NETWORKAUTHENTICATIONMANAGER_H__ */
