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

#include "ZLWin32ApplicationWindow.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

void ZLWin32ApplicationWindow::createWindowToolbar() {
  myRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN | RBS_VARHEIGHT | RBS_BANDBORDERS, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);

  myWindowToolbar.hwnd = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | BTNS_AUTOSIZE | CCS_NORESIZE, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);
	SendMessage(myWindowToolbar.hwnd, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
	SendMessage(myWindowToolbar.hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(myWindowToolbar.hwnd, TB_SETBITMAPSIZE, 0, MAKELONG(IconSize, IconSize));
	SendMessage(myWindowToolbar.hwnd, TB_SETINDENT, 3, 0);
	SendMessage(myWindowToolbar.hwnd, TB_SETIMAGELIST, 0, (LPARAM)ImageList_Create(IconSize, IconSize, ILC_COLOR32 | ILC_MASK, 0, 100));

	ZeroMemory(&myToolbarInfo, sizeof(myToolbarInfo));
	myToolbarInfo.cbSize = sizeof(myToolbarInfo);
	myToolbarInfo.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE | RBBIM_ID; 
	myToolbarInfo.fStyle = RBBS_USECHEVRON | RBBS_NOGRIPPER;
	myToolbarInfo.hwndChild = myWindowToolbar.hwnd;
	myToolbarInfo.wID = 1;
	myToolbarInfo.cxMinChild = 0;
	myToolbarInfo.cyIntegral = 1;
	myToolbarInfo.cyMinChild = 44;
	myToolbarInfo.cyMaxChild = 44;
	myToolbarInfo.cx = 0;
	myToolbarInfo.cxIdeal = 0;
	SendMessage(myRebar, RB_INSERTBAND, (WPARAM)0, (LPARAM)&myToolbarInfo);
	myToolbarInfo.fMask = RBBIM_SIZE | RBBIM_IDEALSIZE; 
}

void ZLWin32ApplicationWindow::updateWindowToolbarInfo() {
	if (myToolbarInfo.cxIdeal != 0) {
		return;
	}

	const int len = SendMessage(myWindowToolbar.hwnd, TB_BUTTONCOUNT, 0, 0);
	int lastVisibleIndex = -1;
	for (int index = 0; index < len; ++index) {
		TBBUTTON info;
		SendMessage(myWindowToolbar.hwnd, TB_GETBUTTON, index, (LPARAM)&info);
		if ((info.fsState & TBSTATE_HIDDEN) == 0) {
			lastVisibleIndex = index;
		}
	}
	if (lastVisibleIndex != -1) {
		RECT rect;
		SendMessage(myWindowToolbar.hwnd, TB_GETITEMRECT, lastVisibleIndex, (LPARAM)&rect);
		myToolbarInfo.cxIdeal = rect.right;
		myToolbarInfo.cx = rect.right;
	}
  
	SendMessage(myRebar, RB_SETBANDINFO, 0, (LPARAM)&myToolbarInfo);
}

void ZLWin32ApplicationWindow::processChevron(const NMREBARCHEVRON &chevron) {
	RECT toolbarRect;
	GetClientRect(myWindowToolbar.hwnd, &toolbarRect);

	HMENU popup = CreatePopupMenu();
	HIMAGELIST imageList = (HIMAGELIST)SendMessage(myWindowToolbar.hwnd, TB_GETIMAGELIST, 0, 0);
	int imageIndex = 0;

	const int len = SendMessage(myWindowToolbar.hwnd, TB_BUTTONCOUNT, 0, 0);
	int index = 0;
	for (; index < len; ++index) {
		TBBUTTON info;
		SendMessage(myWindowToolbar.hwnd, TB_GETBUTTON, index, (LPARAM)&info);
		if ((info.fsState & TBSTATE_HIDDEN) == 0) {
			RECT rect;
			SendMessage(myWindowToolbar.hwnd, TB_GETITEMRECT, index, (LPARAM)&rect);
			if (rect.right > toolbarRect.right) {
				break;
			}
		}
		if (info.idCommand > -100) {
			++imageIndex;
		}
	}

	ZLUnicodeUtil::Ucs2String buffer;
	MENUITEMINFO miInfo;
	miInfo.cbSize = sizeof(MENUITEMINFO);
	miInfo.wID = 1;
	int count = 0;
	for (; index < len; ++index) {
		TBBUTTON info;
		SendMessage(myWindowToolbar.hwnd, TB_GETBUTTON, index, (LPARAM)&info);
		if ((info.fsState & TBSTATE_HIDDEN) == 0) {
			ZLToolbar::ItemPtr item = myWindowToolbar.TBItemByActionCode[info.idCommand];
			if (!item.isNull()) {
				const ZLToolbar::AbstractButtonItem &button =
					(const ZLToolbar::AbstractButtonItem&)*item;
				miInfo.fMask = MIIM_STATE | MIIM_STRING | MIIM_ID | MIIM_BITMAP;
				miInfo.fType = MFT_STRING;
				miInfo.fState = (info.fsState & TBSTATE_ENABLED) ?
					MFS_ENABLED :
					(MFS_DISABLED | MFS_GRAYED);
				miInfo.dwTypeData =
					(WCHAR*)::wchar(::createNTWCHARString(buffer, "  " + button.tooltip()));
				miInfo.cch = buffer.size();
				miInfo.wID = info.idCommand;

				HICON hIcon = ImageList_GetIcon(imageList, imageIndex, ILD_NORMAL);
				HBITMAP hBitmap = myWindowToolbar.BitmapByIcon[hIcon];
				if (hBitmap == 0) {
					HDC hDC = ::CreateCompatibleDC(0);
					BYTE *array = new BYTE[4 * IconSize * IconSize];
					::memset(array, 0xFF, 4 * IconSize * IconSize);
					hBitmap = ::CreateBitmap(IconSize, IconSize, 4, 8, array);
					delete[] array;
					::SelectObject(hDC, hBitmap);
					::DrawIcon(hDC, 0, 0, hIcon);
					::DeleteDC(hDC);
					myWindowToolbar.BitmapByIcon[hIcon] = hBitmap;
				}
				miInfo.hbmpItem = hBitmap;

				InsertMenuItem(popup, count++, true, &miInfo);
			} else if (info.idCommand >= -200) /* is a separator */ {
				if (count > 0) {
					miInfo.fMask = MIIM_TYPE;
					miInfo.fType = MFT_SEPARATOR;
					InsertMenuItem(popup, count++, true, &miInfo);
				}
			}
		}
		if (info.idCommand > -100) {
			++imageIndex;
		}
	}

	POINT p;
	p.x = chevron.rc.right - 2;
	p.y = chevron.rc.bottom;
	ClientToScreen(myMainWindow, &p);
	const int code = TrackPopupMenu(popup, TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, p.x, p.y, 0, myMainWindow, 0);
	onToolbarButtonRelease(code);
	PostMessage(myMainWindow, WM_NULL, 0, 0);
	DestroyMenu(popup);
}
