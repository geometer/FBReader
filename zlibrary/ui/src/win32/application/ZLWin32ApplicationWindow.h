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

#ifndef __ZLWIN32APPLICATIONWINDOW_H__
#define __ZLWIN32APPLICATIONWINDOW_H__

#include <map>

#include <windows.h>
#include <commctrl.h>

#include <ZLUnicodeUtil.h>

#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"

class ZLWin32ViewWidget;
class ZLWin32PopupMenu;

class ZLWin32ApplicationWindow : public ZLDesktopApplicationWindow { 

private:
	static const int IconSize;

	static LRESULT CALLBACK Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static ZLWin32ApplicationWindow *ourApplicationWindow;

public:
	ZLWin32ApplicationWindow(ZLApplication *application);
	~ZLWin32ApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void init();
	void refresh();
	void processAllEvents();

	void createWindowToolbar();
	void createFloatingToolbar();
	void destroyFloatingToolbar();
	void registerFloatingToolbarClass();

	void addToolbarItem(ZLToolbar::ItemPtr item);
	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

	void updateParameters();
	void updateWindowToolbarInfo();
	void updateFullscreenToolbarSize();
	void setTooltip(TOOLTIPTEXT &tooltip);
	void runPopup(const NMTOOLBAR &nmToolbar);
	void processChevron(const NMREBARCHEVRON &chevron);

public:
	HWND mainWindow() const;
	void blockMouseEvents(bool block);
	bool mouseEventsAreBlocked() const;
	void setWait(bool wait);
	void resetFocus();
	void updateCursor() const;

private:
	LRESULT mainLoopCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	void onToolbarButtonRelease(int index);

private:
	class TextEditParameter : public VisualParameter {

	private:
		typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK ComboBoxCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK TextEditCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		TextEditParameter(ZLApplication &application, HWND mainWindow, HWND toolbar, int idCommand, const ZLToolbar::ParameterItem &item);
		HWND handle() const;

	private:
		std::string internalValue() const;
		void internalSetValue(const std::string &value);
		void setValueList(const std::vector<std::string> &values);

	private:
		ZLApplication &myApplication;
		HWND myMainWindow;
		HWND myComboBox;
		WndProc myOriginalComboBoxCallback;
		WndProc myOriginalTextEditCallback;
		const ZLToolbar::ParameterItem &myParameterItem;
	};

private:
	ZLUnicodeUtil::Ucs2String myClassName;

	struct Toolbar {
		Toolbar() : hwnd(0) {
		}
		void clear();

		HWND hwnd;
		std::map<ZLToolbar::ItemPtr,int> SeparatorNumbers;
		std::map<std::string,int> ActionCodeById;
		std::map<std::string,int> ParameterCodeById;
		std::map<int,ZLToolbar::ItemPtr> TBItemByActionCode;
		std::map<HICON,HBITMAP> BitmapByIcon;
	};

	HWND myMainWindow;
	HWND myRebar;
	Toolbar myWindowToolbar;
	HWND myDockWindow;
	Toolbar myFullscreenToolbar;
	bool myFloatingToolbarClassRegistered;
	Toolbar &toolbar(ToolbarType type) {
		return (type == WINDOW_TOOLBAR) ? myWindowToolbar : myFullscreenToolbar;
	}
	REBARBANDINFO myToolbarInfo;
	std::vector<ZLToolbar::ItemPtr> myFloatingToolbarItems;

	std::map<int,HWND> myParameters;

	ZLWin32ViewWidget *myWin32ViewWidget;

	bool myBlockMouseEvents;
	int myKeyboardModifierMask;

	bool myFullScreen;
	bool myWait;

	HCURSOR myCursor;

	shared_ptr<ZLWin32PopupMenu> myPopupMenu;

friend class ZLWin32TimeManager;
};

#endif /* __ZLWIN32APPLICATIONWINDOW_H__ */
