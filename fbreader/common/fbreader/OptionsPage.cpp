/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>

#include "OptionsPage.h"

void OptionsPage::ComboOptionEntry::onValueChange(const std::string &selectedValue) {
	for (std::map<ZLOptionEntry*,std::string>::const_iterator it = myPage.myEntries.begin(); it != myPage.myEntries.end(); it++) {
		if (it->second == selectedValue) {
			it->first->show();
		} else {
			it->first->hide();
		}
	}
}

void OptionsPage::registerEntry(ZLOptionsDialogTab *tab, ZLOptionEntry *entry, const std::string &name) {
	if (entry != 0) {
		entry->hide();
		myEntries[entry] = name;
	}
	tab->addOption(entry);
}

void OptionsPage::registerEntries(ZLOptionsDialogTab *tab, ZLOptionEntry *entry0, ZLOptionEntry *entry1, const std::string &name) {
	if (entry0 != 0) {
		entry0->hide();
		myEntries[entry0] = name;
	}
	if (entry1 != 0) {
		entry1->hide();
		myEntries[entry1] = name;
	}
	tab->addOptions(entry0, entry1);
}
