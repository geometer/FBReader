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

#include "ZLWin32PopupMenu.h"

#include "../../../../core/src/win32/util/W32WCHARUtil.h"

int ZLWin32PopupMenu::ourFontSize;

HFONT ZLWin32PopupMenu::menuFont() {
	static HFONT font = 0;
	if (font == 0) {
		NONCLIENTMETRICS metrics;
		ZeroMemory(&metrics, sizeof(metrics));
		metrics.cbSize = sizeof(metrics);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
		ourFontSize = metrics.lfMenuFont.lfHeight * (-3) / 2;
		metrics.lfMenuFont.lfHeight = ourFontSize;
		metrics.lfMenuFont.lfQuality = 5;
		/*
		LOGFONT logicalFont;
		ZeroMemory(&logicalFont, sizeof(logicalFont));
		logicalFont.lfHeight = ourFontSize;
		logicalFont.lfWeight = FW_REGULAR;
		logicalFont.lfItalic = false;
		logicalFont.lfQuality = 5;
		font = CreateFontIndirect(&logicalFont);
		*/
		font = CreateFontIndirect(&metrics.lfMenuFont);
	}
	return font;
}

ZLWin32PopupMenu::ZLWin32PopupMenu(HWND mainWindow) : myMainWindow(mainWindow) {
	myHMenu = CreatePopupMenu();
	myItemCount = 0;
	myShowIcons = false;
}

ZLWin32PopupMenu::~ZLWin32PopupMenu() {
	DestroyMenu(myHMenu);
}

int ZLWin32PopupMenu::run(POINT pt, bool right) {
	int num = TrackPopupMenu(
		myHMenu,
		(right ? TPM_RIGHTALIGN : TPM_LEFTALIGN) | TPM_TOPALIGN | TPM_RETURNCMD,
		pt.x, pt.y, 0, myMainWindow, 0
	);
	PostMessage(myMainWindow, WM_NULL, 0, 0);
	return (num > 0) ? myCodes[num - 1] : -1;
}

void ZLWin32PopupMenu::measureItem(MEASUREITEMSTRUCT &mi) {
	HDC dc = ::GetDC(myMainWindow);
	HFONT font = menuFont();
	HFONT oldFont = (HFONT)SelectObject(dc, font);
	if (oldFont != font) {
		DeleteObject(oldFont);
	}
	SIZE size;
	const int index = mi.itemID - 1;
	GetTextExtentPointW(dc, textData(index), textSize(index), &size);
	mi.itemWidth = size.cx + 20;
	if (myShowIcons) {
		mi.itemWidth += 42;
	}
	::ReleaseDC(myMainWindow, dc);
	mi.itemHeight = ourFontSize * 4 / 3;
	if (myShowIcons && (mi.itemHeight < 36)) {
		mi.itemHeight = 36;
	}
}

void ZLWin32PopupMenu::drawItem(DRAWITEMSTRUCT &di) {
	HFONT font = menuFont();
	HFONT oldFont = (HFONT)SelectObject(di.hDC, font);
	if (oldFont != font) {
		DeleteObject(oldFont);
	}

	if (di.itemState & ODS_DISABLED) {
		static COLORREF whiteColor = GetSysColor(COLOR_MENU);
		static HBRUSH whiteBrush = CreateSolidBrush(whiteColor);
		if (whiteColor != GetSysColor(COLOR_MENU)) {
			whiteColor = GetSysColor(COLOR_MENU);
			whiteBrush = CreateSolidBrush(whiteColor);
		}
		FillRect(di.hDC, &di.rcItem, whiteBrush);
		SetTextColor(di.hDC, GetSysColor(COLOR_GRAYTEXT));
	} else if (di.itemState & ODS_SELECTED) {
		static COLORREF blueColor = GetSysColor(COLOR_HIGHLIGHT);
		static HBRUSH blueBrush = CreateSolidBrush(blueColor);
		if (blueColor != GetSysColor(COLOR_HIGHLIGHT)) {
			blueColor = GetSysColor(COLOR_HIGHLIGHT);
			blueBrush = CreateSolidBrush(blueColor);
		}
		FillRect(di.hDC, &di.rcItem, blueBrush);
		SetTextColor(di.hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
	} else {
		static COLORREF whiteColor = GetSysColor(COLOR_MENU);
		static HBRUSH whiteBrush = CreateSolidBrush(whiteColor);
		if (whiteColor != GetSysColor(COLOR_MENU)) {
			whiteColor = GetSysColor(COLOR_MENU);
			whiteBrush = CreateSolidBrush(whiteColor);
		}
		FillRect(di.hDC, &di.rcItem, whiteBrush);
		SetTextColor(di.hDC, GetSysColor(COLOR_MENUTEXT));
	}
	SetBkMode(di.hDC, TRANSPARENT);
	const int index = di.itemID - 1;
	int itemHeight = ourFontSize * 4 / 3;
	if (myShowIcons && (itemHeight < 36)) {
		itemHeight = 36;
	}
	HICON icon = myIcons[index];
	if (icon != 0) {
		DrawIcon(di.hDC, di.rcItem.left + 10, di.rcItem.top + (itemHeight - 32) / 2, icon);
	}
	const int left = myShowIcons ? di.rcItem.left + 52 : di.rcItem.left + 10;
	TextOutW(di.hDC, left, di.rcItem.top + (itemHeight - ourFontSize) / 2, textData(index), textSize(index));
}

void ZLWin32PopupMenu::addItem(const std::string &str, HICON icon, bool active, int id) {
	MENUITEMINFO miInfo;
	miInfo.cbSize = sizeof(MENUITEMINFO);
	miInfo.fMask = MIIM_STATE | MIIM_ID | MIIM_FTYPE;
	miInfo.fType = MFT_OWNERDRAW;
	miInfo.fState = active ? MFS_ENABLED : MFS_DISABLED;
	miInfo.wID = myItemCount + 1;
	InsertMenuItem(myHMenu, myItemCount++, true, &miInfo);
	myStrings.push_back(ZLUnicodeUtil::Ucs2String());
	myIcons.push_back(icon);
	if (icon != 0) {
		myShowIcons = true;
	}
	::createNTWCHARString(myStrings.back(), str);
	myCodes.push_back((id != -1) ? id : myItemCount);
}

void ZLWin32PopupMenu::addSeparator() {
	if (myItemCount > 0) {
		MENUITEMINFO miInfo;
		miInfo.cbSize = sizeof(MENUITEMINFO);
		miInfo.fMask = MIIM_FTYPE;
		miInfo.fType = MFT_SEPARATOR;
		myStrings.push_back(ZLUnicodeUtil::Ucs2String());
		myIcons.push_back(0);
		InsertMenuItem(myHMenu, myItemCount++, true, &miInfo);
		myCodes.push_back(-1);
	}
}

WCHAR *ZLWin32PopupMenu::textData(int index) {
	return (WCHAR*)::wchar(myStrings[index]);
}

size_t ZLWin32PopupMenu::textSize(int index) {
	return myStrings[index].size() - 1;
}
