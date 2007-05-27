/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLOptionsDialog.h>

#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLSimpleKeyOptionEntry.h>

#include "KeyBindingsPage.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/FBReaderActions.h"

class KeyboardControlEntry : public ZLSimpleBooleanOptionEntry {

public:
	KeyboardControlEntry(FBReader &fbreader);
	void onStateChanged(bool state);

private:
	FBReader &myFBReader;
};

KeyboardControlEntry::KeyboardControlEntry(FBReader &fbreader) : ZLSimpleBooleanOptionEntry(fbreader.KeyboardControlOption), myFBReader(fbreader) {
}

void KeyboardControlEntry::onStateChanged(bool state) {
	ZLSimpleBooleanOptionEntry::onStateChanged(state);
	myFBReader.grabAllKeys(state);
}

class SingleKeyOptionEntry : public ZLSimpleKeyOptionEntry {

public:
	SingleKeyOptionEntry(const CodeIndexBimap &bimap, ZLKeyBindings &bindings);
	const CodeIndexBimap &codeIndexBimap() const;

private:
	const CodeIndexBimap &myBimap;
};

SingleKeyOptionEntry::SingleKeyOptionEntry(const CodeIndexBimap &bimap, ZLKeyBindings &bindings) : ZLSimpleKeyOptionEntry(bindings), myBimap(bimap) {
}

const ZLSimpleKeyOptionEntry::CodeIndexBimap &SingleKeyOptionEntry::codeIndexBimap() const {
	return myBimap;
}

class MultiKeyOptionEntry : public ZLKeyOptionEntry {

public:
	MultiKeyOptionEntry(FBReader &fbreader);
	void onAccept();
	int actionIndex(const std::string &key);
	void onValueChanged(const std::string &key, int index);

	void setOrientation(ZLViewWidget::Angle);

private:
	void addAction(int code, const std::string &name);

private:
	ZLSimpleKeyOptionEntry::CodeIndexBimap myBimap;

	SingleKeyOptionEntry myEntry0;
	SingleKeyOptionEntry myEntry90;
	SingleKeyOptionEntry myEntry180;
	SingleKeyOptionEntry myEntry270;
	SingleKeyOptionEntry *myCurrentEntry;
};

void MultiKeyOptionEntry::addAction(int code, const std::string &name) {
	myBimap.insert(code);
	addActionName(name);
}

MultiKeyOptionEntry::MultiKeyOptionEntry(FBReader &fbreader) :
	ZLKeyOptionEntry(),
	myEntry0(myBimap, fbreader.keyBindings(ZLViewWidget::DEGREES0)),
	myEntry90(myBimap, fbreader.keyBindings(ZLViewWidget::DEGREES90)),
	myEntry180(myBimap, fbreader.keyBindings(ZLViewWidget::DEGREES180)),
	myEntry270(myBimap, fbreader.keyBindings(ZLViewWidget::DEGREES270)),
	myCurrentEntry(&myEntry0) {
	addAction(NO_ACTION, "None");

	// switch view
	addAction(ACTION_SHOW_COLLECTION, "Show Library");
	addAction(ACTION_SHOW_LAST_BOOKS, "Show Recent Books");
	addAction(ACTION_OPEN_PREVIOUS_BOOK, "Open Previous Book");
	addAction(ACTION_SHOW_CONTENTS, "Show Table Of Contents");

	// navigation
	addAction(ACTION_SCROLL_TO_HOME, "Go To Home");
	addAction(ACTION_SCROLL_TO_START_OF_TEXT, "Go To Start Of Section");
	addAction(ACTION_SCROLL_TO_END_OF_TEXT, "Go To End Of Section");
	addAction(ACTION_GOTO_NEXT_TOC_SECTION, "Go To Next TOC Section");
	addAction(ACTION_GOTO_PREVIOUS_TOC_SECTION, "Go To Previous TOC Section");
	addAction(ACTION_LARGE_SCROLL_FORWARD, "Large Scroll Forward");
	addAction(ACTION_LARGE_SCROLL_BACKWARD, "Large Scroll Backward");
	addAction(ACTION_SMALL_SCROLL_FORWARD, "Small Scroll Forward");
	addAction(ACTION_SMALL_SCROLL_BACKWARD, "Small Scroll Backward");
	addAction(ACTION_UNDO, "Undo");
	addAction(ACTION_REDO, "Redo");

	// selection
	addAction(ACTION_COPY_SELECTED_TEXT_TO_CLIPBOARD, "Copy Selection To Clipboard");
	addAction(ACTION_OPEN_SELECTED_TEXT_IN_DICTIONARY, "Open Selected Text In Dictionary");
	addAction(ACTION_CLEAR_SELECTION, "Clear Selection");

	// search
	addAction(ACTION_SEARCH, "Search");
	addAction(ACTION_FIND_PREVIOUS, "Find Previous");
	addAction(ACTION_FIND_NEXT, "Find Next");

	// look
	addAction(ACTION_INCREASE_FONT, "Increase Font Size");
	addAction(ACTION_DECREASE_FONT, "Decrease Font Size");
	addAction(ACTION_SHOW_HIDE_POSITION_INDICATOR, "Toggle Position Indicator");
	addAction(ACTION_TOGGLE_FULLSCREEN, "Toggle Fullscreen Mode");
	addAction(ACTION_FULLSCREEN_ON, "Switch To Fullscreen Mode");
	addAction(ACTION_ROTATE_SCREEN, "Rotate Screen");

	// dialogs
	addAction(ACTION_SHOW_OPTIONS, "Show Options Dialog");
	addAction(ACTION_SHOW_BOOK_INFO, "Show Book Info Dialog");
	addAction(ACTION_ADD_BOOK, "Add Book");

	// quit
	addAction(ACTION_CANCEL, "Cancel");
	addAction(ACTION_QUIT, "Quit");
}

