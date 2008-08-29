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

#include <windowsx.h>

#define RBN_CHEVRONPUSHED (RBN_FIRST - 10)

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLOptionEntry.h>
#include <ZLDialog.h>
#include <ZLPaintContext.h>
#include <ZLPopupData.h>

#include "../../../../core/src/util/ZLKeyUtil.h"
#include "ZLWin32ApplicationWindow.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../view/ZLWin32ViewWidget.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"
#include "../time/ZLWin32Time.h"

const int ZLWin32ApplicationWindow::IconSize = 32;

ZLWin32ApplicationWindow *ZLWin32ApplicationWindow::ourApplicationWindow = 0;

LRESULT CALLBACK ZLWin32ApplicationWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ourApplicationWindow->mainLoopCallback(hWnd, uMsg, wParam, lParam);
}

LRESULT ZLWin32ApplicationWindow::mainLoopCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
		{
			LRESULT code = DefWindowProc(hWnd, uMsg, wParam, lParam);
			updateWindowToolbarInfo();
			updateFullscreenToolbarSize();
			return code;
		}
		case WM_TIMER:
			((ZLWin32TimeManager&)ZLTimeManager::instance()).execute(wParam);
			return 0;
		case WM_CREATE:
			myMainWindow = hWnd;
			ZLApplicationWindow::init();
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_KEYDOWN:
			if (wParam == 0x10) {
				myKeyboardModifierMask |= 0x1;
			} else if (wParam == 0x11) {
				myKeyboardModifierMask |= 0x2;
			} else if (wParam == 0x12) {
				myKeyboardModifierMask |= 0x4;
			} else {
				application().doActionByKey(ZLKeyUtil::keyName(wParam, wParam, myKeyboardModifierMask));
				myKeyboardModifierMask = 0;
			}
			return 0;
		case WM_KEYUP:
			if (wParam == 0x10) {
				myKeyboardModifierMask &= ~0x1;
			} else if (wParam == 0x11) {
				myKeyboardModifierMask &= ~0x2;
			} else if (wParam == 0x12) {
				myKeyboardModifierMask &= ~0x4;
			}
			return 0;
		case WM_SIZE:
		{
			int offset = 0;
			if (!myFullScreen) {
				//SendMessage(myToolbar, TB_AUTOSIZE, 0, 0);

				RECT rebarRect;
				GetWindowRect(myRebar, &rebarRect);
				offset = rebarRect.bottom - rebarRect.top - 1;

				MoveWindow(myRebar, 0, 0, LOWORD(lParam), rebarRect.bottom - rebarRect.top, true);
			}
			MoveWindow(myWin32ViewWidget->handle(), 0, offset, LOWORD(lParam), HIWORD(lParam) - offset, true);
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case WM_CLOSE:
			if (myFullScreen) {
				myWindowStateOption.setValue(FULLSCREEN);
			} else if (IsMaximized(myMainWindow)) {
				myWindowStateOption.setValue(MAXIMIZED);
			} else {
				myWindowStateOption.setValue(NORMAL);
				RECT rectangle;
				GetWindowRect(myMainWindow, &rectangle);
				myXOption.setValue(rectangle.left);
				myYOption.setValue(rectangle.top);
				myWidthOption.setValue(rectangle.right - rectangle.left + 1);
				myHeightOption.setValue(rectangle.bottom - rectangle.top + 1);
			}
			application().closeView();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			onToolbarButtonPress(LOWORD(wParam));
			return 0;
		case WM_NOTIFY:
		{
			const NMHDR *notificationHeader = (const NMHDR*)lParam;
			switch (notificationHeader->code) {
				case TTN_NEEDTEXT:
					setTooltip(*(TOOLTIPTEXT*)lParam);
					break;
				case NM_CUSTOMDRAW:
					updateTextFields();
					break;
				case TBN_DROPDOWN:
					runPopup(*(const NMTOOLBAR*)lParam);
					break;
				case RBN_CHEVRONPUSHED:
					processChevron(*(const NMREBARCHEVRON*)lParam);
					break;
			}
			return 0;
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void ZLWin32ApplicationWindow::setTooltip(TOOLTIPTEXT &tooltip) {
	Toolbar &tb = toolbar(isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR);
	ZLToolbar::ItemPtr item = tb.TBItemByActionCode[tooltip.hdr.idFrom];
	if (!item.isNull()) {
		const ZLToolbar::AbstractButtonItem &button =
			(const ZLToolbar::AbstractButtonItem&)*item;
		ZLUnicodeUtil::Ucs2String tooltipBuffer;
		::createNTWCHARString(tooltipBuffer, button.tooltip());
		size_t length = std::max(tooltipBuffer.size(), (size_t)80);
		memcpy((char*)tooltip.szText, (char*)::wchar(tooltipBuffer), 2 * length);
		tooltip.uFlags = TTF_DI_SETITEM;
	}
}

void ZLWin32ApplicationWindow::runPopup(const NMTOOLBAR &nmToolbar) {
	Toolbar &tb = toolbar(isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR);
	ZLToolbar::ItemPtr item = tb.TBItemByActionCode[nmToolbar.iItem];
	if (!item.isNull()) {
		const ZLToolbar::MenuButtonItem &button =
			(const ZLToolbar::MenuButtonItem&)*item;
		shared_ptr<ZLPopupData> popupData = button.popupData();
		if (!popupData.isNull()) {
			const int count = popupData->count();
			if (count != 0) {
				HMENU popup = CreatePopupMenu();
				ZLUnicodeUtil::Ucs2String buffer;
				MENUITEMINFO miInfo;
				miInfo.cbSize = sizeof(MENUITEMINFO);
				miInfo.fMask = MIIM_STATE | MIIM_STRING | MIIM_ID;
				miInfo.fType = MFT_STRING;
				miInfo.fState = MFS_ENABLED;
				for (int i = 0; i < count; ++i) {
					miInfo.wID = i + 1;
					const std::string text = popupData->text(i);
					miInfo.dwTypeData = (WCHAR*)::wchar(::createNTWCHARString(buffer, text));
					miInfo.cch = buffer.size();
					InsertMenuItem(popup, i, true, &miInfo);
				}
				POINT p;
				p.x = nmToolbar.rcButton.left;
				p.y = nmToolbar.rcButton.bottom + 6;
				ClientToScreen(myMainWindow, &p);
				int code = TrackPopupMenu(popup, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, p.x, p.y, 0, myMainWindow, 0);
				if (code > 0) {
					popupData->run(code - 1);
				}
				PostMessage(myMainWindow, WM_NULL, 0, 0);
				DestroyMenu(popup);
			}
		}
	}
}

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myMainWindow(0),
	myRebar(0),
	myDockWindow(0),
	myBlockMouseEvents(false),
	myKeyboardModifierMask(0),
	myFullScreen(false),
	myWait(false),
	myCursor(0) {

	::createNTWCHARString(myClassName, ZLibrary::ApplicationName());

	HWND existingWindow = FindWindow(::wchar(myClassName), 0);
	if (existingWindow != 0) {
		ShowWindow(existingWindow, SW_SHOW);
		SetForegroundWindow(existingWindow);
		exit(0);
	}

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(wc.hInstance, TEXT("ApplicationIcon"));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = ::wchar(myClassName);
	wc.hIconSm = wc.hIcon;

	RegisterClassEx(&wc);

	ourApplicationWindow = this;
}

void ZLWin32ApplicationWindow::init() {
	const WCHAR *aName = ::wchar(myClassName);
	CreateWindow(aName, aName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, myXOption.value(), myYOption.value(), myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, GetModuleHandle(0), 0);

	// TODO: Hmm, replace SW_SHOWDEFAULT by nCmdShow?
	ShowWindow(myMainWindow, SW_SHOWDEFAULT);

	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			setFullscreen(true);
			break;
		case MAXIMIZED:
			ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
			break;
	}
}

void ZLWin32ApplicationWindow::refresh() {
	ZLDesktopApplicationWindow::refresh();

	if (!isFullscreen()) {
		myToolbarInfo.cxIdeal = 0;
	}
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	ourApplicationWindow = 0;
}

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	Toolbar &tb = toolbar(type(button));

	PostMessage(tb.hwnd, TB_CHECKBUTTON, tb.ActionCodeById[button.actionId()], button.isPressed());
}

