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
#include <stdio.h>

#include "../options/FBCategoryKey.h"

#include "FBReaderActions.h"

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/opds/OPDSLink_FeedReader.h"
#include "../network/opds/OPDSXMLParser.h"
#include "../database/booksdb/BooksDB.h"

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
		shared_ptr<NetworkLink> link = 0;
		shared_ptr<OPDSFeedReader> fr = new OPDSLink::FeedReader(link, url);
		shared_ptr<ZLXMLReader> prsr = new OPDSXMLParser(fr);
		ZLExecutionData::perform(ZLNetworkManager::Instance().createXMLParserRequest(url, prsr));

		shared_ptr<ZLDialog> checkDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("checkNetworkCatalogDialog"));
		ZLStringOption NameOption(ZLCategoryKey::NETWORK, "name", "title", "");
		NameOption.setValue(link->Title);
		checkDialog->addOption(ZLResourceKey("name"), NameOption);
		ZLStringOption SubNameOption(ZLCategoryKey::NETWORK, "subname", "title", "");
		SubNameOption.setValue(link->Summary);
		checkDialog->addOption(ZLResourceKey("subname"), SubNameOption);
		ZLStringOption IconOption(ZLCategoryKey::NETWORK, "icon", "title", "");
		IconOption.setValue(link->Icon);
		checkDialog->addOption(ZLResourceKey("icon"), IconOption);

		checkDialog->addButton(ZLResourceKey("add"), true);
		checkDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
		if (checkDialog->run()) {
			checkDialog->acceptValues();
			checkDialog.reset();
			link->Title = NameOption.value();
			link->Summary = SubNameOption.value();
			link->Icon = IconOption.value();
			link->myLinks.erase("icon");
			if (IconOption.value() != std::string()) {
				link->myLinks["icon"] = IconOption.value();
			}
			BooksDB::Instance().saveNetworkLink(link);
			NetworkLinkCollection::Instance().reReadLinks();
		}
	}
}

