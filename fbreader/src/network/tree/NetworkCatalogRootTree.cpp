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

#include "NetworkTreeNodes.h"

const ZLTypeId NetworkCatalogRootTree::TYPE_ID(NetworkCatalogTree::TYPE_ID);

const ZLTypeId &NetworkCatalogRootTree::typeId() const {
	return TYPE_ID;
}

NetworkCatalogRootTree::NetworkCatalogRootTree(RootTree *parent, NetworkLink &link, size_t position) :
	NetworkCatalogTree(parent, link.libraryItem(), position), myLink(link) {
	init(); //at old version, init is called when node should be painted (and if initialized yet)
}

void NetworkCatalogRootTree::init() {
	//shared_ptr<NetworkAuthenticationManager> mgr = myLink.authenticationManager();
	registerAction(new ExpandCatalogAction(*this));
	registerAction(new ReloadAction(*this));
//	if (!mgr.isNull()) {
//		registerAction(new LoginAction(*mgr));
//		registerAction(new LogoutAction(*mgr));
//		if (!mgr->refillAccountLink().empty()) {
//			registerAction(new RefillAccountAction(*mgr));
//		}
//		if (mgr->registrationSupported()) {
//			registerAction(new RegisterUserAction(*mgr), true);
//		}
//		if (mgr->passwordRecoverySupported()) {
//			registerAction(new PasswordRecoveryAction(*mgr), true);
//		}
	//	}
}

const ZLResource &NetworkCatalogRootTree::resource() const {
	   return ZLResource::resource("networkView")["libraryItemRootNode"];
}
