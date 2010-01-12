/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32POPUPMENU_H__
#define __ZLWIN32POPUPMENU_H__

#include <vector>
#include <string>

#include <windows.h>

#include <ZLUnicodeUtil.h>

class ZLWin32PopupMenu {

private:
	static HFONT menuFont();
	static int ourFontSize;

public:
	ZLWin32PopupMenu(HWND mainWindow);
	~ZLWin32PopupMenu();

	int run(POINT pt, bool right = false);

	void measureItem(MEASUREITEMSTRUCT &mi);
	void drawItem(DRAWITEMSTRUCT &di);

	void addSeparator();
	void addItem(const std::string &str, HICON icon, bool active, int id = -1);

private:
	WCHAR *textData(int index);
	size_t textSize(int index);
	
private:
	HWND myMainWindow;
	HMENU myHMenu;
	std::vector<ZLUnicodeUtil::Ucs2String> myStrings;
	std::vector<HICON> myIcons;
	std::vector<int> myCodes;
	bool myShowIcons;
	int myItemCount;
};

#endif /* __ZLWIN32POPUPMENU_H__ */
