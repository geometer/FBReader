/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLOptionEntry.h>
#include <ZLOptionsDialog.h>

#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "ConfigPage.h"

#include "../fbreader/FBReader.h"

class TimeoutEntry : public ZLSimpleSpinOptionEntry {

public:
	TimeoutEntry(ZLIntegerRangeOption &option);
	void onAccept(int value);
};

TimeoutEntry::TimeoutEntry(ZLIntegerRangeOption &option) : ZLSimpleSpinOptionEntry(option, 5) {
}

void TimeoutEntry::onAccept(int value) {
	ZLOption::startAutoSave(isVisible() ? value : 0);
	ZLSimpleSpinOptionEntry::onAccept(value);
}

ConfigPage::ConfigPage(FBReader &fbreader, ZLDialogContent &dialogTab) {
	ZLToggleBooleanOptionEntry *enableEntry =
		new ZLToggleBooleanOptionEntry(fbreader.ConfigAutoSavingOption);
	dialogTab.addOption(ZLResourceKey("autoSave"), enableEntry);

	ZLOptionEntry *timeoutEntry = new TimeoutEntry(fbreader.ConfigAutoSaveTimeoutOption);
	enableEntry->addDependentEntry(timeoutEntry);
	dialogTab.addOption(ZLResourceKey("timeout"), timeoutEntry);

	enableEntry->onStateChanged(enableEntry->initialState());
}
