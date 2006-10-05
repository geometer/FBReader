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
#include "ConfigPage.h"

class EnableAutoSavingEntry : public ZLSimpleBooleanOptionEntry {

public:
	EnableAutoSavingEntry(ZLBooleanOption &option, ZLOptionEntry &timeoutEntry);
	void onValueChange(bool state);

private:
	ZLOptionEntry &myTimeoutEntry;
};

EnableAutoSavingEntry::EnableAutoSavingEntry(ZLBooleanOption &option, ZLOptionEntry &timeoutEntry) : ZLSimpleBooleanOptionEntry("Save State Automatically", option), myTimeoutEntry(timeoutEntry) {
}

class TimeoutEntry : public ZLSimpleSpinOptionEntry {

public:
	TimeoutEntry(ZLIntegerRangeOption &option);
	void onAccept(int value);
};

void EnableAutoSavingEntry::onValueChange(bool state) {
	myTimeoutEntry.setVisible(state);
}

TimeoutEntry::TimeoutEntry(ZLIntegerRangeOption &option) : ZLSimpleSpinOptionEntry("Timeout Between Savings, Seconds", option, 5) {
}

void TimeoutEntry::onAccept(int value) {
	ZLOption::startAutoSave(isVisible() ? value : 0);
	ZLSimpleSpinOptionEntry::onAccept(value);
}

ConfigPage::ConfigPage(FBReader &fbreader, ZLDialogContent &dialogTab) {
	ZLOptionEntry *timeoutEntry = new TimeoutEntry(fbreader.ConfigAutoSaveTimeoutOption);
	ZLBooleanOptionEntry *enableEntry = new EnableAutoSavingEntry(fbreader.ConfigAutoSavingOption, *timeoutEntry);
	dialogTab.addOption(enableEntry);
	dialogTab.addOption(timeoutEntry);
	enableEntry->onValueChange(enableEntry->initialState());
}
