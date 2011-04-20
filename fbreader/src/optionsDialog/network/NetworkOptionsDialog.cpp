/*
 * Copyright (C) 2010-2011 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkOptionsDialog.h"

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLStringUtil.h>

#include <ZLOptionEntry.h>
#include <ZLOptionsDialog.h>
#include <ZLNetworkManager.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "../../fbreader/FBReader.h"

#include "../../network/NetworkLinkCollection.h"
#include "../../network/NetworkLink.h"
#include "../../network/UserList.h"


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



NetworkOptionsDialog::NetworkOptionsDialog() : AbstractOptionsDialog(ZLResourceKey("NetworkOptionsDialog"), true) {
	FBReader &fbreader = FBReader::Instance();

	ZLDialogContent &connectionTab = dialog().createTab(ZLResourceKey("Connection"));

	ZLNetworkManager &networkManager = ZLNetworkManager::Instance();
	connectionTab.addOption(ZLResourceKey("timeout"), new ZLSimpleSpinOptionEntry(networkManager.TimeoutOption(), 5));
	if (!networkManager.providesProxyInfo()) {
		ZLToggleBooleanOptionEntry *useProxyEntry = new ZLToggleBooleanOptionEntry(networkManager.UseProxyOption());
		connectionTab.addOption(ZLResourceKey("useProxy"), useProxyEntry);
		ZLSimpleStringOptionEntry *proxyHostEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyHostOption());
		connectionTab.addOption(ZLResourceKey("proxyHost"), proxyHostEntry);
		ZLSimpleStringOptionEntry *proxyPortEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyPortOption());
		connectionTab.addOption(ZLResourceKey("proxyPort"), proxyPortEntry);
		useProxyEntry->addDependentEntry(proxyHostEntry);
		useProxyEntry->addDependentEntry(proxyPortEntry);
		useProxyEntry->onStateChanged(useProxyEntry->initialState());
	}

	ZLDialogContent &libraryTab = dialog().createTab(ZLResourceKey("NetworkLibrary"));

	NetworkLinkCollection &linkCollection = NetworkLinkCollection::Instance();
	const size_t linkCollectionSize = linkCollection.size();
	const size_t linkCollectionSizeMinusOne = linkCollectionSize - 1;
	for (size_t i = 0; i < linkCollectionSize; ++i) {
		NetworkLink &link = linkCollection.link(i);
		if (i < linkCollectionSizeMinusOne) {
			NetworkLink &link2 = linkCollection.link(++i);
			libraryTab.addOptions(link.SiteName, "", new NetworkLinkBooleanOptionEntry(link.OnOption),
					link2.SiteName, "", new NetworkLinkBooleanOptionEntry(link2.OnOption));
		} else {
			libraryTab.addOption(link.SiteName, "", new NetworkLinkBooleanOptionEntry(link.OnOption));
		}
	}

	std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	createIntegrationTab(fbreader.webBrowserCollection(), ZLResourceKey("Web"), additional);
}
