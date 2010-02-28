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

#include "../fbreader/FBReader.h"

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/UserList.h"

#include "NetworkLibraryPage.h"


class NetworkLinkBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	NetworkLinkBooleanOptionEntry(ZLBooleanOption &option);
	bool initialState() const;
	void onAccept(bool state);

private:
	ZLBooleanOption &myOption;
};

NetworkLinkBooleanOptionEntry::NetworkLinkBooleanOptionEntry(ZLBooleanOption &option) : myOption(option) {
}

bool NetworkLinkBooleanOptionEntry::initialState() const {
	return myOption.value();
}

void NetworkLinkBooleanOptionEntry::onAccept(bool state) {
	bool oldState = myOption.value();
	myOption.setValue(state);
	if (state != oldState) {
		FBReader::Instance().invalidateNetworkView();
	}
}


NetworkLibraryPage::NetworkLibraryPage(ZLDialogContent &dialogTab) {
	NetworkLinkCollection &linkCollection = NetworkLinkCollection::Instance();
	const size_t linkCollectionSize = linkCollection.size();
	for (size_t i = 0; i < linkCollectionSize; ++i) {
		NetworkLink &link = linkCollection.link(i);
		dialogTab.addOption(link.SiteName, "", new NetworkLinkBooleanOptionEntry(link.OnOption));
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
