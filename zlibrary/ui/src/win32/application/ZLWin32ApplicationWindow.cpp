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

#include <windowsx.h>

#define RBN_CHEVRONPUSHED (RBN_FIRST - 10)
#define MY_WM_FULLSCREEN (WM_USER + 200)

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>
#include <ZLFile.h>
#include <ZLOptionEntry.h>
#include <ZLDialog.h>
#include <ZLPaintContext.h>
#include <ZLPopupData.h>

#include "ZLWin32ApplicationWindow.h"
#include "ZLWin32PopupMenu.h"
#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../view/ZLWin32ViewWidget.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"
#include "../time/ZLWin32Time.h"

const int ZLWin32ApplicationWindow::IconSize = 32;

ZLWin32ApplicationWindow *ZLWin32ApplicationWindow::ourApplicationWindow = 0;

static HWND getTextItem(HWND comboBox) {
	COMBOBOXINFO cbi;
	ZeroMemory(&cbi, sizeof(cbi));
	cbi.cbSize = sizeof(cbi);
	GetComboBoxInfo(comboBox, &cbi);
	return cbi.hwndItem;
}

LRESULT CALLBACK ZLWin32ApplicationWindow::Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ourApplicationWindow->mainLoopCallback(hWnd, uMsg, wParam, lParam);
}

LRESULT ZLWin32ApplicationWindow::mainLoopCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case MY_WM_FULLSCREEN:
			setFullscreen(true);
			return 0;
		case WM_MEASUREITEM:
			if (!myPopupMenu.isNull()) {
				myPopupMenu->measureItem(*(MEASUREITEMSTRUCT*)lParam);
			}
			return 1;
		case WM_DRAWITEM:
			if (!myPopupMenu.isNull()) {
				myPopupMenu->drawItem(*(DRAWITEMSTRUCT*)lParam);
			}
			return 1;
		case WM_MOUSEWHEEL:
			if (!mouseEventsAreBlocked()) {
				application().doActionByKey(
					((short)HIWORD(wParam) > 0) ?
						ZLApplication::MouseScrollUpKey :
						ZLApplication::MouseScrollDownKey
				);
			}
			return 0;
		case WM_PAINT:
		{
			LRESULT code = DefWindowProc(hWnd, uMsg, wParam, lParam);
			updateWindowToolbarInfo();
			updateFullscreenToolbarSize();
			return code;
		}
		case WM_TIMER:
			((ZLWin32TimeManager&)ZLTimeManager::Instance()).execute(wParam);
			return 0;
		case WM_CREATE:
			myMainWindow = hWnd;
			ZLApplicationWindow::init();
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
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
		case WM_SYSKEYUP:
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
			RECT rebarRect;
			GetWindowRect(myRebar, &rebarRect);
			int offset = rebarRect.bottom - rebarRect.top - 1;
			MoveWindow(myRebar, 0, 0, LOWORD(lParam), rebarRect.bottom - rebarRect.top, true);
			if (myFullScreen) {
				offset = 0;
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
				myWidthOption.setValue(rectangle.right - rectangle.left);
				myHeightOption.setValue(rectangle.bottom - rectangle.top);
			}
			application().closeView();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			onToolbarButtonRelease(LOWORD(wParam));
			return 0;
		case WM_NOTIFY:
		{
			const NMHDR *notificationHeader = (const NMHDR*)lParam;
			switch (notificationHeader->code) {
				case TTN_NEEDTEXT:
					setTooltip(*(TOOLTIPTEXT*)lParam);
					break;
				case NM_CUSTOMDRAW:
					updateParameters();
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
	if (tb.hwnd == 0) {
		return;
	}
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
	if (tb.hwnd == 0) {
		return;
	}
	ZLToolbar::ItemPtr item = tb.TBItemByActionCode[nmToolbar.iItem];
	if (!item.isNull()) {
		const ZLToolbar::MenuButtonItem &button =
			(const ZLToolbar::MenuButtonItem&)*item;
		shared_ptr<ZLPopupData> popupData = button.popupData();
		if (!popupData.isNull()) {
			const int count = popupData->count();
			if (count != 0) {
				myPopupMenu = new ZLWin32PopupMenu(myMainWindow);
				for (int i = 0; i < count; ++i) {
					myPopupMenu->addItem(popupData->text(i), 0, true);
				}
				POINT p;
				p.x = nmToolbar.rcButton.left;
				p.y = nmToolbar.rcButton.bottom + 6;
				ClientToScreen(myMainWindow, &p);
				int code = myPopupMenu->run(p);
				if (code > 0) {
					popupData->run(code - 1);
				}
				myPopupMenu.reset();
			}
		}
	}
}

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myMainWindow(0),
	myRebar(0),
	myDockWindow(0),
	myFloatingToolbarClassRegistered(false),
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
	wc.lpfnWndProc = Callback;
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
	const DWORD layoutStyle = ZLLanguageUtil::isRTLLanguage(ZLibrary::Language()) ? WS_EX_LAYOUTRTL : 0;
	CreateWindowEx(layoutStyle, aName, aName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, myXOption.value(), myYOption.value(), myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, GetModuleHandle(0), 0);

	// TODO: Hmm, replace SW_SHOWDEFAULT by nCmdShow?
	ShowWindow(myMainWindow, SW_SHOWDEFAULT);

	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			PostMessage(myMainWindow, MY_WM_FULLSCREEN, 0, 0);
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

void ZLWin32ApplicationWindow::processAllEvents() {
	// TODO: implement
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	ourApplicationWindow = 0;
	ZLWin32DialogManager &dManager = (ZLWin32DialogManager&)ZLWin32DialogManager::Instance();
	dManager.removeApplicationWindow();
}

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	Toolbar &tb = toolbar(type(button));
	if (tb.hwnd == 0) {
		return;
	}

	PostMessage(tb.hwnd, TB_CHECKBUTTON, tb.ActionCodeById[button.actionId()], button.isPressed());
}

