/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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
#include <ZLDialog.h>

#include "../options/FBOptions.h"
#include "FBReaderActions.h"
#include "NetLibraryView.h"
#include "../network/NetworkLink.h"

ShowNetworkLibraryAction::ShowNetworkLibraryAction(FBReader &fbreader) : SetModeAction(fbreader, FBReader::NET_LIBRARY_MODE, FBReader::BOOK_TEXT_MODE) {
}

bool ShowNetworkLibraryAction::isVisible() const {
	return SetModeAction::isVisible() && NetworkLinkCollection::instance().containsEnabledLinks();
}

SearchOnNetworkAction::SearchOnNetworkAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::NET_LIBRARY_MODE) {
}

void SearchOnNetworkAction::run() {
	((NetLibraryView&)*fbreader().myNetLibraryView).search();
	fbreader().refreshWindow();
}

AdvancedSearchOnNetworkAction::AdvancedSearchOnNetworkAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::NET_LIBRARY_MODE) {
}

void AdvancedSearchOnNetworkAction::run() {
	shared_ptr<ZLDialog> searchDialog = ZLDialogManager::instance().createDialog(ZLResourceKey("networkSearchDialog"));

	ZLStringOption titleOption(FBCategoryKey::SEARCH, "network", "title", "");
	searchDialog->addOption(ZLResourceKey("bookTitle"), titleOption);
	ZLStringOption authorOption(FBCategoryKey::SEARCH, "network", "author", "");
	searchDialog->addOption(ZLResourceKey("author"), authorOption);
	ZLStringOption seriesOption(FBCategoryKey::SEARCH, "network", "series", "");
	searchDialog->addOption(ZLResourceKey("series"), seriesOption);
	ZLStringOption categoryOption(FBCategoryKey::SEARCH, "network", "category", "");
	searchDialog->addOption(ZLResourceKey("category"), categoryOption);
	ZLStringOption descriptionOption(FBCategoryKey::SEARCH, "network", "description", "");
	searchDialog->addOption(ZLResourceKey("description"), descriptionOption);
	searchDialog->addButton(ZLResourceKey("go"), true);
	searchDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (searchDialog->run()) {
		searchDialog->acceptValues();
		searchDialog.reset();
		std::string titlePattern = titleOption.value();
		ZLStringUtil::stripWhiteSpaces(titlePattern);
		std::string authorPattern = authorOption.value();
		ZLStringUtil::stripWhiteSpaces(authorPattern);
		std::string seriesPattern = seriesOption.value();
		ZLStringUtil::stripWhiteSpaces(seriesPattern);
		std::string categoryPattern = categoryOption.value();
		ZLStringUtil::stripWhiteSpaces(categoryPattern);
		std::string descriptionPattern = descriptionOption.value();
		ZLStringUtil::stripWhiteSpaces(descriptionPattern);

		if (!titlePattern.empty() ||
				!authorPattern.empty() ||
				!seriesPattern.empty() ||
				!categoryPattern.empty() ||
				!descriptionPattern.empty()) {
			((NetLibraryView&)*fbreader().myNetLibraryView).search(titlePattern, authorPattern, seriesPattern, categoryPattern, descriptionPattern);
			fbreader().refreshWindow();
		}
	}
}
