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
#include <ZLDialogManager.h>

#include <ZLOptionEntry.h>
#include <ZLOptionsDialog.h>
#include <ZLNetworkManager.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkAuthenticationManager.h"

#include "NetworkLibraryPage.h"

NetworkLibraryPage::NetworkLibraryPage(ZLDialogContent &dialogTab) {
	NetworkLinkCollection &linkCollection = NetworkLinkCollection::Instance();
	const size_t linkCollectionSize = linkCollection.size();
	for (size_t i = 0; i < linkCollectionSize; ++i) {
		NetworkLink &link = linkCollection.link(i);
		dialogTab.addOption(link.SiteName, "", new ZLSimpleBooleanOptionEntry(link.OnOption));
		myOldUseFlag.push_back(link.OnOption.value());
	}
	ZLNetworkManager &networkManager = ZLNetworkManager::Instance();
	if (!networkManager.providesProxyInfo()) {
		ZLToggleBooleanOptionEntry *useProxyEntry = new ZLToggleBooleanOptionEntry(networkManager.UseProxyOption());
		dialogTab.addOption(ZLResourceKey("useProxy"), useProxyEntry);
		ZLSimpleStringOptionEntry *proxyHostEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyHostOption());
		dialogTab.addOption(ZLResourceKey("proxyHost"), proxyHostEntry);
		ZLSimpleStringOptionEntry *proxyPortEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyPortOption());
		dialogTab.addOption(ZLResourceKey("proxyPort"), proxyPortEntry);
		useProxyEntry->addDependentEntry(proxyHostEntry);
		useProxyEntry->addDependentEntry(proxyPortEntry);
		useProxyEntry->onStateChanged(useProxyEntry->initialState());
	}
	dialogTab.addOption(ZLResourceKey("timeout"), new ZLSimpleSpinOptionEntry(networkManager.TimeoutOption(), 5));
}

bool NetworkLibraryPage::onApply() {
	NetworkLinkCollection &linkCollection = NetworkLinkCollection::Instance();
	const size_t linkCollectionSize = linkCollection.size();

	bool netChanged = false;

	for (size_t i = 0; i < linkCollectionSize; ++i) {
		NetworkLink &link = linkCollection.link(i);
		const bool oldUse = myOldUseFlag[i];
		if (link.OnOption.value() != oldUse) {
			shared_ptr<NetworkAuthenticationManager> mgr = link.authenticationManager();
			if (!mgr.isNull()) {
				mgr->logOut();
			}
			netChanged = true;
		}
	}

	return netChanged;
}
