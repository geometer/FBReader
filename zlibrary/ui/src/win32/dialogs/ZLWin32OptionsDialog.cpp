/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLWin32OptionsDialog.h"
#include "ZLWin32DialogContent.h"
#include "../application/ZLWin32ApplicationWindow.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(ZLWin32ApplicationWindow &window, const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) : ZLOptionsDialog(resource, applyAction), myWindow(window), myPropertySheet(window.mainWindow(), caption(), showApplyButton) {
}

ZLDialogContent &ZLWin32OptionsDialog::createTab(const ZLResourceKey &key) {
	ZLWin32DialogContent *tab = new ZLWin32DialogContent(tabResource(key));
	myTabs.push_back(tab);

	W32DialogPanel &panel = myPropertySheet.createPanel(tab->displayName());
	myPanelToKeyMap[&panel] = tab->key();
	panel.addListener(this);
	panel.setElement(tab->contentPtr());

	W32Table &table = tab->contentTable();
	table.setMargins(4, 4, 4, 4);
	table.setSpacings(4, 8);

	return *tab;
}

const std::string &ZLWin32OptionsDialog::selectedTabKey() const {
	return mySelectedTabKey;
}

void ZLWin32OptionsDialog::selectTab(const ZLResourceKey &key) {
	mySelectedTabKey = key.Name;
}

void ZLWin32OptionsDialog::onEvent(const std::string &event, W32EventSender &sender) {
	if (event == W32DialogPanel::PANEL_SELECTED_EVENT) {
		mySelectedTabKey = myPanelToKeyMap[&(W32DialogPanel&)sender];
	} else if (event == W32DialogPanel::APPLY_BUTTON_PRESSED_EVENT) {
		accept();
	}
}

bool ZLWin32OptionsDialog::runInternal() {
	myWindow.blockMouseEvents(true);
	bool code = myPropertySheet.run(tabResource(ZLResourceKey(mySelectedTabKey)).value());
	myWindow.blockMouseEvents(false);
	return code;
}
