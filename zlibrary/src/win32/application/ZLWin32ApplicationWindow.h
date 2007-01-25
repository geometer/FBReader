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

#ifndef __ZLWIN32APPLICATIONWINDOW_H__
#define __ZLWIN32APPLICATIONWINDOW_H__

#include <map>

#include <windows.h>
#include <commctrl.h>

#include <ZLApplication.h>

class ZLWin32ViewWidget;

class ZLWin32ApplicationWindow : public ZLApplicationWindow { 

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static ZLWin32ApplicationWindow *ourApplicationWindow;

	static int x(WPARAM lParam);
	static int y(WPARAM lParam);

public:
	ZLWin32ApplicationWindow(ZLApplication *application);
	~ZLWin32ApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void refresh();
	void close();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;
	bool isMousePresented() const;
	bool isKeyboardPresented() const;

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void init();

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
	void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);

public:
	HWND mainWindow() const;
	int topOffset() const;
	void blockMouseEvents(bool block);

private:
	HBITMAP maskBitmap(HBITMAP original);
	HBITMAP grayBitmap(HBITMAP original);
	HBITMAP ditheredBitmap(HBITMAP original);

public:
	void onToolbarButtonPress(int index);
/*
	void handleKeyEventSlot(GdkEventKey *event);
	void handleScrollEventSlot(GdkEventScroll *event);
*/

private:
	ZLIntegerRangeOption myWidthOption;
	ZLIntegerRangeOption myHeightOption;

	HWND myMainWindow;
	HWND myToolbar;

	ZLWin32ViewWidget *myWin32ViewWidget;

	std::map<int,ZLApplication::Toolbar::ItemPtr> myButtonByActionCode;
	//bool myFullScreen;

	bool myBlockMouseEvents;
	int myKeyboardModifierMask;
};

#endif /* __ZLWIN32APPLICATIONWINDOW_H__ */
