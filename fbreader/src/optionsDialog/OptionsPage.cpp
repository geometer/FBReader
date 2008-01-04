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

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>

#include "OptionsPage.h"

void ComboOptionEntry::onValueSelected(int index) {
	const std::string &selectedValue = values()[index];
	const std::map<ZLOptionEntry*,std::string> &entries = myPage.myEntries;
	int count = 0;
	for (std::map<ZLOptionEntry*,std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it, ++count) {
		it->first->setVisible(it->second == selectedValue);
	}
}

void OptionsPage::registerEntry(ZLDialogContent &tab, const ZLResourceKey &entryKey, ZLOptionEntry *entry, const std::string &name) {
	if (entry != 0) {
		entry->setVisible(false);
		myEntries[entry] = name;
	}
	tab.addOption(entryKey, entry);
}

void OptionsPage::registerEntries(ZLDialogContent &tab, const ZLResourceKey &entry0Key, ZLOptionEntry *entry0, const ZLResourceKey &entry1Key, ZLOptionEntry *entry1, const std::string &name) {
	if (entry0 != 0) {
		entry0->setVisible(false);
		myEntries[entry0] = name;
	}
	if (entry1 != 0) {
		entry1->setVisible(false);
		myEntries[entry1] = name;
	}
	tab.addOptions(entry0Key, entry0, entry1Key, entry1);
}
