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

#include <map>

#include <ZLStringUtil.h>

#include "Migration.h"
#include "../fbreader/FBReaderActions.h"

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
	oldToNewNames["1"] = ActionCode::SHOW_LIBRARY;
	oldToNewNames["30"] = ActionCode::OPEN_PREVIOUS_BOOK;
	oldToNewNames["5"] = ActionCode::SHOW_TOC;
	oldToNewNames["15"] = ActionCode::SCROLL_TO_HOME;
	oldToNewNames["16"] = ActionCode::SCROLL_TO_START_OF_TEXT;
	oldToNewNames["17"] = ActionCode::SCROLL_TO_END_OF_TEXT;
	oldToNewNames["33"] = ActionCode::GOTO_NEXT_TOC_SECTION;
	oldToNewNames["34"] = ActionCode::GOTO_PREVIOUS_TOC_SECTION;
	oldToNewNames["9"] = ActionCode::PAGE_SCROLL_FORWARD;
	oldToNewNames["10"] = ActionCode::PAGE_SCROLL_BACKWARD;
	oldToNewNames["11"] = ActionCode::LINE_SCROLL_FORWARD;
	oldToNewNames["12"] = ActionCode::LINE_SCROLL_BACKWARD;
	oldToNewNames["3"] = ActionCode::UNDO;
	oldToNewNames["4"] = ActionCode::REDO;
	oldToNewNames["35"] = ActionCode::COPY_SELECTED_TEXT_TO_CLIPBOARD;
	oldToNewNames["37"] = ActionCode::OPEN_SELECTED_TEXT_IN_DICTIONARY;
	oldToNewNames["36"] = ActionCode::CLEAR_SELECTION;
	oldToNewNames["6"] = ActionCode::SEARCH;
	oldToNewNames["7"] = ActionCode::FIND_PREVIOUS;
	oldToNewNames["8"] = ActionCode::FIND_NEXT;
	oldToNewNames["19"] = ActionCode::INCREASE_FONT;
	oldToNewNames["20"] = ActionCode::DECREASE_FONT;
	oldToNewNames["21"] = ActionCode::SHOW_HIDE_POSITION_INDICATOR;
	oldToNewNames["22"] = ActionCode::TOGGLE_FULLSCREEN;
	oldToNewNames["23"] = ActionCode::FULLSCREEN_ON;
	oldToNewNames["27"] = ActionCode::ROTATE_SCREEN;
	oldToNewNames["2"] = ActionCode::SHOW_OPTIONS_DIALOG;
	oldToNewNames["25"] = ActionCode::SHOW_BOOK_INFO_DIALOG;
	oldToNewNames["24"] = ActionCode::ADD_BOOK;
	oldToNewNames["18"] = ActionCode::CANCEL;
	oldToNewNames["29"] = ActionCode::QUIT;

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
