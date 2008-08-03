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

const int ICON_SIZE = 32;

ZLWin32ApplicationWindow *ZLWin32ApplicationWindow::ourApplicationWindow = 0;

int ZLWin32ApplicationWindow::x(WPARAM lParam) {
	return LOWORD(lParam);
}

int ZLWin32ApplicationWindow::y(WPARAM lParam) {
	return HIWORD(lParam) - ourApplicationWindow->topOffset();
}

LRESULT CALLBACK ZLWin32ApplicationWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ourApplicationWindow->mainLoopCallback(hWnd, uMsg, wParam, lParam);
}

LRESULT ZLWin32ApplicationWindow::mainLoopCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_TIMER:
			((ZLWin32TimeManager&)ZLTimeManager::instance()).execute(wParam);
			return 0;
		case WM_CREATE:
			myMainWindow = hWnd;
			ZLApplicationWindow::init();
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_LBUTTONDOWN:
			if (!myBlockMouseEvents) {
				SetFocus(myMainWindow);
				if (myWin32ViewWidget->myMouseCaptured) {
					SetCapture(myMainWindow);
				}
				myWin32ViewWidget->onMousePress(x(lParam), y(lParam));
			}
			return 0;
		case WM_LBUTTONUP:
			if (!myBlockMouseEvents) {
				myWin32ViewWidget->onMouseRelease(x(lParam), y(lParam));
			}
			if (myMainWindow == GetCapture()) {
				ReleaseCapture();
			}
			return 0;
		case WM_MOUSEMOVE:
			if (!myBlockMouseEvents) {
				if (wParam & MK_LBUTTON) {
					myWin32ViewWidget->onMouseMovePressed(x(lParam), y(lParam));
				} else {
					myWin32ViewWidget->onMouseMove(x(lParam), y(lParam));
				}
			}
			if (myCursor != 0) {
				SetCursor(myCursor);
			}
			return 0;
		case WM_MOUSEWHEEL:
			if (!myBlockMouseEvents) {
				application().doActionByKey(
					((short)HIWORD(wParam) > 0) ?
						ZLApplication::MouseScrollUpKey :
						ZLApplication::MouseScrollDownKey
				);
			}
			return 0;
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
			if (myToolbar != 0) {
				SendMessage(myToolbar, TB_AUTOSIZE, 0, 0);
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_ERASEBKGND:
			return 0;
		case WM_PAINT:
			myWin32ViewWidget->doPaint();
			return 0;
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
			if (LOWORD(wParam) <= myActionCodeById.size()) {
				onToolbarButtonPress(LOWORD(wParam));
			}
			return 0;
		case WM_NOTIFY:
		{
			const NMHDR *notificationHeader = (const NMHDR*)lParam;
			switch (notificationHeader->code) {
				case TTN_NEEDTEXT:
				{
					TOOLTIPTEXT &tooltip = *(TOOLTIPTEXT*)lParam;
					ZLToolbar::ItemPtr item = myTBItemByActionCode[tooltip.hdr.idFrom];
					if (!item.isNull()) {
						const ZLToolbar::AbstractButtonItem &button =
							(const ZLToolbar::AbstractButtonItem&)*item;
						ZLUnicodeUtil::Ucs2String tooltipBuffer;
						::createNTWCHARString(tooltipBuffer, button.tooltip());
						size_t length = std::max(tooltipBuffer.size(), (size_t)80);
						memcpy((char*)tooltip.szText, (char*)::wchar(tooltipBuffer), 2 * length);
						tooltip.uFlags = TTF_DI_SETITEM;
					}
					break;
				}
				case NM_CUSTOMDRAW:
					for (size_t i = 0; i < myTextFieldCodeById.size(); ++i) {
						updateTextField(-200 + i);
					}
					break;
				case TBN_DROPDOWN:
				{
					NMTOOLBAR &nmToolbar = *(NMTOOLBAR*)lParam;
					ZLToolbar::ItemPtr item = myTBItemByActionCode[nmToolbar.iItem];
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
									miInfo.cch = text.size();
									InsertMenuItem(popup, i, true, &miInfo);
								}
								POINT p;
								p.x = nmToolbar.rcButton.left;
								p.y = nmToolbar.rcButton.bottom + 8;
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
					break;
				}
			}
			return 0;
		}
		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			const int dir = (uMsg == WM_VSCROLL) ? SB_VERT : SB_HORZ;
			const size_t extra = 
				(uMsg == WM_VSCROLL) ?
					myWin32ViewWidget->myVScrollBarExtra :
					myWin32ViewWidget->myHScrollBarExtra;
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_ALL;
			GetScrollInfo(myMainWindow, dir, &info);
			switch (LOWORD(wParam)) {
				case SB_TOP:
					info.nPos = info.nMin;
					break;
				case SB_BOTTOM:
					info.nPos = info.nMax - extra;
					break;
				case SB_LINEUP:
					info.nPos -= 100;
					break;
				case SB_LINEDOWN:
					info.nPos += 100;
					break;
				case SB_PAGEUP:
					info.nPos -= info.nPage;
					break;
				case SB_PAGEDOWN:
					info.nPos += info.nPage;
					break;
				case SB_THUMBTRACK:
					info.nPos = info.nTrackPos;
					break;
			}
			info.fMask = SIF_POS;
			SetScrollInfo(myMainWindow, dir, &info, true);
			info.fMask = SIF_ALL;
			GetScrollInfo(myMainWindow, dir, &info);
			myWin32ViewWidget->onScrollbarMoved(
				(uMsg == WM_VSCROLL) ? ZLView::VERTICAL : ZLView::HORIZONTAL,
				info.nMax - info.nMin - extra,
				info.nPos,
				info.nPos + info.nPage - extra
			);
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

