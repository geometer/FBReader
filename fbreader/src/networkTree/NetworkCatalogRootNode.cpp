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
#include "NetworkOperationRunnable.h"
#include "AuthenticationDialog.h"
#include "PasswordRecoveryDialog.h"
#include "RegisterUserDialog.h"

#include "../fbreader/FBReader.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

class NetworkCatalogRootNode::LoginAction : public ZLNamedRunnable {

public:
	LoginAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;

private:
	NetworkAuthenticationManager &myManager;
};

class NetworkCatalogRootNode::LogoutAction : public ZLNamedRunnable {

public:
	LogoutAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;

private:
	NetworkAuthenticationManager &myManager;
};

class NetworkCatalogRootNode::RefillAccountAction : public ZLNamedRunnable {

public:
	RefillAccountAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;
	std::string text(const ZLResource &resource) const;

private:
	NetworkAuthenticationManager &myManager;
};

class NetworkCatalogRootNode::DontShowAction : public ZLNamedRunnable {

public:
	DontShowAction(NetworkLink &link);
	void run();
	ZLResourceKey key() const;

private:
	NetworkLink &myLink;
};

class NetworkCatalogRootNode::PasswordRecoveryAction : public ZLNamedRunnable {

public:
	PasswordRecoveryAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;

private:
	NetworkAuthenticationManager &myManager;
};

class NetworkCatalogRootNode::RegisterUserAction : public ZLNamedRunnable {

public:
	RegisterUserAction(NetworkAuthenticationManager &mgr);
	void run();
	ZLResourceKey key() const;

private:
	NetworkAuthenticationManager &myManager;
};

const ZLTypeId NetworkCatalogRootNode::TYPE_ID(NetworkCatalogNode::TYPE_ID);

NetworkCatalogRootNode::NetworkCatalogRootNode(ZLBlockTreeView::RootNode *parent, NetworkLink &link, size_t atPosition) : NetworkCatalogNode(parent, link.libraryItem(), atPosition), myLink(link) {
	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	if (!mgr.isNull()) {
		myLoginAction = new LoginAction(*mgr);
		myLogoutAction = new LogoutAction(*mgr);
		myRefillAccountAction = new RefillAccountAction(*mgr);
		if (mgr->registrationSupported()) {
			myRegisterUserAction = new RegisterUserAction(*mgr);
		}
		if (mgr->passwordRecoverySupported()) {
			myPasswordRecoveryAction = new PasswordRecoveryAction(*mgr);
		}
	}
	myDontShowAction = new DontShowAction(myLink);
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

bool NetworkCatalogRootNode::hasAuxHyperlink() const {
	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	if (mgr.isNull() || mgr->isAuthorised(false).Status != B3_FALSE) {
		return false;
	}
	return !myRegisterUserAction.isNull() || !myPasswordRecoveryAction.isNull();
}

void NetworkCatalogRootNode::paintHyperlinks(ZLPaintContext &context, int vOffset) {
	int left = 0;
	int auxleft = 0;
	drawHyperlink(context, left, vOffset, expandCatalogAction());
	if (isOpen()) {
		drawHyperlink(context, left, vOffset, reloadAction());
	}

	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	if (!mgr.isNull()) {
		if (mgr->isAuthorised(false).Status == B3_FALSE) {
			drawHyperlink(context, left, vOffset, myLoginAction);
			drawHyperlink(context, auxleft, vOffset, myRegisterUserAction, true);
			drawHyperlink(context, auxleft, vOffset, myPasswordRecoveryAction, true);
		} else {
			drawHyperlink(context, left, vOffset, myLogoutAction);
			if (!mgr->currentAccount().empty() &&
					!mgr->refillAccountLink().empty()) {
				drawHyperlink(context, left, vOffset, myRefillAccountAction);
			}
		}
	}
	if (NetworkLinkCollection::Instance().numberOfEnabledLinks() > 1) {
		drawHyperlink(context, left, vOffset, myDontShowAction);
	}
}

shared_ptr<ZLImage> NetworkCatalogRootNode::lastResortCoverImage() const {
	return 0;
}


NetworkCatalogRootNode::LoginAction::LoginAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
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

NetworkCatalogRootNode::LogoutAction::LogoutAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
}

ZLResourceKey NetworkCatalogRootNode::LogoutAction::key() const {
	return ZLResourceKey("logout");
}

std::string NetworkCatalogRootNode::LogoutAction::text(const ZLResource &resource) const {
	const std::string text = ZLNamedRunnable::text(resource);
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

NetworkCatalogRootNode::RefillAccountAction::RefillAccountAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
}

ZLResourceKey NetworkCatalogRootNode::RefillAccountAction::key() const {
	return ZLResourceKey("refillAccount");
}

std::string NetworkCatalogRootNode::RefillAccountAction::text(const ZLResource &resource) const {
	const std::string text = ZLNamedRunnable::text(resource);
	std::string account = myManager.currentAccount();
	if (!account.empty() && !myManager.refillAccountLink().empty()) {
		return ZLStringUtil::printf(text, account);
	}
	return text;
}

void NetworkCatalogRootNode::RefillAccountAction::run() {
	FBReader::Instance().openLinkInBrowser(myManager.refillAccountLink());
}

NetworkCatalogRootNode::PasswordRecoveryAction::PasswordRecoveryAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
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

NetworkCatalogRootNode::RegisterUserAction::RegisterUserAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
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