void ZLWin32ApplicationWindow::onToolbarButtonRelease(int actionCode) {
	Toolbar &tb = toolbar(isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR);
	if (tb.hwnd == 0) {
		return;
	}
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
		int cx;
		int cy;
		HDC displayDC = GetDC(0);
		if (displayDC != 0) {
			cx = GetDeviceCaps(displayDC, HORZRES);
			cy = GetDeviceCaps(displayDC, VERTRES);
			ReleaseDC(0, displayDC);
		} else {
			cx = GetSystemMetrics(SM_CXSCREEN);
			cy = GetSystemMetrics(SM_CYSCREEN);
		}
		SetWindowPos(myMainWindow, HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW);
		ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
		ShowWindow(myRebar, SW_HIDE);
		createFloatingToolbar();
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

		RECT mainRect;
		GetClientRect(myMainWindow, &mainRect);
		RECT rebarRect;
		GetWindowRect(myRebar, &rebarRect);
		MoveWindow(myRebar, 0, 0, mainRect.right - mainRect.left, rebarRect.bottom - rebarRect.top, true);

		destroyFloatingToolbar();
	}
	refresh();
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLWin32ApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	if (myRebar == 0) {
		createWindowToolbar();
	}

	Toolbar &tb = toolbar(type(*item));
	if (tb.hwnd == 0) {
		myFloatingToolbarItems.push_back(item);
		return;
	}

	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		case ZLToolbar::Item::COMBO_BOX:
		{
			const ZLToolbar::ParameterItem &textFieldItem = (ZLToolbar::ParameterItem&)*item;
			button.idCommand = -200 + tb.ParameterCodeById.size();
			tb.ParameterCodeById[textFieldItem.actionId()] = button.idCommand;
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
			ZLUnicodeUtil::Ucs2String wPath;
			HICON bitmap = (HICON)LoadImageW(0, ::wchar(::createNTWCHARString(wPath, file.path())), IMAGE_ICON, IconSize, IconSize, LR_LOADFROMFILE);
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

	switch (type) {
		default:
			break;
		case ZLToolbar::Item::TEXT_FIELD:
		case ZLToolbar::Item::COMBO_BOX:
		{
			const ZLToolbar::ParameterItem &textFieldItem = (ZLToolbar::ParameterItem&)*item;
			TBBUTTONINFO buttonInfo;
			buttonInfo.cbSize = sizeof(TBBUTTONINFO);
			buttonInfo.dwMask = TBIF_SIZE;
			buttonInfo.cx = 10 + 8 * textFieldItem.maxWidth();
			if (type == ZLToolbar::Item::COMBO_BOX) {
				buttonInfo.cx += 15;
			}
			SendMessage(tb.hwnd, TB_SETBUTTONINFO, button.idCommand, (LPARAM)&buttonInfo);
			TextEditParameter *parameter = new TextEditParameter(application(), myMainWindow, tb.hwnd, button.idCommand, textFieldItem);
			addVisualParameter(textFieldItem.parameterId(), parameter);
			myParameters[button.idCommand] = parameter->handle();
			break;
		}
	}
}

