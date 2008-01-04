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

#include "ZLWin32MessageBox.h"
#include "../application/ZLWin32ApplicationWindow.h"

ZLWin32MessageBox::ZLWin32MessageBox(ZLWin32ApplicationWindow &window, W32StandardIcon::IconId iconId, const std::string &title, const std::string &message) : myWindow(window), myPanel(myWindow.mainWindow(), title), myReturnCode(-1) {
	W32HBox *panelBox = new W32HBox();
	panelBox->setMargins(4, 4, 8, 8);
	myPanel.setElement(panelBox);

	W32VBox *imageBox = new W32VBox();
	//imageBox->setAlignment(W32VBox::CENTER);
	imageBox->setMargins(8, 8, 0, 0);
	imageBox->addElement(new W32StandardIcon(iconId));
	panelBox->addElement(imageBox);

	W32VBox *textBox = new W32VBox();
	textBox->setSpacing(4);
	textBox->addElement(new W32Label(message, W32Label::ALIGN_CENTER));
	panelBox->addElement(textBox);

	myButtonBox = new W32HBox();
	textBox->addElement(myButtonBox);
	myButtonBox->setHomogeneous(true);
	myButtonBox->setAlignment(W32HBox::CENTER);
	myButtonBox->setSpacing(4);
	myButtonBox->setMargins(0, 0, 8, 8);

	panelBox->setVisible(true);
}

void ZLWin32MessageBox::addButton(const std::string &text) {
	W32WidgetPtr button = new W32PushButton(text);
	myButtons.push_back(button);
	((W32Control&)*button).addListener(this);
	button->setVisible(true);
	myButtonBox->addElement(button);
	if (myButtons.size() > 1) {
		myPanel.setExitOnOk(false);
	}
}

int ZLWin32MessageBox::run() {
	myWindow.blockMouseEvents(true);
	myPanel.runDialog();
	myWindow.blockMouseEvents(false);
	return myReturnCode;
}

void ZLWin32MessageBox::onEvent(const std::string &event, W32EventSender &sender) {
	if (event == W32PushButton::RELEASED_EVENT) {
		for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
			if ((const W32EventSender*)(const W32Control*)&**it == &sender) {
				myReturnCode = it - myButtons.begin();
				myPanel.endDialog(true);
				break;
			}
		}
	}
}
