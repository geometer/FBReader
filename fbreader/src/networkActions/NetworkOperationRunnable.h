/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKOPERATIONRUNNABLE_H__
#define __NETWORKOPERATIONRUNNABLE_H__

#include <string>

#include <ZLRunnable.h>
#include <ZLBoolean3.h>
#include <ZLNetworkSSLCertificate.h>

#include "../network/NetworkItems.h"
#include "../network/NetworkBookCollection.h"

class ZLProgressDialog;

class NetworkAuthenticationManager;

class NetworkOperationRunnable : public ZLRunnable {

public:
	static void showErrorMessage(const std::string &message);
	static bool tryConnect();

protected:
	NetworkOperationRunnable(const std::string &uiMessageKey);
	~NetworkOperationRunnable();

public:
	void executeWithUI();
	bool hasErrors() const;
	void showErrorMessage() const;
	const std::string &errorMessage() const;

protected:
	std::string myErrorMessage;
	shared_ptr<ZLProgressDialog> myDialog;
};

class DownloadBookRunnable : public NetworkOperationRunnable {

public:
	DownloadBookRunnable(shared_ptr<BookReference> reference, shared_ptr<NetworkAuthenticationManager> authManager);
	DownloadBookRunnable(const std::string &url);
	~DownloadBookRunnable();
	void run();

	const std::string &fileName() const;

private:
	shared_ptr<BookReference> myReference;
	shared_ptr<NetworkAuthenticationManager> myAuthManager;
	std::string myFileName;
};

class IsAuthorisedRunnable : public NetworkOperationRunnable {

public:
	IsAuthorisedRunnable(NetworkAuthenticationManager &mgr);
	void run();

	ZLBoolean3 result();

private:
	NetworkAuthenticationManager &myManager;
	ZLBoolean3 myResult;
};

class AuthoriseRunnable : public NetworkOperationRunnable {

public:
	AuthoriseRunnable(NetworkAuthenticationManager &mgr, const std::string &password);
	void run();

private:
	NetworkAuthenticationManager &myManager;
	const std::string &myPassword;
};

class InitializeAuthenticationManagerRunnable : public NetworkOperationRunnable {

public:
	InitializeAuthenticationManagerRunnable(NetworkAuthenticationManager &mgr);
	void run();

private:
	NetworkAuthenticationManager &myManager;
};

class LogOutRunnable : public NetworkOperationRunnable {

public:
	LogOutRunnable(NetworkAuthenticationManager &mgr);
	void run();

private:
	NetworkAuthenticationManager &myManager;
};

class PurchaseBookRunnable : public NetworkOperationRunnable {

public:
	PurchaseBookRunnable(NetworkAuthenticationManager &mgr, const NetworkBookItem &book);
	void run();

private:
	NetworkAuthenticationManager &myManager;
	const NetworkBookItem &myBook;
};

class PasswordRecoveryRunnable : public NetworkOperationRunnable {

public:
	PasswordRecoveryRunnable(NetworkAuthenticationManager &mgr, const std::string &email);
	void run();

private:
	NetworkAuthenticationManager &myManager;
	const std::string &myEMail;
};

class RegisterUserRunnable : public NetworkOperationRunnable {

public:
	RegisterUserRunnable(NetworkAuthenticationManager &mgr, const std::string &login, const std::string &password, const std::string &email);
	void run();

private:
	NetworkAuthenticationManager &myManager;
	const std::string &myLogin;
	const std::string &myPassword;
	const std::string &myEMail;
};


class SearchRunnable : public NetworkOperationRunnable {

protected:
	SearchRunnable();

public:
	shared_ptr<NetworkBookCollection> result();

protected:
	shared_ptr<NetworkBookCollection> mySearchResult;
};

inline shared_ptr<NetworkBookCollection> SearchRunnable::result() { return mySearchResult; }


class SimpleSearchRunnable : public SearchRunnable {

public:
	SimpleSearchRunnable(const std::string &pattern);
	void run();

private:
	const std::string myPattern;
};


class AdvancedSearchRunnable : public SearchRunnable {

public:
	AdvancedSearchRunnable(const std::string &titleAndSeries, const std::string &author, const std::string &category, const std::string &description);
	void run();

private:
	const std::string myTitleAndSeries;
	const std::string myAuthor;
	const std::string myCategory;
	const std::string myDescription;
};


class LoadSubCatalogRunnable : public NetworkOperationRunnable {

public:
	LoadSubCatalogRunnable(NetworkCatalogItem &item, NetworkItem::List &children);
	void run();

private:
	NetworkCatalogItem &myItem;
	NetworkItem::List &myChildren;
};

#endif /* __NETWORKOPERATIONRUNNABLE_H__ */
