/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "FBReader.h"
#include "KeyBindingsPage.h"

class KeyboardControlEntry : public ZLSimpleBooleanOptionEntry {

public:
  KeyboardControlEntry(FBReader &fbreader);
  void onValueChange(bool state);

private:
  FBReader &myFBReader;
};

KeyboardControlEntry::KeyboardControlEntry(FBReader &fbreader) : ZLSimpleBooleanOptionEntry("Grab System Keys", fbreader.KeyboardControlOption), myFBReader(fbreader) {
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

  void setOrientation(ZLViewWidget::Angle);

private:
  void addAction(ActionCode code, const std::string &name);
  std::map<std::string,ActionCode> &changedCodes();

private:
  FBReader &myFBReader;
  std::vector<ActionCode> myCodeByIndex;
  std::map<ActionCode,int> myIndexByCode;

  ZLViewWidget::Angle myOrientation;

  std::map<std::string,ActionCode> myChangedCodes0;
  std::map<std::string,ActionCode> myChangedCodes90;
  std::map<std::string,ActionCode> myChangedCodes180;
  std::map<std::string,ActionCode> myChangedCodes270;
};

void FBReaderKeyOptionEntry::addAction(ActionCode code, const std::string &name) {
  myIndexByCode[code] = myCodeByIndex.size();
  myCodeByIndex.push_back(code);
  addActionName(name);
}

FBReaderKeyOptionEntry::FBReaderKeyOptionEntry(FBReader &fbreader) : ZLKeyOptionEntry("Key settings"), myFBReader(fbreader), myOrientation(ZLViewWidget::DEGREES0) {
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
  addAction(ACTION_LARGE_SCROLL_FORWARD, "Large Scroll Forward");
  addAction(ACTION_LARGE_SCROLL_BACKWARD, "Large Scroll Backward");
  addAction(ACTION_SMALL_SCROLL_FORWARD, "Small Scroll Forward");
  addAction(ACTION_SMALL_SCROLL_BACKWARD, "Small Scroll Backward");
  addAction(ACTION_UNDO, "Undo");
  addAction(ACTION_REDO, "Redo");

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
  if (myFBReader.isRotationSupported()) {
    addAction(ACTION_ROTATE_SCREEN, "Rotate Screen");
  }

  // dialogs
  addAction(ACTION_SHOW_OPTIONS, "Show Options Dialog");
  addAction(ACTION_SHOW_BOOK_INFO, "Show Book Info Dialog");
  addAction(ACTION_ADD_BOOK, "Add Book");

  // quit
  addAction(ACTION_CANCEL, "Cancel");
  addAction(ACTION_QUIT, "Quit");

  setOrientation(ZLViewWidget::DEGREES0);
}

void FBReaderKeyOptionEntry::setOrientation(ZLViewWidget::Angle angle) {
  myOrientation = angle;
  reset();
}

std::map<std::string,ActionCode> &FBReaderKeyOptionEntry::changedCodes() {
  switch (myOrientation) {
    case ZLViewWidget::DEGREES0:
    default:
      return myChangedCodes0;
    case ZLViewWidget::DEGREES90:
      return myChangedCodes90;
    case ZLViewWidget::DEGREES180:
      return myChangedCodes180;
    case ZLViewWidget::DEGREES270:
      return myChangedCodes270;
  }
}

FBReaderKeyOptionEntry::~FBReaderKeyOptionEntry() {
}

void FBReaderKeyOptionEntry::onAccept() {
  for (std::map<std::string,ActionCode>::const_iterator it = myChangedCodes0.begin(); it != myChangedCodes0.end(); ++it) {
    myFBReader.keyBindings(ZLViewWidget::DEGREES0, true).bindKey(it->first, it->second);
  }
  for (std::map<std::string,ActionCode>::const_iterator it = myChangedCodes90.begin(); it != myChangedCodes90.end(); ++it) {
    myFBReader.keyBindings(ZLViewWidget::DEGREES90, true).bindKey(it->first, it->second);
  }
  for (std::map<std::string,ActionCode>::const_iterator it = myChangedCodes180.begin(); it != myChangedCodes180.end(); ++it) {
    myFBReader.keyBindings(ZLViewWidget::DEGREES180, true).bindKey(it->first, it->second);
  }
  for (std::map<std::string,ActionCode>::const_iterator it = myChangedCodes270.begin(); it != myChangedCodes270.end(); ++it) {
    myFBReader.keyBindings(ZLViewWidget::DEGREES270, true).bindKey(it->first, it->second);
  }
}