void ZLWin32ApplicationWindow::onToolbarButtonPress(int actionCode) {
	Toolbar &tb = toolbar(isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR);
	std::map<int,ZLToolbar::ItemPtr>::const_iterator it = tb.TBItemByActionCode.find(actionCode);
	if ((it == tb.TBItemByActionCode.end()) || it->second.isNull()) {
		return;
	}
		
	switch (it->second->type()) {
		default:
			break;
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
			onButtonPress((ZLToolbar::AbstractButtonItem&)*it->second);
			break;
	}
}

void ZLWin32ApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	int style = GetWindowLong(myMainWindow, GWL_STYLE);
	static WINDOWPLACEMENT mainPlacement;
	static WINDOWPLACEMENT toolbarPlacement;
	if (myFullScreen) {
		GetWindowPlacement(myMainWindow, &mainPlacement);
		GetWindowPlacement(myRebar, &toolbarPlacement);
		SetWindowLong(myMainWindow, GWL_STYLE, style & ~WS_CAPTION);
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(myMainWindow, HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW);
		ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
		ShowWindow(myRebar, SW_HIDE);
		if (myDockWindow != 0) {
			ShowWindow(myDockWindow, SW_SHOWDEFAULT);
			updateFullscreenToolbarSize();
		}
	} else {
		SetWindowLong(myMainWindow, GWL_STYLE, style | WS_CAPTION);
		ShowWindow(myRebar, SW_SHOWNORMAL);
		ShowWindow(myMainWindow, SW_SHOWNORMAL);
		SetWindowPlacement(myMainWindow, &mainPlacement);
		SetWindowPlacement(myRebar, &toolbarPlacement);
		if (myDockWindow != 0) {
			ShowWindow(myDockWindow, SW_HIDE);
		}
	}
	refresh();
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

