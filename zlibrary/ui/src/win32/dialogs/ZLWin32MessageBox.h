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

#ifndef __ZLWIN32MESSAGEBOX_H__
#define __ZLWIN32MESSAGEBOX_H__

#include <string>

#include "../w32widgets/W32DialogPanel.h"
#include "../w32widgets/W32Container.h"
#include "../w32widgets/W32Control.h"

class ZLWin32ApplicationWindow;

class ZLWin32MessageBox : public W32Listener {

public:
	ZLWin32MessageBox(ZLWin32ApplicationWindow &window, W32StandardIcon::IconId iconId, const std::string &title, const std::string &message);

	void addButton(const std::string &text);
	int run();

	void onEvent(const std::string &event, W32EventSender &sender);

private:
	ZLWin32ApplicationWindow &myWindow;
	W32StandaloneDialogPanel myPanel;
	W32HBox *myButtonBox;
	W32WidgetList myButtons;
	int myReturnCode;

private:
	ZLWin32MessageBox(const ZLWin32MessageBox&);
	const ZLWin32MessageBox &operator = (const ZLWin32MessageBox&);
};

#endif /* __ZLWIN32MESSAGEBOX_H__ */