LRESULT CALLBACK ZLWin32ApplicationWindow::TextEditParameter::ComboBoxCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	TextEditParameter &parameter =
		*(TextEditParameter*)GetWindowLong(hWnd, GWL_USERDATA);

	if ((uMsg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELCHANGE)) {
		HWND comboBox = parameter.myComboBox;
		const int index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
		const int length = SendMessage(comboBox, CB_GETLBTEXTLEN, index, 0);
		ZLUnicodeUtil::Ucs2String buffer;
		if (length > 0) {
			buffer.assign(length + 1, '\0');
			SendMessage(comboBox, CB_GETLBTEXT, index, (LPARAM)&buffer.front());
			buffer.pop_back();
		}
		std::string value;
		ZLUnicodeUtil::ucs2ToUtf8(value, buffer);
		parameter.setValue(value);

		parameter.myApplication.doAction(parameter.myParameterItem.actionId());
		SetFocus(parameter.myMainWindow);
	}

	WndProc orig = parameter.myOriginalComboBoxCallback;
	return orig(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ZLWin32ApplicationWindow::TextEditParameter::TextEditCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	TextEditParameter &parameter =
		*(TextEditParameter*)GetWindowLong(hWnd, GWL_USERDATA);

	if (uMsg == WM_CHAR) {
		if (wParam == 13) {
			parameter.myApplication.doAction(parameter.myParameterItem.actionId());
			SetFocus(parameter.myMainWindow);
			return 0;
		} else if (wParam == 27) {
			parameter.restoreOldValue();
			SetFocus(parameter.myMainWindow);
			return 0;
		}
	}
	WndProc orig = parameter.myOriginalTextEditCallback;
	return orig(hWnd, uMsg, wParam, lParam);
}

