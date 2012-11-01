/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>

#include "../../fbreader/FBReader.h"

#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "../../networkActions/AuthenticationDialog.h"
#include "../../networkActions/PasswordRecoveryDialog.h"
#include "../../networkActions/RegisterUserDialog.h"

#include "NetworkLibrary.h"

#include "NetworkTreeNodes.h"

class NetworkTreeCatalogAuthAction : public ZLTreeAction {

protected:
	NetworkTreeCatalogAuthAction(NetworkAuthenticationManager &mgr, bool forLoggedUsers);

protected:
	bool makesSense() const;

protected:
	NetworkAuthenticationManager &myManager;

private:
	const bool myForLoggedUsers;
};

class NetworkCatalogRootTree::LoginAction : public NetworkTreeCatalogAuthAction {

public:
	LoginAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

class NetworkCatalogRootTree::LogoutAction : public NetworkTreeCatalogAuthAction {

public:
	LogoutAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;
};

class NetworkCatalogRootTree::RefillAccountAction : public NetworkTreeCatalogAuthAction {

public:
	RefillAccountAction(NetworkAuthenticationManager &mgr);

private:
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;
	bool makesSense() const;
};

class NetworkCatalogRootTree::PasswordRecoveryAction : public NetworkTreeCatalogAuthAction {

public:
	PasswordRecoveryAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

class NetworkCatalogRootTree::RegisterUserAction : public NetworkTreeCatalogAuthAction {

public:
	RegisterUserAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

const ZLTypeId NetworkCatalogRootTree::TYPE_ID(NetworkCatalogTree::TYPE_ID);

const ZLTypeId &NetworkCatalogRootTree::typeId() const {
	return TYPE_ID;
}

NetworkCatalogRootTree::NetworkCatalogRootTree(RootTree *parent, NetworkLink &link, size_t position) :
	NetworkCatalogTree(parent, link.libraryItem(), position), myLink(link) {
	init(); //at old version, init is called when node should be painted (and if initialized yet)
}

void NetworkCatalogRootTree::init() {
	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	//registerAction(new ExpandCatalogAction(*this));
	//registerAction(new ReloadAction(*this));
	if (!mgr.isNull()) {
		registerAction(new LoginAction(*mgr));
		registerAction(new LogoutAction(*mgr));
		if (!mgr->refillAccountLink().empty()) {
			registerAction(new RefillAccountAction(*mgr));
		}
		if (mgr->registrationSupported()) {
			registerAction(new RegisterUserAction(*mgr));
		}
		if (mgr->passwordRecoverySupported()) {
			registerAction(new PasswordRecoveryAction(*mgr));
		}
	}
}

const ZLResource &NetworkCatalogRootTree::resource() const {
	   return ZLResource::resource("networkView")["libraryItemRootNode"];
}

NetworkTreeCatalogAuthAction::NetworkTreeCatalogAuthAction(NetworkAuthenticationManager &mgr, bool forLoggedUsers) : myManager(mgr), myForLoggedUsers(forLoggedUsers) {
}

bool NetworkTreeCatalogAuthAction::makesSense() const {
	return (myManager.isAuthorised(false).Status == B3_FALSE) != myForLoggedUsers;
}

NetworkCatalogRootTree::LoginAction::LoginAction(NetworkAuthenticationManager &mgr) : NetworkTreeCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootTree::LoginAction::key() const {
	return ZLResourceKey("login");
}

void NetworkCatalogRootTree::LoginAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	AuthenticationDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
	NetworkLibrary::Instance().refresh();
}

NetworkCatalogRootTree::LogoutAction::LogoutAction(NetworkAuthenticationManager &mgr) : NetworkTreeCatalogAuthAction(mgr, true) {
}

ZLResourceKey NetworkCatalogRootTree::LogoutAction::key() const {
	return ZLResourceKey("logout");
}

std::string NetworkCatalogRootTree::LogoutAction::text(const ZLResource &resource) const {
	const std::string text = ZLTreeAction::text(resource);
	return ZLStringUtil::printf(text, myManager.currentUserName());
}

void NetworkCatalogRootTree::LogoutAction::run() {
	LogOutRunnable logout(myManager);
	logout.executeWithUI();
//	FBReader::Instance().invalidateAccountDependents();
//	FBReader::Instance().refreshWindow();
	NetworkLibrary::Instance().refresh();
}


NetworkCatalogRootTree::RefillAccountAction::RefillAccountAction(NetworkAuthenticationManager &mgr) : NetworkTreeCatalogAuthAction(mgr, true) {
}

ZLResourceKey NetworkCatalogRootTree::RefillAccountAction::key() const {
	return ZLResourceKey("refillAccount");
}

std::string NetworkCatalogRootTree::RefillAccountAction::text(const ZLResource &resource) const {
	const std::string text = ZLTreeAction::text(resource);
	std::string account = myManager.currentAccount();
	if (!account.empty() && !myManager.refillAccountLink().empty()) {
		return ZLStringUtil::printf(text, account);
	}
	return text;
}

void NetworkCatalogRootTree::RefillAccountAction::run() {
	FBReader::Instance().openLinkInBrowser(myManager.refillAccountLink());
	NetworkLibrary::Instance().refresh();
}

bool NetworkCatalogRootTree::RefillAccountAction::makesSense() const {
	return
		NetworkTreeCatalogAuthAction::makesSense() &&
		!myManager.currentAccount().empty();
}

NetworkCatalogRootTree::PasswordRecoveryAction::PasswordRecoveryAction(NetworkAuthenticationManager &mgr) : NetworkTreeCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootTree::PasswordRecoveryAction::key() const {
	return ZLResourceKey("passwordRecovery");
}

void NetworkCatalogRootTree::PasswordRecoveryAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	PasswordRecoveryDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
	NetworkLibrary::Instance().refresh();
}

NetworkCatalogRootTree::RegisterUserAction::RegisterUserAction(NetworkAuthenticationManager &mgr) : NetworkTreeCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootTree::RegisterUserAction::key() const {
	return ZLResourceKey("register");
}

void NetworkCatalogRootTree::RegisterUserAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	RegisterUserDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
	NetworkLibrary::Instance().refresh();
}
