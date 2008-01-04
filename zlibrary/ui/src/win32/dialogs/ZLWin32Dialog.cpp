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

#include <ZLDialogManager.h>

#include "ZLWin32Dialog.h"
#include "ZLWin32DialogContent.h"
#include "../application/ZLWin32ApplicationWindow.h"

ZLWin32Dialog::ZLWin32Dialog(ZLWin32ApplicationWindow &window, const ZLResource &resource) : myWindow(window), myPanel(myWindow.mainWindow(), resource[ZLDialogManager::DIALOG_TITLE].value()) {
	W32VBox *panelBox = new W32VBox();
	myPanel.setElement(panelBox);

	ZLWin32DialogContent *contentTab = new ZLWin32DialogContent(resource);
	myTab = contentTab;
	panelBox->addElement(contentTab->contentPtr());

	W32Table &table = contentTab->contentTable();
	table.setSpacings(4, 8);
	table.setMargins(4, 4, 4, 4);

	myButtonBox = new W32HBox();
	panelBox->addElement(myButtonBox);
	myButtonBox->setHomogeneous(true);
	myButtonBox->setAlignment(W32HBox::RIGHT);
	myButtonBox->setSpacing(4);
	myButtonBox->setMargins(4, 4, 4, 4);
}

void ZLWin32Dialog::addButton(const ZLResourceKey &key, bool accept) {
	W32WidgetPtr button = new W32PushButton(ZLDialogManager::buttonName(key), accept ? W32PushButton::OK_BUTTON : W32PushButton::CANCEL_BUTTON);
	button->setVisible(true);
	myButtonBox->addElement(button);
}

bool ZLWin32Dialog::run() {
	myWindow.blockMouseEvents(true);
	bool result = myPanel.runDialog();
	myWindow.blockMouseEvents(false);
	return result;
}
