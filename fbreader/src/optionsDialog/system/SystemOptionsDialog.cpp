/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLOptionsDialog.h>
#include <ZLLanguageList.h>
#include <ZLOptionEntry.h>

#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLLanguageOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "SystemOptionsDialog.h"

#include "../../fbreader/FBReader.h"
#include "../../formats/FormatPlugin.h"
#include "../../encodingOption/EncodingOptionEntry.h"


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


SystemOptionsDialog::SystemOptionsDialog() : AbstractOptionsDialog(ZLResourceKey("SystemOptionsDialog"), true) {
	ZLOptionsDialog &dialog = this->dialog();

	ZLDialogContent &encodingTab = dialog.createTab(ZLResourceKey("Language"));
	encodingTab.addOption(ZLResourceKey("autoDetect"), new ZLSimpleBooleanOptionEntry(PluginCollection::Instance().LanguageAutoDetectOption));
	encodingTab.addOption(ZLResourceKey("defaultLanguage"), new ZLLanguageOptionEntry(PluginCollection::Instance().DefaultLanguageOption, ZLLanguageList::languageCodes()));
	EncodingEntry *encodingEntry = new EncodingEntry(PluginCollection::Instance().DefaultEncodingOption);
	EncodingSetEntry *encodingSetEntry = new EncodingSetEntry(*encodingEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncodingSet"), encodingSetEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncoding"), encodingEntry);

	if (ZLOption::isAutoSavingSupported()) {
		ZLDialogContent &configTab = dialog.createTab(ZLResourceKey("Config"));
		FBReader &fbreader = FBReader::Instance();
		ZLToggleBooleanOptionEntry *enableEntry =
			new ZLToggleBooleanOptionEntry(fbreader.ConfigAutoSavingOption);
		configTab.addOption(ZLResourceKey("autoSave"), enableEntry);

		ZLOptionEntry *timeoutEntry = new TimeoutEntry(fbreader.ConfigAutoSaveTimeoutOption);
		enableEntry->addDependentEntry(timeoutEntry);
		configTab.addOption(ZLResourceKey("timeout"), timeoutEntry);

		enableEntry->onStateChanged(enableEntry->initialState());
	}

	FBReader &fbreader = FBReader::Instance();

	std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	ZLOptionEntry *entry =
		new ZLSimpleBooleanOptionEntry(fbreader.EnableSingleClickDictionaryOption);
	additional.push_back(std::make_pair(ZLResourceKey("singleClickOpen"), entry));
	createIntegrationTab(fbreader.dictionaryCollection(), ZLResourceKey("Dictionary"), additional);

	dialog.createPlatformDependentTabs();
}