static const std::string OPTIONS = "Options";

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myMainWindow(0),
	myToolbar(0),
	myBlockMouseEvents(false),
	myKeyboardModifierMask(0),
	myFullScreen(false),
	myWait(false),
	myCursor(0) {

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
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
	wc.lpszClassName = ::wchar(::createNTWCHARString(myClassName, ZLibrary::ApplicationName()));
	wc.hIconSm = wc.hIcon;

	RegisterClassEx(&wc);

	ourApplicationWindow = this;
}

void ZLWin32ApplicationWindow::init() {
	const WCHAR *aName = ::wchar(myClassName);
	CreateWindow(aName, aName, WS_VSCROLL | WS_OVERLAPPEDWINDOW, myXOption.value(), myYOption.value(), myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, GetModuleHandle(0), 0);

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

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	ourApplicationWindow = 0;
}

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	PostMessage(myToolbar, TB_CHECKBUTTON, myActionCodeById[button.actionId()], button.isPressed());
}

void ZLWin32ApplicationWindow::onToolbarButtonPress(int actionCode) {
	onButtonPress((ZLToolbar::AbstractButtonItem&)*myTBItemByActionCode[actionCode]);
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
		GetWindowPlacement(myToolbar, &toolbarPlacement);
		ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
		ShowWindow(myToolbar, SW_HIDE);
		SetWindowLong(myMainWindow, GWL_STYLE, style & ~WS_CAPTION);
	} else {
		SetWindowLong(myMainWindow, GWL_STYLE, style | WS_CAPTION);
		ShowWindow(myToolbar, SW_SHOWNORMAL);
		ShowWindow(myMainWindow, SW_SHOWNORMAL);
		SetWindowPlacement(myMainWindow, &mainPlacement);
		SetWindowPlacement(myToolbar, &toolbarPlacement);
	}
	myWin32ViewWidget->repaint();
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLWin32ApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	if (myToolbar == 0) {
  	myToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);
		SendMessage(myToolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
		SendMessage(myToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(myToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(ICON_SIZE, ICON_SIZE));
		SendMessage(myToolbar, TB_SETINDENT, 3, 0);
		SendMessage(myToolbar, TB_SETIMAGELIST, 0, (LPARAM)ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR32 | ILC_MASK, 0, 100));
	}

	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		{
			const ZLToolbar::TextFieldItem &textFieldItem = (ZLToolbar::TextFieldItem&)*item;
			button.idCommand = -200 + myTextFieldCodeById.size();
			myTextFieldCodeById[textFieldItem.actionId()] = button.idCommand;
			button.iBitmap = I_IMAGENONE;
			button.fsStyle = TBSTYLE_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
			button.fsState = 0;
			myTBItemByActionCode[button.idCommand] = item;
			break;
		}
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		{
			const ZLToolbar::AbstractButtonItem &buttonItem = (const ZLToolbar::AbstractButtonItem&)*item;
    
			std::string imageFileName = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + buttonItem.iconName() + ".ico";
			ZLFile file(imageFileName);
			HICON bitmap = (HICON)LoadImageA(0, file.path().c_str(), IMAGE_ICON, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
			ImageList_AddIcon((HIMAGELIST)SendMessage(myToolbar, TB_GETIMAGELIST, 0, 0), bitmap);
    
			button.iBitmap = myActionCodeById.size();
			button.fsStyle = TBSTYLE_BUTTON;
			if (type == ZLToolbar::Item::MENU_BUTTON) {
				button.fsStyle |= TBSTYLE_DROPDOWN;
			} else if (type == ZLToolbar::Item::TOGGLE_BUTTON) {
				button.fsStyle |= TBSTYLE_CHECK;
			}
			const int actionCode = myActionCodeById.size() + 1;
			myActionCodeById[buttonItem.actionId()] = actionCode;
			button.idCommand = actionCode;
			myTBItemByActionCode[button.idCommand] = item;
			button.dwData = 0;
			button.iString = 0;
    
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
		{
			button.idCommand = -100 - mySeparatorNumbers.size();
			mySeparatorNumbers[item] = button.idCommand;
			button.iBitmap = 6;
			button.fsStyle = TBSTYLE_SEP;
			break;
		}
	}
	SendMessage(myToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
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

void ZLWin32ApplicationWindow::updateTextField(int idCommand) {
	RECT rect;
	const int index = SendMessage(myToolbar, TB_COMMANDTOINDEX, idCommand, 0);
	if (myTextFields[idCommand] == 0) {
		TBBUTTONINFO buttonInfo;
		buttonInfo.cbSize = sizeof(TBBUTTONINFO);
		buttonInfo.dwMask = TBIF_SIZE;
		buttonInfo.cx = 50;
		SendMessage(myToolbar, TB_SETBUTTONINFO, idCommand, (LPARAM)&buttonInfo);
		SendMessage(myToolbar, TB_GETITEMRECT, index, (LPARAM)&rect);
		HWND handle = CreateWindow(WC_EDIT, 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NOHIDESEL | ES_CENTER | ES_NUMBER, rect.left + 5, rect.top + 12, rect.right - rect.left - 10, rect.bottom - rect.top - 15, myToolbar, (HMENU)idCommand, GetModuleHandle(0), 0);
		myTextFields[idCommand] = handle;
		ZLToolbar::ItemPtr item = myTBItemByActionCode[idCommand];
		const ZLToolbar::TextFieldItem &textFieldItem = (ZLToolbar::TextFieldItem&)*item;
		new TextFieldData(handle, myMainWindow, application(), textFieldItem.actionId());
		addVisualParameter(textFieldItem.parameterId(), new TextEditParameter(handle));
	} else {
		SendMessage(myToolbar, TB_GETITEMRECT, index, (LPARAM)&rect);
		SetWindowPos(
			myTextFields[idCommand], 0, rect.left + 5, rect.top + 10, 0, 0,
			SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE
		);
	}
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	const ZLToolbar::Item::Type type = item->type();
	switch (type) {
		case ZLToolbar::Item::TEXT_FIELD:
		{
			const ZLToolbar::TextFieldItem &textFieldItem = (const ZLToolbar::TextFieldItem&)*item;
			HWND handle = myTextFields[myTextFieldCodeById[textFieldItem.actionId()]];
			if (handle != 0) {
				PostMessage(myToolbar, TB_SETSTATE, (WPARAM)myTextFieldCodeById[textFieldItem.actionId()], visible ? 0 : TBSTATE_HIDDEN);
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
			PostMessage(myToolbar, TB_SETSTATE, (WPARAM)myActionCodeById[buttonItem.actionId()], state);
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			PostMessage(myToolbar, TB_SETSTATE, mySeparatorNumbers[item], visible ? 0 : TBSTATE_HIDDEN);
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

int ZLWin32ApplicationWindow::topOffset() const {
	if ((myToolbar != 0) && (!myFullScreen)) {
		RECT toolbarRectangle;
		GetWindowRect(myToolbar, &toolbarRectangle);
		POINT p;
		p.x = toolbarRectangle.right;
		p.y = toolbarRectangle.bottom;
		ScreenToClient(myMainWindow, &p);
		return p.y;
	}
	return 0;
}

void ZLWin32ApplicationWindow::blockMouseEvents(bool block) {
	myBlockMouseEvents = block;
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

ZLWin32ApplicationWindow::TextEditParameter::TextEditParameter(HWND textEdit) : myTextEdit(textEdit) {
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
