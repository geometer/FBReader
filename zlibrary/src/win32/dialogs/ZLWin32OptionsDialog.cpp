/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <windows.h>
#include <prsht.h>

#include "ZLWin32OptionsDialog.h"
#include "ZLWin32DialogContent.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(HWND mainWindow, const std::string &id, const std::string &caption) : ZLOptionsDialog(id), myPropertySheet(mainWindow, caption) {
}

ZLDialogContent &ZLWin32OptionsDialog::createTab(const std::string &name) {
	shared_ptr<ZLWin32DialogContent> tab = new ZLWin32DialogContent();
	myTabs.push_back(tab);

	W32DialogPanel &panel = myPropertySheet.createPanel(name);
	panel.setListener(this);
	panel.setElement(tab->contentPtr());
	W32Table &table = tab->contentTable();
	const int charHeight = panel.charDimension().Height;
	table.setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
	table.setSpacings(charHeight / 2, charHeight);

	return *tab;
}

const std::string &ZLWin32OptionsDialog::selectedTabName() const {
	return mySelectedTabName;
}

void ZLWin32OptionsDialog::selectTab(const std::string &name) {
	mySelectedTabName = name;
}

void ZLWin32OptionsDialog::onEvent(const std::string &event, W32EventSender &sender) {
	mySelectedTabName = ((W32DialogPanel&)sender).caption();
}

bool ZLWin32OptionsDialog::run() {
	bool result = myPropertySheet.run(mySelectedTabName);
	if (result) {
		for (std::vector<shared_ptr<ZLWin32DialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
			(*it)->accept();
		}
	}
	return result;
}