int FBReaderKeyOptionEntry::actionIndex(const std::string &key) {
  std::map<std::string,ActionCode>::const_iterator it = changedCodes().find(key);
  return myIndexByCode[(it != changedCodes().end()) ? it->second : myFBReader.keyBindings(myOrientation, true).getBinding(key)];
}

void FBReaderKeyOptionEntry::onValueChange(const std::string &key, int index) {
  changedCodes()[key] = myCodeByIndex[index];
}

class OrientationEntry : public ZLComboOptionEntry {

public:
  OrientationEntry(FBReaderKeyOptionEntry &keyEntry);
  const std::string &name() const;
  const std::string &initialValue() const;
  const std::vector<std::string> &values() const;
  void onValueChange(const std::string&);
  void onAccept(const std::string&);

private:
  FBReaderKeyOptionEntry &myKeyEntry;
};

OrientationEntry::OrientationEntry(FBReaderKeyOptionEntry &keyEntry) : myKeyEntry(keyEntry) {
}

const std::string &OrientationEntry::name() const {
  static std::string _name = "Orientation";
  return _name;
}

static const std::string ROTATION_0 = "0 Degrees";
static const std::string ROTATION_90 = "90 Degrees Counterclockwise";
static const std::string ROTATION_180 = "180 Degrees";
static const std::string ROTATION_270 = "90 Degrees Clockwise";

const std::string &OrientationEntry::initialValue() const {
  return ROTATION_0;
}

const std::vector<std::string> &OrientationEntry::values() const {
  static std::vector<std::string> _values;
  if (_values.empty()) {
    _values.push_back(ROTATION_0);
    _values.push_back(ROTATION_90);
    _values.push_back(ROTATION_180);
    _values.push_back(ROTATION_270);
  }
  return _values;
}

void OrientationEntry::onValueChange(const std::string &value) {
  if (value == ROTATION_0) {
    myKeyEntry.setOrientation(ZLViewWidget::DEGREES0);
  } else if (value == ROTATION_90) {
    myKeyEntry.setOrientation(ZLViewWidget::DEGREES90);
  } else if (value == ROTATION_180) {
    myKeyEntry.setOrientation(ZLViewWidget::DEGREES180);
  } else if (value == ROTATION_270) {
    myKeyEntry.setOrientation(ZLViewWidget::DEGREES270);
  }
}

void OrientationEntry::onAccept(const std::string&) {
}

class UseSeparateOptionsEntry : public ZLSimpleBooleanOptionEntry {

public:
  UseSeparateOptionsEntry(FBReader &fbreader, FBReaderKeyOptionEntry &keyEntry, OrientationEntry &orientationEntry);
  void onValueChange(bool state);

private:
  FBReaderKeyOptionEntry &myKeyEntry;
  OrientationEntry &myOrientationEntry;
};

UseSeparateOptionsEntry::UseSeparateOptionsEntry(FBReader &fbreader, FBReaderKeyOptionEntry &keyEntry, OrientationEntry &orientationEntry) : ZLSimpleBooleanOptionEntry("Keybindings Depend On Orientation", fbreader.useSeparateBindings()), myKeyEntry(keyEntry), myOrientationEntry(orientationEntry) {
}

void UseSeparateOptionsEntry::onValueChange(bool state) {
  ZLSimpleBooleanOptionEntry::onValueChange(state);
  myOrientationEntry.setVisible(state);
  myKeyEntry.reset();
}

KeyBindingsPage::KeyBindingsPage(FBReader &fbreader, ZLDialogContent &dialogTab) {
  if (fbreader.isFullKeyboardControlSupported()) {
    dialogTab.addOption(new KeyboardControlEntry(fbreader));
  }
  FBReaderKeyOptionEntry *keyEntry = new FBReaderKeyOptionEntry(fbreader);
  ZLBooleanOptionEntry *useSeparateBindingsEntry = 0;
  if (fbreader.isRotationSupported()) {
    OrientationEntry *orientationEntry = new OrientationEntry(*keyEntry);
    useSeparateBindingsEntry = new UseSeparateOptionsEntry(fbreader, *keyEntry, *orientationEntry);
    dialogTab.addOption(useSeparateBindingsEntry);
    dialogTab.addOption(orientationEntry);
  }
  dialogTab.addOption(keyEntry);
  if (useSeparateBindingsEntry != 0) {
    useSeparateBindingsEntry->onValueChange(useSeparateBindingsEntry->initialState());
  }
}