void ZLWin32ApplicationWindow::updateParameters() {
	const size_t len = myWindowToolbar.ParameterCodeById.size();
	for (size_t i = 0; i < len; ++i) {
		const size_t idCommand = -200 + i;
		if (myParameters[idCommand] != 0) {
			RECT rect;
			const int index = SendMessage(myWindowToolbar.hwnd, TB_COMMANDTOINDEX, idCommand, 0);
			SendMessage(myWindowToolbar.hwnd, TB_GETITEMRECT, index, (LPARAM)&rect);
			SetWindowPos(
				myParameters[idCommand], 0, rect.left + 5, rect.top + 8, 0, 0,
				SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE
			);
		}
	}
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	Toolbar &tb = toolbar(type(*item));
	if (tb.hwnd == 0) {
		return;
	}

	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		case ZLToolbar::Item::COMBO_BOX:
		{
			const ZLToolbar::ParameterItem &textFieldItem = (const ZLToolbar::ParameterItem&)*item;
			HWND handle = myParameters[tb.ParameterCodeById[textFieldItem.actionId()]];
			if (handle != 0) {
				const int idCommand = tb.ParameterCodeById[textFieldItem.actionId()];
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

static void addTooltipToWindow(HWND window, const std::string &tooltip) {
	HWND tooltipHandle = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, window, 0, GetModuleHandle(0), 0);
	TOOLINFO tti;
	tti.cbSize = sizeof(tti);
	tti.uFlags = TTF_SUBCLASS;
	tti.hwnd = window;
	tti.hinst = GetModuleHandle(0);
	tti.uId = 0;
	ZLUnicodeUtil::Ucs2String buffer;
	::createNTWCHARString(buffer, tooltip);
	tti.lpszText = (WCHAR*)::wchar(buffer);
	GetClientRect(window, &tti.rect);
	SendMessage(tooltipHandle, TTM_ADDTOOL, 0, (LPARAM)&tti);
}

ZLWin32ApplicationWindow::TextEditParameter::TextEditParameter(ZLApplication &application, HWND mainWindow, HWND toolbar, int idCommand, const ZLToolbar::ParameterItem &item) : myApplication(application), myMainWindow(mainWindow), myParameterItem(item) {
	const int index = SendMessage(toolbar, TB_COMMANDTOINDEX, idCommand, 0);
	RECT rect;
	SendMessage(toolbar, TB_GETITEMRECT, index, (LPARAM)&rect);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (item.type() == ZLToolbar::Item::COMBO_BOX) {
		style |= CBS_DROPDOWN | WS_VSCROLL;
	}
	myComboBox = CreateWindow(WC_COMBOBOX, 0, style, rect.left + 5, rect.top + 8, rect.right - rect.left - 10, rect.bottom - rect.top - 14, toolbar, (HMENU)idCommand, GetModuleHandle(0), 0);
	HWND textItem = getTextItem(myComboBox);
	DWORD textItemStyle = GetWindowLong(textItem, GWL_STYLE);
	textItemStyle |= ES_CENTER | ES_NOHIDESEL;
	if (item.symbolSet() == ZLToolbar::ParameterItem::SET_DIGITS) {
		textItemStyle |= ES_NUMBER;
	}
	SetWindowLong(textItem, GWL_STYLE, textItemStyle);

	addTooltipToWindow(myComboBox, item.tooltip());
	addTooltipToWindow(getTextItem(myComboBox), item.tooltip());

	myOriginalComboBoxCallback = (WndProc)SetWindowLong(myComboBox, GWL_WNDPROC, (LONG)ComboBoxCallback);
	SetWindowLong(myComboBox, GWL_USERDATA, (LONG)this);

	HWND textEdit = getTextItem(myComboBox);
	myOriginalTextEditCallback = (WndProc)SetWindowLong(textEdit, GWL_WNDPROC, (LONG)TextEditCallback);
	SetWindowLong(textEdit, GWL_USERDATA, (LONG)this);
}

HWND ZLWin32ApplicationWindow::TextEditParameter::handle() const {
	return myComboBox;
}

void ZLWin32ApplicationWindow::TextEditParameter::setValueList(const std::vector<std::string> &values) {
	std::string oldValue = internalValue();

	if (myParameterItem.type() == ZLToolbar::Item::TEXT_FIELD) {
		return;
	}

	SendMessage(myComboBox, CB_RESETCONTENT, 0, 0);

	ZLUnicodeUtil::Ucs2String buffer;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it) {
		SendMessage(myComboBox, CB_ADDSTRING, 0, (LPARAM)::wchar(::createNTWCHARString(buffer, *it)));
	}

	if (values.size() > 0) {
		SendMessage(myComboBox, CB_SETMINVISIBLE, std::min((int)values.size(), 7), 0);
	}

	internalSetValue(oldValue);
}

std::string ZLWin32ApplicationWindow::TextEditParameter::internalValue() const {
	int len = GetWindowTextLengthW(myComboBox);
	if (len == 0) {
		return "";
	}
	static ZLUnicodeUtil::Ucs2String buffer;
	buffer.assign(len + 1, '\0');
	GetWindowTextW(myComboBox, (WCHAR*)::wchar(buffer), len + 1);
	buffer.pop_back();
	std::string text;
	ZLUnicodeUtil::ucs2ToUtf8(text, buffer);
	return text;
}

void ZLWin32ApplicationWindow::TextEditParameter::internalSetValue(const std::string &value) {
	static ZLUnicodeUtil::Ucs2String buffer;
	::createNTWCHARString(buffer, value);
	SetWindowTextW(getTextItem(myComboBox), ::wchar(buffer));
}

void ZLWin32ApplicationWindow::resetFocus() {
	SetFocus(myMainWindow);
}

void ZLWin32ApplicationWindow::updateCursor() const {
	if (myCursor != 0) {
		SetCursor(myCursor);
	}
}

void ZLWin32ApplicationWindow::Toolbar::clear() {
	hwnd = 0;
	SeparatorNumbers.clear();
	ActionCodeById.clear();
	ParameterCodeById.clear();
	TBItemByActionCode.clear();
	for (std::map<HICON,HBITMAP>::const_iterator it = BitmapByIcon.begin(); it != BitmapByIcon.end(); ++it) {
		DestroyIcon(it->first);
		DeleteBitmap(it->second);
	}
	BitmapByIcon.clear();
}
