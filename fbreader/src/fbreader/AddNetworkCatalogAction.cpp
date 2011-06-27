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

#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLNetworkManager.h>
#include <ZLibrary.h>

#include "../options/FBCategoryKey.h"

#include "FBReaderActions.h"

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"

AddNetworkCatalogAction::AddNetworkCatalogAction() : ModeDependentAction(FBReader::NETWORK_LIBRARY_MODE) {
}

void AddNetworkCatalogAction::run() {
	shared_ptr<ZLDialog> addDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("addNetworkCatalogDialog"));
	ZLStringOption URLOption(ZLCategoryKey::NETWORK, "url", "title", "");
	addDialog->addOption(ZLResourceKey("url"), URLOption);
	addDialog->addButton(ZLResourceKey("add"), true);
	addDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (addDialog->run()) {
		addDialog->acceptValues();
		addDialog.reset();
		std::string url = URLOption.value();
		std:: string localpath = ZLibrary::ApplicationWritableDirectory() + ZLibrary::FileNameDelimiter + "temp.xml";
		ZLNetworkManager::Instance().downloadFile(url, localpath);
	}
}


