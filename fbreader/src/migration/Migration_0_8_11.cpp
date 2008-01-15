/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <map>

#include <ZLStringUtil.h>

#include "Migration.h"

static void changeActionNames(const std::map<std::string,std::string> map, const std::string &group) {
	const int length = ZLIntegerOption(ZLCategoryKey::CONFIG, group, "Number", 0).value();
	for (int i = 0; i < length; ++i) {
		std::string optionName = "Action";
		ZLStringUtil::appendNumber(optionName, i);
		ZLStringOption option(ZLCategoryKey::CONFIG, group, optionName, "");
		std::string value = option.value();
		std::map<std::string,std::string>::const_iterator it = map.find(value);
		if (it != map.end()) {
			option.setValue(it->second);
		}
	}
}

static void changeActionNames() {
	std::map<std::string,std::string> oldToNewNames;
	oldToNewNames["0"] = "none";
	oldToNewNames["1"] = "showLibrary";
	oldToNewNames["28"] = "showRecent";
	oldToNewNames["30"] = "previousBook";
	oldToNewNames["5"] = "toc";
	oldToNewNames["15"] = "gotoHome";
	oldToNewNames["16"] = "gotoSectionStart";
	oldToNewNames["17"] = "gotoSectionEnd";
	oldToNewNames["33"] = "nextTOCSection";
	oldToNewNames["34"] = "previousTOCSection";
	oldToNewNames["9"] = "largeScrollForward";
	oldToNewNames["10"] = "largeScrollBackward";
	oldToNewNames["11"] = "smallScrollForward";
	oldToNewNames["12"] = "smallScrollBackward";
	oldToNewNames["3"] = "undo";
	oldToNewNames["4"] = "redo";
	oldToNewNames["35"] = "copyToClipboard";
	oldToNewNames["37"] = "openInDictionary";
	oldToNewNames["36"] = "clearSelection";
	oldToNewNames["6"] = "search";
	oldToNewNames["7"] = "findPrevious";
	oldToNewNames["8"] = "findNext";
	oldToNewNames["19"] = "increaseFont";
	oldToNewNames["20"] = "decreaseFont";
	oldToNewNames["21"] = "toggleIndicator";
	oldToNewNames["22"] = "toggleFullscreen";
	oldToNewNames["23"] = "onFullscreen";
	oldToNewNames["27"] = "rotate";
	oldToNewNames["2"] = "preferences";
	oldToNewNames["25"] = "bookInfo";
	oldToNewNames["24"] = "addBook";
	oldToNewNames["18"] = "cancel";
	oldToNewNames["29"] = "quit";

	changeActionNames(oldToNewNames, "Keys");
	changeActionNames(oldToNewNames, "Keys90");
	changeActionNames(oldToNewNames, "Keys180");
	changeActionNames(oldToNewNames, "Keys270");
}

Migration_0_8_11::Migration_0_8_11() : Migration("0.8.11") {
}

void Migration_0_8_11::doMigrationInternal() {
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "ScrollingDelay",
		ZLCategoryKey::CONFIG, "TapScrolling", "ScrollingDelay",
		"0"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "Mode",
		ZLCategoryKey::CONFIG, "TapScrolling", "Mode",
		"0"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "LinesToKeep",
		ZLCategoryKey::CONFIG, "TapScrolling", "LinesToKeep",
		"1"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "LinesToScroll",
		ZLCategoryKey::CONFIG, "TapScrolling", "LinesToScroll",
		"1"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "PercentToScroll",
		ZLCategoryKey::CONFIG, "TapScrolling", "PercentToScroll",
		"50"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "FingerTapScrolling", "Enabled",
		ZLCategoryKey::CONFIG, "TapScrolling", "Enabled",
		"true"	
	);
	moveOption(
		ZLCategoryKey::CONFIG, "Options", "ScrollingDelay",
		ZLCategoryKey::CONFIG, "LargeScrolling", "ScrollingDelay",
		"250"	
	);
	changeActionNames();
}
