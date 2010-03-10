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

#include <ZLStringUtil.h>
#include <ZLResource.h>
#include <ZLImage.h>
#include <ZLDialogManager.h>

#include "NetworkNodes.h"

#include "../fbreader/FBReader.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

#include "../networkActions/NetworkOperationRunnable.h"
#include "../networkActions/AuthenticationDialog.h"
#include "../networkActions/PasswordRecoveryDialog.h"
#include "../networkActions/RegisterUserDialog.h"

class NetworkCatalogAuthAction : public ZLRunnableWithKey {

protected:
	NetworkCatalogAuthAction(NetworkAuthenticationManager &mgr, bool forLoggedUsers);

protected:
	bool makesSense() const;

protected:
	NetworkAuthenticationManager &myManager;

private:
	const bool myForLoggedUsers;
};

class NetworkCatalogRootNode::LoginAction : public NetworkCatalogAuthAction {

public:
	LoginAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

class NetworkCatalogRootNode::LogoutAction : public NetworkCatalogAuthAction {

public:
	LogoutAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;
};

class NetworkCatalogRootNode::RefillAccountAction : public NetworkCatalogAuthAction {

public:
	RefillAccountAction(NetworkAuthenticationManager &mgr);

private:
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;
	bool makesSense() const;
};

class NetworkCatalogRootNode::DontShowAction : public ZLRunnableWithKey {

public:
	DontShowAction(NetworkLink &link);

private:
	void run();
	ZLResourceKey key() const;
	bool makesSense() const;

private:
	NetworkLink &myLink;
};

class NetworkCatalogRootNode::PasswordRecoveryAction : public NetworkCatalogAuthAction {

public:
	PasswordRecoveryAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

class NetworkCatalogRootNode::RegisterUserAction : public NetworkCatalogAuthAction {

public:
	RegisterUserAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
};

const ZLTypeId NetworkCatalogRootNode::TYPE_ID(NetworkCatalogNode::TYPE_ID);

NetworkCatalogRootNode::NetworkCatalogRootNode(ZLBlockTreeView::RootNode *parent, NetworkLink &link, size_t atPosition) : NetworkCatalogNode(parent, link.libraryItem(), atPosition), myLink(link) {
}

void NetworkCatalogRootNode::init() {
	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	registerAction(new ExpandCatalogAction(*this));
	registerAction(new ReloadAction(*this));
	if (!mgr.isNull()) {
		registerAction(new LoginAction(*mgr));
		registerAction(new LogoutAction(*mgr));
		if (!mgr->refillAccountLink().empty()) {
			registerAction(new RefillAccountAction(*mgr));
		}
		if (mgr->registrationSupported()) {
			registerAction(new RegisterUserAction(*mgr), true);
		}
		if (mgr->passwordRecoverySupported()) {
			registerAction(new PasswordRecoveryAction(*mgr), true);
		}
	}
	registerAction(new DontShowAction(myLink));
}

const ZLTypeId &NetworkCatalogRootNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkCatalogRootNode::resource() const {
	return ZLResource::resource("networkView")["libraryItemRootNode"];
}

const NetworkLink &NetworkCatalogRootNode::link() const {
	return myLink;
}

shared_ptr<ZLImage> NetworkCatalogRootNode::lastResortCoverImage() const {
	return 0;
}

NetworkCatalogAuthAction::NetworkCatalogAuthAction(NetworkAuthenticationManager &mgr, bool forLoggedUsers) : myManager(mgr), myForLoggedUsers(forLoggedUsers) {
}

bool NetworkCatalogAuthAction::makesSense() const {
	return (myManager.isAuthorised(false).Status == B3_FALSE) != myForLoggedUsers;
}

NetworkCatalogRootNode::LoginAction::LoginAction(NetworkAuthenticationManager &mgr) : NetworkCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootNode::LoginAction::key() const {
	return ZLResourceKey("login");
}

void NetworkCatalogRootNode::LoginAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	AuthenticationDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}

NetworkCatalogRootNode::LogoutAction::LogoutAction(NetworkAuthenticationManager &mgr) : NetworkCatalogAuthAction(mgr, true) {
}

ZLResourceKey NetworkCatalogRootNode::LogoutAction::key() const {
	return ZLResourceKey("logout");
}

std::string NetworkCatalogRootNode::LogoutAction::text(const ZLResource &resource) const {
	const std::string text = ZLRunnableWithKey::text(resource);
	return ZLStringUtil::printf(text, myManager.currentUserName());
}

void NetworkCatalogRootNode::LogoutAction::run() {
	LogOutRunnable logout(myManager);
	logout.executeWithUI();
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}

NetworkCatalogRootNode::DontShowAction::DontShowAction(NetworkLink &link) : myLink(link) {
}

ZLResourceKey NetworkCatalogRootNode::DontShowAction::key() const {
	return ZLResourceKey("dontShow");
}

void NetworkCatalogRootNode::DontShowAction::run() {
	ZLResourceKey boxKey("dontShowConfirmBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myLink.SiteName);
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}
	myLink.OnOption.setValue(false);
	FBReader::Instance().invalidateNetworkView();
	FBReader::Instance().refreshWindow();
}

bool NetworkCatalogRootNode::DontShowAction::makesSense() const {
	return NetworkLinkCollection::Instance().numberOfEnabledLinks() > 1;
}

NetworkCatalogRootNode::RefillAccountAction::RefillAccountAction(NetworkAuthenticationManager &mgr) : NetworkCatalogAuthAction(mgr, true) {
}

ZLResourceKey NetworkCatalogRootNode::RefillAccountAction::key() const {
	return ZLResourceKey("refillAccount");
}

std::string NetworkCatalogRootNode::RefillAccountAction::text(const ZLResource &resource) const {
	const std::string text = ZLRunnableWithKey::text(resource);
	std::string account = myManager.currentAccount();
	if (!account.empty() && !myManager.refillAccountLink().empty()) {
		return ZLStringUtil::printf(text, account);
	}
	return text;
}

void NetworkCatalogRootNode::RefillAccountAction::run() {
	FBReader::Instance().openLinkInBrowser(myManager.refillAccountLink());
}

bool NetworkCatalogRootNode::RefillAccountAction::makesSense() const {
	return
		NetworkCatalogAuthAction::makesSense() && 
		!myManager.currentAccount().empty();
}

NetworkCatalogRootNode::PasswordRecoveryAction::PasswordRecoveryAction(NetworkAuthenticationManager &mgr) : NetworkCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootNode::PasswordRecoveryAction::key() const {
	return ZLResourceKey("passwordRecovery");
}

void NetworkCatalogRootNode::PasswordRecoveryAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	PasswordRecoveryDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}

NetworkCatalogRootNode::RegisterUserAction::RegisterUserAction(NetworkAuthenticationManager &mgr) : NetworkCatalogAuthAction(mgr, false) {
}

ZLResourceKey NetworkCatalogRootNode::RegisterUserAction::key() const {
	return ZLResourceKey("register");
}

void NetworkCatalogRootNode::RegisterUserAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	RegisterUserDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}