void MultiKeyOptionEntry::setOrientation(ZLViewWidget::Angle angle) {
	switch (angle) {
		case ZLViewWidget::DEGREES0:
			myCurrentEntry = &myEntry0;
			break;
		case ZLViewWidget::DEGREES90:
			myCurrentEntry = &myEntry90;
			break;
		case ZLViewWidget::DEGREES180:
			myCurrentEntry = &myEntry180;
			break;
		case ZLViewWidget::DEGREES270:
			myCurrentEntry = &myEntry270;
			break;
	}
	resetView();
}

void MultiKeyOptionEntry::onAccept() {
	myEntry0.onAccept();
	myEntry90.onAccept();
	myEntry180.onAccept();
	myEntry270.onAccept();
}

int MultiKeyOptionEntry::actionIndex(const std::string &key) {
	return myCurrentEntry->actionIndex(key);
}

void MultiKeyOptionEntry::onValueChanged(const std::string &key, int index) {
	myCurrentEntry->onValueChanged(key, index);
}

class OrientationEntry : public ZLComboOptionEntry {

public:
	OrientationEntry(MultiKeyOptionEntry &keyEntry);
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onValueSelected(int index);
	void onAccept(const std::string &value);

private:
	MultiKeyOptionEntry &myKeyEntry;
};

OrientationEntry::OrientationEntry(MultiKeyOptionEntry &keyEntry) : myKeyEntry(keyEntry) {
}

const std::string &OrientationEntry::initialValue() const {
	return values()[0];
}

const std::vector<std::string> &OrientationEntry::values() const {
	static std::vector<std::string> _values;
	if (_values.empty()) {
		_values.push_back("0 Degrees");
		_values.push_back("90 Degrees Counterclockwise");
		_values.push_back("180 Degrees");
		_values.push_back("90 Degrees Clockwise");
	}
	return _values;
}

void OrientationEntry::onValueSelected(int index) {
	static ZLViewWidget::Angle angles[] = {
		ZLViewWidget::DEGREES0,
		ZLViewWidget::DEGREES90,
		ZLViewWidget::DEGREES180,
		ZLViewWidget::DEGREES270
	};
	myKeyEntry.setOrientation(angles[index]);
}

void OrientationEntry::onAccept(const std::string&) {
}

class UseSeparateOptionsEntry : public ZLSimpleBooleanOptionEntry {

public:
	UseSeparateOptionsEntry(FBReader &fbreader, ZLOptionEntry &keyEntry, OrientationEntry &orientationEntry);
	void onStateChanged(bool state);

private:
	ZLOptionEntry &myKeyEntry;
	OrientationEntry &myOrientationEntry;
};

UseSeparateOptionsEntry::UseSeparateOptionsEntry(FBReader &fbreader, ZLOptionEntry &keyEntry, OrientationEntry &orientationEntry) : ZLSimpleBooleanOptionEntry(fbreader.UseSeparateBindingsOption), myKeyEntry(keyEntry), myOrientationEntry(orientationEntry) {
}

void UseSeparateOptionsEntry::onStateChanged(bool state) {
	ZLSimpleBooleanOptionEntry::onStateChanged(state);
	myOrientationEntry.setVisible(state);
	myKeyEntry.resetView();
}

KeyBindingsPage::KeyBindingsPage(FBReader &fbreader, ZLDialogContent &dialogTab) {
	if (fbreader.isFullKeyboardControlSupported()) {
		dialogTab.addOption(ZLResourceKey("grabSystemKeys"), new KeyboardControlEntry(fbreader));
	}
	MultiKeyOptionEntry *keyEntry = new MultiKeyOptionEntry(fbreader);
	OrientationEntry *orientationEntry = new OrientationEntry(*keyEntry);
	ZLBooleanOptionEntry *useSeparateBindingsEntry = new UseSeparateOptionsEntry(fbreader, *keyEntry, *orientationEntry);
	dialogTab.addOption(ZLResourceKey("separate"), useSeparateBindingsEntry);
	dialogTab.addOption(ZLResourceKey("orientation"), orientationEntry);
	dialogTab.addOption("", "", keyEntry);
	useSeparateBindingsEntry->onStateChanged(useSeparateBindingsEntry->initialState());
}
