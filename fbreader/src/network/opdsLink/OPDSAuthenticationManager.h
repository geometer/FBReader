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

#ifndef __OPDSAUTHENTICATIONMANAGER_H__
#define __OPDSAUTHENTICATIONMANAGER_H__

#include "../NetworkAuthenticationManager.h"


/*
 * When signing in and/or trying to access account page there can be following answers:
 *   - HTTP/1.1 200 OK
 *     Content can be used to qualify the result
 *
 *   - HTTP/1.1 302 Found
 *     "Location:" can be used to qualify the result
 *
 *   - HTTP/1.1 4xx
 *     for example: HTTP/1.1 401 Unauthorized
 */

class OPDSAuthenticationManager : public NetworkAuthenticationManager {

public:
	OPDSAuthenticationManager(
		const std::string &siteName,
		const std::string &postLogin,
		const std::string &postPassword,
		const std::string &postSignInUrl,
		const std::string &signOutUrl,
		const std::string &accountUrl
	);

public:
	AuthenticationStatus isAuthorised(bool useNetwork = true);
	std::string authorise(const std::string &pwd); // returns error message
	void logOut();

	std::string networkBookId(const NetworkLibraryBookItem &book);
	NetworkLibraryBookItem::URLType downloadLinkType(const NetworkLibraryBookItem &book);

public: // Account specific methods (can be called only if authorised!!!)
	const std::string &currentUserName();
	/*bool needsInitialization();
	std::string initialize(); // returns error message
	bool needPurchase(const NetworkLibraryBookItem &book); // returns true if link must be purchased before downloading
	std::string purchaseBook(NetworkLibraryBookItem &book); // returns error message
	std::string downloadLink(const NetworkLibraryBookItem &book); // returns link to download book*/

	//std::string refillAccountLink();
	//std::string currentAccount();

	//const std::string &certificate();

private: // config data
	std::string myPostLogin;
	std::string myPostPassword;
	std::string myPostSignInUrl;
	std::string mySignOutUrl;
	std::string myAccountUrl;

private:
	bool myAccountChecked;

	ZLStringOption myAccountUserNameOption;
};

#endif /* __OPDSAUTHENTICATIONMANAGER_H__ */