class TextFieldData {

private:
	typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);

	static LRESULT CALLBACK Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	TextFieldData(HWND textField, HWND mainWindow, ZLApplication &application, const std::string &actionId);

private:
	WndProc myOriginalCallback;
	HWND myMainWindow;
	ZLApplication &myApplication;
	const std::string myActionId;
};

void ZLWin32ApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	if (myRebar == 0) {
		createWindowToolbar();
		createFloatingToolbar();
	}

	Toolbar &tb = toolbar(type(*item));

	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		{
			const ZLToolbar::TextFieldItem &textFieldItem = (ZLToolbar::TextFieldItem&)*item;
			button.idCommand = -200 + tb.TextFieldCodeById.size();
			tb.TextFieldCodeById[textFieldItem.actionId()] = button.idCommand;
			button.iBitmap = I_IMAGENONE;
			button.fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
			button.fsState = 0;
			tb.TBItemByActionCode[button.idCommand] = item;
			break;
		}
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		{
			const ZLToolbar::AbstractButtonItem &buttonItem = (const ZLToolbar::AbstractButtonItem&)*item;
    
			std::string imageFileName = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + buttonItem.iconName() + ".ico";
			ZLFile file(imageFileName);
			HICON bitmap = (HICON)LoadImageA(0, file.path().c_str(), IMAGE_ICON, IconSize, IconSize, LR_LOADFROMFILE);
			ImageList_AddIcon((HIMAGELIST)SendMessage(tb.hwnd, TB_GETIMAGELIST, 0, 0), bitmap);
    
			button.iBitmap = tb.ActionCodeById.size();
			button.fsStyle = TBSTYLE_BUTTON;
			if (type == ZLToolbar::Item::MENU_BUTTON) {
				button.fsStyle |= TBSTYLE_DROPDOWN;
			} else if (type == ZLToolbar::Item::TOGGLE_BUTTON) {
				button.fsStyle |= TBSTYLE_CHECK;
			}
			const int actionCode = tb.ActionCodeById.size() + 1;
			tb.ActionCodeById[buttonItem.actionId()] = actionCode;
			button.idCommand = actionCode;
			tb.TBItemByActionCode[button.idCommand] = item;
			button.dwData = 0;
			button.iString = 0;
    
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
		{
			button.idCommand = -100 - tb.SeparatorNumbers.size();
			tb.SeparatorNumbers[item] = button.idCommand;
			button.iBitmap = 6;
			button.fsStyle = TBSTYLE_SEP;
			break;
		}
	}
	SendMessage(tb.hwnd, TB_ADDBUTTONS, 1, (LPARAM)&button);

	if (type == ZLToolbar::Item::TEXT_FIELD) {
		const ZLToolbar::TextFieldItem &textFieldItem = (ZLToolbar::TextFieldItem&)*item;
		TBBUTTONINFO buttonInfo;
		buttonInfo.cbSize = sizeof(TBBUTTONINFO);
		buttonInfo.dwMask = TBIF_SIZE;
		buttonInfo.cx = 10 + 8 * textFieldItem.maxWidth();
		SendMessage(tb.hwnd, TB_SETBUTTONINFO, button.idCommand, (LPARAM)&buttonInfo);
		TextEditParameter *parameter = new TextEditParameter(tb.hwnd, button.idCommand, textFieldItem);
		myTextFields[button.idCommand] = parameter->handle();
		ZLToolbar::ItemPtr item = tb.TBItemByActionCode[button.idCommand];
		new TextFieldData(parameter->handle(), myMainWindow, application(), textFieldItem.actionId());
		addVisualParameter(textFieldItem.parameterId(), parameter);
	}
}

