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

#include "../network/NetworkLibraryItems.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkLink.h"
#include "../network/NetworkAuthenticationManager.h"

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

const NetworkLink &NetworkCatalogRootNode::link() const {
	return myLink;
}

bool NetworkCatalogRootNode::hasAuxHyperlink() const {
	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	return !mgr.isNull() && mgr->isAuthorised(false) == B3_FALSE;
}

void NetworkCatalogRootNode::paintHyperlinks(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("networkView")["libraryItemRootNode"];

	int left = 0;
	int auxleft = 0;
	drawHyperlink(
		context, left, vOffset,
		resource[isOpen() ? "collapseTree" : "expandTree"].value(),
		expandCatalogAction()
	);
	if (isOpen()) {
		drawHyperlink(context, left, vOffset, resource["reload"].value(), reloadAction());
	}

	shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	if (!mgr.isNull()) {
		if (mgr->isAuthorised(false) == B3_FALSE) {
			drawHyperlink(context, left, vOffset, resource["login"].value(), myLoginAction);
			drawAuxHyperlink(context, auxleft, vOffset, resource["passwordRecovery"].value(), myPasswordRecoveryAction);
			drawAuxHyperlink(context, auxleft, vOffset, resource["register"].value(), myRegisterUserAction);
		} else {
			const std::string logoutString = ZLStringUtil::printf(resource["logout"].value(), mgr->currentUserName());
			drawHyperlink(context, left, vOffset, logoutString, myLogoutAction);
			std::string account = mgr->currentAccount();
			if (!account.empty() && !mgr->refillAccountLink().empty()) {
				const std::string refillString = ZLStringUtil::printf(resource["refillAccount"].value(), account);
				drawHyperlink(context, left, vOffset, refillString, myRefillAccountAction);
			}
		}
	}
	if (NetworkLinkCollection::Instance().numberOfEnabledLinks() > 1) {
		drawHyperlink(context, left, vOffset, resource["dontShow"].value(), myDontShowAction);
	}
}

shared_ptr<ZLImage> NetworkCatalogRootNode::lastResortCoverImage() const {
	return 0;
}


NetworkCatalogRootNode::LoginAction::LoginAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
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

void NetworkCatalogRootNode::LogoutAction::run() {
	myManager.logOut();
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}

NetworkCatalogRootNode::DontShowAction::DontShowAction(NetworkLink &link) : myLink(link) {
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

void NetworkCatalogRootNode::RefillAccountAction::run() {
	FBReader::Instance().openLinkInBrowser(myManager.refillAccountLink());
}

NetworkCatalogRootNode::PasswordRecoveryAction::PasswordRecoveryAction(NetworkAuthenticationManager &mgr) : myManager(mgr) {
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

void NetworkCatalogRootNode::RegisterUserAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	RegisterUserDialog::run(myManager);
	FBReader::Instance().invalidateAccountDependents();
	FBReader::Instance().refreshWindow();
}
