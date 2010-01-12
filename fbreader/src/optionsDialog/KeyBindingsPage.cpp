/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLApplication.h>

#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLSimpleKeyOptionEntry.h>

#include "KeyBindingsPage.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/FBReaderActions.h"

class KeyboardControlEntry : public ZLSimpleBooleanOptionEntry {

public:
	KeyboardControlEntry();
	void onStateChanged(bool state);
};

KeyboardControlEntry::KeyboardControlEntry() : ZLSimpleBooleanOptionEntry(FBReader::Instance().KeyboardControlOption) {
}

void KeyboardControlEntry::onStateChanged(bool state) {
	ZLSimpleBooleanOptionEntry::onStateChanged(state);
	FBReader::Instance().grabAllKeys(state);
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
	MultiKeyOptionEntry(const ZLResource &resource);
	void onAccept();
	int actionIndex(const std::string &key);
	void onValueChanged(const std::string &key, int index);
	void onKeySelected(const std::string &key);

	void setOrientation(ZLView::Angle);
	void setExitOnCancelEntry(ZLOptionEntry *exitOnCancelEntry);

private:
	void addAction(const std::string &actionId);

private:
	const ZLResource &myResource;
	ZLSimpleKeyOptionEntry::CodeIndexBimap myBimap;

	SingleKeyOptionEntry myEntry0;
	SingleKeyOptionEntry myEntry90;
	SingleKeyOptionEntry myEntry180;
	SingleKeyOptionEntry myEntry270;
	SingleKeyOptionEntry *myCurrentEntry;
	ZLOptionEntry *myExitOnCancelEntry;
};

void MultiKeyOptionEntry::addAction(const std::string &actionId) {
	myBimap.insert(actionId);
	addActionName(myResource[ZLResourceKey(actionId)].value());
}

MultiKeyOptionEntry::MultiKeyOptionEntry(const ZLResource &resource) :
	ZLKeyOptionEntry(),
	myResource(resource),
	myEntry0(myBimap, *FBReader::Instance().keyBindings(ZLView::DEGREES0)),
	myEntry90(myBimap, *FBReader::Instance().keyBindings(ZLView::DEGREES90)),
	myEntry180(myBimap, *FBReader::Instance().keyBindings(ZLView::DEGREES180)),
	myEntry270(myBimap, *FBReader::Instance().keyBindings(ZLView::DEGREES270)),
	myCurrentEntry(&myEntry0),
	myExitOnCancelEntry(0) {
	addAction(ZLApplication::NoAction);

	// switch view
	addAction(ActionCode::SHOW_LIBRARY);
	addAction(ActionCode::SHOW_NETWORK_LIBRARY);
	addAction(ActionCode::OPEN_PREVIOUS_BOOK);
	addAction(ActionCode::SHOW_TOC);

	// navigation
	addAction(ActionCode::SCROLL_TO_HOME);
	addAction(ActionCode::SCROLL_TO_START_OF_TEXT);
	addAction(ActionCode::SCROLL_TO_END_OF_TEXT);
	addAction(ActionCode::GOTO_NEXT_TOC_SECTION);
	addAction(ActionCode::GOTO_PREVIOUS_TOC_SECTION);
	addAction(ActionCode::PAGE_SCROLL_FORWARD);
	addAction(ActionCode::PAGE_SCROLL_BACKWARD);
	addAction(ActionCode::LINE_SCROLL_FORWARD);
	addAction(ActionCode::LINE_SCROLL_BACKWARD);
	addAction(ActionCode::UNDO);
	addAction(ActionCode::REDO);

	// selection
	addAction(ActionCode::COPY_SELECTED_TEXT_TO_CLIPBOARD);
	addAction(ActionCode::OPEN_SELECTED_TEXT_IN_DICTIONARY);
	addAction(ActionCode::CLEAR_SELECTION);

	// search
	addAction(ActionCode::SEARCH);
	addAction(ActionCode::FIND_PREVIOUS);
	addAction(ActionCode::FIND_NEXT);

	// look
	addAction(ActionCode::INCREASE_FONT);
	addAction(ActionCode::DECREASE_FONT);
	addAction(ActionCode::SHOW_HIDE_POSITION_INDICATOR);
	addAction(ActionCode::TOGGLE_FULLSCREEN);
	addAction(ActionCode::ROTATE_SCREEN);

	// dialogs
	addAction(ActionCode::SHOW_OPTIONS_DIALOG);
	addAction(ActionCode::SHOW_BOOK_INFO_DIALOG);
	addAction(ActionCode::ADD_BOOK);

	// quit
	addAction(ActionCode::CANCEL);
	addAction(ActionCode::QUIT);
}