LRESULT CALLBACK TextFieldData::Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	TextFieldData *data = (TextFieldData*)GetWindowLong(hWnd, GWL_USERDATA);
	if (uMsg == WM_CHAR) {
		if (wParam == 13) {
			data->myApplication.doAction(data->myActionId);
			SetFocus(data->myMainWindow);
			return 0;
		} else if (wParam == 27) {
			SetFocus(data->myMainWindow);
			return 0;
		}
	}
	WndProc orig = data->myOriginalCallback;
	return orig(hWnd, uMsg, wParam, lParam);
}

TextFieldData::TextFieldData(HWND textField, HWND mainWindow, ZLApplication &application, const std::string &actionId) : myMainWindow(mainWindow), myApplication(application), myActionId(actionId) {
	myOriginalCallback = (WndProc)SetWindowLong(textField, GWL_WNDPROC, (LONG)Callback);
	SetWindowLong(textField, GWL_USERDATA, (LONG)this);
}

void ZLWin32ApplicationWindow::updateTextFields() {
	const size_t len = myWindowToolbar.TextFieldCodeById.size();
	for (size_t i = 0; i < len; ++i) {
		const size_t idCommand = -200 + i;
		if (myTextFields[idCommand] != 0) {
			RECT rect;
			const int index = SendMessage(myWindowToolbar.hwnd, TB_COMMANDTOINDEX, idCommand, 0);
			SendMessage(myWindowToolbar.hwnd, TB_GETITEMRECT, index, (LPARAM)&rect);
			SetWindowPos(
				myTextFields[idCommand], 0, rect.left + 5, rect.top + 10, 0, 0,
				SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE
			);
		}
	}
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	Toolbar &tb = toolbar(type(*item));

	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		{
			const ZLToolbar::TextFieldItem &textFieldItem = (const ZLToolbar::TextFieldItem&)*item;
			HWND handle = myTextFields[tb.TextFieldCodeById[textFieldItem.actionId()]];
			if (handle != 0) {
				const int idCommand = tb.TextFieldCodeById[textFieldItem.actionId()];
				PostMessage(tb.hwnd, TB_SETSTATE, idCommand, visible ? 0 : TBSTATE_HIDDEN);
				ShowWindow(handle, visible ? SW_SHOW : SW_HIDE);
				PostMessage(handle, EM_SETREADONLY, !enabled, 0);
			}
			break;
		}
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		{
			ZLToolbar::AbstractButtonItem &buttonItem = (ZLToolbar::AbstractButtonItem&)*item;
			LPARAM state = (visible ? 0 : TBSTATE_HIDDEN) | (enabled ? TBSTATE_ENABLED : 0);
			const int idCommand = tb.ActionCodeById[buttonItem.actionId()];
			PostMessage(tb.hwnd, TB_SETSTATE, (WPARAM)idCommand, state);
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			PostMessage(tb.hwnd, TB_SETSTATE, tb.SeparatorNumbers[item], visible ? 0 : TBSTATE_HIDDEN);
			break;
	}
}

