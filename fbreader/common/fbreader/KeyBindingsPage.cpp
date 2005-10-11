/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>

#include "FBReader.h"
#include "KeyBindingsPage.h"

class KeyboardControlEntry : public ZLSimpleBooleanOptionEntry {

public:
	KeyboardControlEntry(FBReader &fbreader);
	void onValueChange(bool state);

private:
	FBReader &myFBReader;
};

KeyboardControlEntry::KeyboardControlEntry(FBReader &fbreader) : ZLSimpleBooleanOptionEntry("Grab System Keys", FBReader::KeyboardControlOption), myFBReader(fbreader) {
}

void KeyboardControlEntry::onValueChange(bool state) {
	ZLSimpleBooleanOptionEntry::onValueChange(state);
	myFBReader.grabAllKeys(state);
}

class FBReaderKeyOptionEntry : public ZLKeyOptionEntry {

public:
	FBReaderKeyOptionEntry(FBReader &fbreader);
	~FBReaderKeyOptionEntry();
	void onAccept();
	int actionIndex(const std::string &key);
	void onValueChange(const std::string &key, int index);

private:
	void addAction(FBReader::ActionCode code, const std::string &name);

private:
	FBReader &myFBReader;
	std::vector<FBReader::ActionCode> myCodeByIndex;
	std::map<FBReader::ActionCode,int> myIndexByCode;

	std::map<std::string,FBReader::ActionCode> myChangedCodes;
};

void FBReaderKeyOptionEntry::addAction(FBReader::ActionCode code, const std::string &name) {
	myIndexByCode[code] = myCodeByIndex.size();
	myCodeByIndex.push_back(code);
	addActionName(name);
}

FBReaderKeyOptionEntry::FBReaderKeyOptionEntry(FBReader &fbreader) : ZLKeyOptionEntry("Key settings"), myFBReader(fbreader) {
	addAction(FBReader::NO_ACTION, "None");

	// switch view
	addAction(FBReader::ACTION_SHOW_COLLECTION, "Show Library");
	addAction(FBReader::ACTION_SHOW_LAST_BOOKS, "Show Recent Books");
	addAction(FBReader::ACTION_SHOW_CONTENTS, "Show Table Of Contents");

	// navigation
	addAction(FBReader::ACTION_SCROLL_TO_HOME, "Go To Home");
	addAction(FBReader::ACTION_SCROLL_TO_START_OF_TEXT, "Go To Start Of Section");
	addAction(FBReader::ACTION_SCROLL_TO_END_OF_TEXT, "Go To End Of Section");
	addAction(FBReader::ACTION_LARGE_SCROLL_FORWARD, "Large Scroll Forward");
	addAction(FBReader::ACTION_LARGE_SCROLL_BACKWARD, "Large Scroll Backward");
	addAction(FBReader::ACTION_SMALL_SCROLL_FORWARD, "Small Scroll Forward");
	addAction(FBReader::ACTION_SMALL_SCROLL_BACKWARD, "Small Scroll Backward");
	addAction(FBReader::ACTION_UNDO, "Undo");
	addAction(FBReader::ACTION_REDO, "Redo");

	// search
	addAction(FBReader::ACTION_SEARCH, "Search");
	addAction(FBReader::ACTION_FIND_PREVIOUS, "Find Previous");
	addAction(FBReader::ACTION_FIND_NEXT, "Find Next");

	// look
	addAction(FBReader::ACTION_INCREASE_FONT, "Increase Font Size");
	addAction(FBReader::ACTION_DECREASE_FONT, "Decrease Font Size");
	addAction(FBReader::ACTION_SHOW_HIDE_POSITION_INDICATOR, "Toggle Position Indicator");
	addAction(FBReader::ACTION_TOGGLE_FULLSCREEN, "Toggle Fullscreen Mode");
	addAction(FBReader::ACTION_FULLSCREEN_ON, "Switch To Fullscreen Mode");
	if (myFBReader.isRotationSupported()) {
		addAction(FBReader::ACTION_ROTATE_SCREEN, "Rotate Screen");
	}

	// dialogs
	addAction(FBReader::ACTION_SHOW_OPTIONS, "Show Options Dialog");
	addAction(FBReader::ACTION_SHOW_BOOK_INFO, "Show Book Info Dialog");
	addAction(FBReader::ACTION_ADD_BOOK, "Add Book");

	// quit
	addAction(FBReader::ACTION_CANCEL, "Cancel");
	addAction(FBReader::ACTION_QUIT, "Quit");
}

FBReaderKeyOptionEntry::~FBReaderKeyOptionEntry() {
}

void FBReaderKeyOptionEntry::onAccept() {
	for (std::map<std::string,FBReader::ActionCode>::const_iterator it = myChangedCodes.begin(); it != myChangedCodes.end(); it++) {
		myFBReader.bindKey(it->first, it->second);
	}
}

int FBReaderKeyOptionEntry::actionIndex(const std::string &key) {
	std::map<std::string,FBReader::ActionCode>::const_iterator it = myChangedCodes.find(key);
	return myIndexByCode[(it != myChangedCodes.end()) ? it->second : myFBReader.keyBinding(key)];
}

void FBReaderKeyOptionEntry::onValueChange(const std::string &key, int index) {
	myChangedCodes[key] = myCodeByIndex[index];
}

KeyBindingsPage::KeyBindingsPage(FBReader &fbreader, ZLOptionsDialogTab *dialogTab) {
	if (fbreader.isFullKeyboardControlSupported()) {
		dialogTab->addOption(new KeyboardControlEntry(fbreader));
	}
	dialogTab->addOption(new FBReaderKeyOptionEntry(fbreader));
}