void MultiKeyOptionEntry::setOrientation(ZLView::Angle angle) {
	switch (angle) {
		case ZLView::DEGREES0:
			myCurrentEntry = &myEntry0;
			break;
		case ZLView::DEGREES90:
			myCurrentEntry = &myEntry90;
			break;
		case ZLView::DEGREES180:
			myCurrentEntry = &myEntry180;
			break;
		case ZLView::DEGREES270:
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
	if (myExitOnCancelEntry != 0) {
		myExitOnCancelEntry->setVisible(myBimap.codeByIndex(index) == ActionCode::CANCEL);
	}
}

void MultiKeyOptionEntry::setExitOnCancelEntry(ZLOptionEntry *exitOnCancelEntry) {
	myExitOnCancelEntry = exitOnCancelEntry;
}

void MultiKeyOptionEntry::onKeySelected(const std::string &key) {
	if (myExitOnCancelEntry != 0) {
		myExitOnCancelEntry->setVisible(myBimap.codeByIndex(myCurrentEntry->actionIndex(key)) == ActionCode::CANCEL);
	}
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
	static ZLView::Angle angles[] = {
		ZLView::DEGREES0,
		ZLView::DEGREES90,
		ZLView::DEGREES180,
		ZLView::DEGREES270
	};
	myKeyEntry.setOrientation(angles[index]);
}

void OrientationEntry::onAccept(const std::string&) {
}

class UseSeparateOptionsEntry : public ZLSimpleBooleanOptionEntry {

public:
	UseSeparateOptionsEntry(ZLOptionEntry &keyEntry, OrientationEntry &orientationEntry);
	void onStateChanged(bool state);

private:
	ZLOptionEntry &myKeyEntry;
	OrientationEntry &myOrientationEntry;
};

UseSeparateOptionsEntry::UseSeparateOptionsEntry(ZLOptionEntry &keyEntry, OrientationEntry &orientationEntry) : ZLSimpleBooleanOptionEntry(FBReader::Instance().UseSeparateBindingsOption), myKeyEntry(keyEntry), myOrientationEntry(orientationEntry) {
}

void UseSeparateOptionsEntry::onStateChanged(bool state) {
	ZLSimpleBooleanOptionEntry::onStateChanged(state);
	myOrientationEntry.setVisible(state);
	myKeyEntry.resetView();
}

KeyBindingsPage::KeyBindingsPage(ZLDialogContent &dialogTab) {
	FBReader &fbreader = FBReader::Instance();
	if (ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::FULL_KEYBOARD_CONTROL, false).value()) {
		dialogTab.addOption(ZLResourceKey("grabSystemKeys"), new KeyboardControlEntry());
	}
	ZLResourceKey actionKey("action");
	MultiKeyOptionEntry *keyEntry = new MultiKeyOptionEntry(dialogTab.resource(actionKey));
	OrientationEntry *orientationEntry = new OrientationEntry(*keyEntry);
	ZLBooleanOptionEntry *useSeparateBindingsEntry = new UseSeparateOptionsEntry(*keyEntry, *orientationEntry);
	dialogTab.addOption(ZLResourceKey("separate"), useSeparateBindingsEntry);
	dialogTab.addOption(ZLResourceKey("orientation"), orientationEntry);
	dialogTab.addOption("", "", keyEntry);
	ZLOptionEntry *exitOnCancelEntry = new ZLSimpleBooleanOptionEntry(fbreader.QuitOnCancelOption);
	keyEntry->setExitOnCancelEntry(exitOnCancelEntry);
	dialogTab.addOption(ZLResourceKey("quitOnCancel"), exitOnCancelEntry);
	exitOnCancelEntry->setVisible(false);
	useSeparateBindingsEntry->onStateChanged(useSeparateBindingsEntry->initialState());
	dialogTab.addOption(ZLResourceKey("keyDelay"), new ZLSimpleSpinOptionEntry(fbreader.KeyDelayOption, 50));
}