ZLViewWidget *ZLWin32ApplicationWindow::createViewWidget() {
	myWin32ViewWidget = new ZLWin32ViewWidget(*this);
	return myWin32ViewWidget;
}

void ZLWin32ApplicationWindow::close() {
	DestroyWindow(myMainWindow);
}

void ZLWin32ApplicationWindow::grabAllKeys(bool) {
}

void ZLWin32ApplicationWindow::setCaption(const std::string &caption) {
	::setWindowText(myMainWindow, caption);
}

HWND ZLWin32ApplicationWindow::mainWindow() const {
	return myMainWindow;
}

void ZLWin32ApplicationWindow::blockMouseEvents(bool block) {
	myBlockMouseEvents = block;
}

bool ZLWin32ApplicationWindow::mouseEventsAreBlocked() const {
	return myBlockMouseEvents;
}

void ZLWin32ApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	static HCURSOR handCursor = LoadCursor(0, IDC_HAND);
	myCursor = hyperlink ? handCursor : 0;
}

void ZLWin32ApplicationWindow::setWait(bool wait) {
	static HCURSOR waitCursor = LoadCursor(0, IDC_WAIT);
	myCursor = wait ? waitCursor : 0;
	SetCursor(myCursor);
	myWait = wait;
}

ZLWin32ApplicationWindow::TextEditParameter::TextEditParameter(HWND toolbar, int idCommand, const ZLToolbar::TextFieldItem &item) {
	const int index = SendMessage(toolbar, TB_COMMANDTOINDEX, idCommand, 0);
	RECT rect;
	SendMessage(toolbar, TB_GETITEMRECT, index, (LPARAM)&rect);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NOHIDESEL | ES_CENTER;
	if (item.symbolSet() == ZLToolbar::TextFieldItem::SET_DIGITS) {
		style |= ES_NUMBER;
	}
	myTextEdit = CreateWindow(WC_EDIT, 0, style, rect.left + 5, rect.top + 12, rect.right - rect.left - 10, rect.bottom - rect.top - 15, toolbar, (HMENU)idCommand, GetModuleHandle(0), 0);

	HWND tooltipHandle = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, myTextEdit, 0, GetModuleHandle(0), 0);
	ZLUnicodeUtil::Ucs2String buffer;
	::createNTWCHARString(buffer, item.tooltip());
	TOOLINFO tti;
	tti.cbSize = sizeof(tti);
	tti.uFlags = TTF_SUBCLASS;
	tti.hwnd = myTextEdit;
	tti.hinst = GetModuleHandle(0);
	tti.uId = 0;
	tti.lpszText = (WCHAR*)::wchar(buffer);
	GetClientRect(myTextEdit, &tti.rect);
	SendMessage(tooltipHandle, TTM_ADDTOOL, 0, (LPARAM)&tti);
}

HWND ZLWin32ApplicationWindow::TextEditParameter::handle() const {
	return myTextEdit;
}

std::string ZLWin32ApplicationWindow::TextEditParameter::internalValue() const {
	int len = GetWindowTextLengthW(myTextEdit);
	if (len == 0) {
		return "";
	}
	static ZLUnicodeUtil::Ucs2String buffer;
	buffer.assign(len + 1, '\0');
	GetWindowTextW(myTextEdit, (WCHAR*)::wchar(buffer), len + 1);
	std::string text;
	ZLUnicodeUtil::ucs2ToUtf8(text, buffer);
	return text;
}

void ZLWin32ApplicationWindow::TextEditParameter::internalSetValue(const std::string &value) {
	static ZLUnicodeUtil::Ucs2String buffer;
	::createNTWCHARString(buffer, value);
	SetWindowTextW(myTextEdit, ::wchar(buffer));
}

void ZLWin32ApplicationWindow::resetFocus() {
	SetFocus(myMainWindow);
}

void ZLWin32ApplicationWindow::updateCursor() const {
	if (myCursor != 0) {
		SetCursor(myCursor);
	}
}
