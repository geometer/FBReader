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

#include <ZLUnicodeUtil.h>

#include "W32DialogPanel.h"
#include "W32Control.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

static const int FirstControlId = 2001;

std::map<HWND,W32DialogPanel*> W32DialogPanel::ourPanels;
const std::string W32DialogPanel::PANEL_SELECTED_EVENT = "Dialog Panel: Selected";
const std::string W32DialogPanel::APPLY_BUTTON_PRESSED_EVENT = "Dialog Panel: Apply Button Pressed";
UINT W32DialogPanel::LAYOUT_MESSAGE = 0;

static void allocateString(WORD *&p, const std::string &text) {
	ZLUnicodeUtil::Ucs2String ucs2Str;
	::createNTWCHARString(ucs2Str, text);
	memcpy(p, ::wchar(ucs2Str), 2 * ucs2Str.size());
	p += ucs2Str.size();
}

W32DialogPanel::W32DialogPanel(HWND mainWindow, const std::string &caption) : W32ControlCollection(FirstControlId), myMainWindow(mainWindow), myCaption(caption), myAddress(0), myPanelWindow(0) {
	if (LAYOUT_MESSAGE == 0) {
		LAYOUT_MESSAGE = RegisterWindowMessageA("layout");
	}
}

W32DialogPanel::~W32DialogPanel() {
	if (myAddress != 0) {
		delete[] myAddress;
	}
	if (myPanelWindow != 0) {
		ourPanels.erase(myPanelWindow);
	}
}

void W32DialogPanel::init(HWND dialogWindow) {
	myPanelWindow = dialogWindow;
	ShowScrollBar(myPanelWindow, SB_BOTH, false);
	ourPanels[myPanelWindow] = this;	
	myElement->init(dialogWindow, this);
}

void W32DialogPanel::calculateSize() {
	myMinimumSize = myElement->minimumSize();
	mySize = myMinimumSize;
}

W32Widget::Size W32DialogPanel::size() const {
	return mySize;
}

void W32DialogPanel::setSize(W32Widget::Size size) {
	mySize = size;
	myRealSize = size;
}

void W32DialogPanel::updateElementSize() {
	myElement->setPosition(0, 0, mySize);
}

DWORD W32DialogPanel::style() const {
	return DS_SHELLFONT | DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION | WS_VSCROLL | WS_HSCROLL;
}

DLGTEMPLATE *W32DialogPanel::dialogTemplate() {
	if (myAddress != 0) {
		delete[] myAddress;
	}

	if ((mySize.Width == 0) && (mySize.Height == 0)) {
		calculateSize();
	}
	updateElementSize();

	const std::string fontName = "MS Shell Dlg";
	int size = 14 + ZLUnicodeUtil::utf8Length(myCaption) + ZLUnicodeUtil::utf8Length(fontName) + myElement->allocationSize();
	size += size % 2;
	myAddress = new WORD[size];

	WORD *p = myAddress;
	*p++ = LOWORD(style());
	*p++ = HIWORD(style());
	*p++ = 0;
	*p++ = 0;
	*p++ = myElement->controlNumber();
	*p++ = 0; // X
	*p++ = 0; // Y
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = 0;
	*p++ = 0;
	allocateString(p, myCaption);
	*p++ = 8; // FONT SIZE -- should be always 8?
	allocateString(p, fontName);
	if ((p - myAddress) % 2 == 1) {
		p++;
	}

	short id = FirstControlId;
	myElement->allocate(p, id);

	return (DLGTEMPLATE*)myAddress;
}

void W32DialogPanel::setElement(W32WidgetPtr element) {
	myElement = element;
}

bool W32DialogPanel::commandCallback(WPARAM wParam) {
	W32Control *control = (*this)[LOWORD(wParam)];
	if (control != 0) {
		control->commandCallback(HIWORD(wParam));
		return true;
	}
	return false;
}

bool W32DialogPanel::notificationCallback(WPARAM wParam, LPARAM lParam) {
	W32Control *control = (*this)[LOWORD(wParam)];
	if (control != 0) {
		control->notificationCallback(lParam);
	}
	return false;
}

bool W32DialogPanel::drawItemCallback(WPARAM wParam, DRAWITEMSTRUCT &di) {
	W32Control *control = (*this)[LOWORD(wParam)];
	if (control != 0) {
		control->drawItemCallback(di);
		return true;
	}
	return false;
}

void W32DialogPanel::invalidate() {
	if (myPanelWindow != 0) {
		PostMessage(myPanelWindow, LAYOUT_MESSAGE, 0, 0);
		myDoLayout = true;
	}
}

void W32DialogPanel::layout() {
	if (myDoLayout) {
		calculateSize();
		if (myRealSize.Width != 0) {
			mySize.Width = std::max(mySize.Width, myRealSize.Width);
		}
		if (myPanelWindow != 0) {
			RECT rectangle;
			rectangle.left = 0;
			rectangle.top = 0;
			rectangle.right = mySize.Width;
			rectangle.bottom = mySize.Height;
			MapDialogRect(myPanelWindow, &rectangle);
			RECT realRectangle;
			GetClientRect(myPanelWindow, &realRectangle);
			bool hOversized = rectangle.right > realRectangle.right + 1;
			bool vOversized = rectangle.bottom > realRectangle.bottom + 1;
			int rightBound = realRectangle.right;
			int bottomBound = realRectangle.bottom;
			if (hOversized || vOversized) {
				if (!hOversized || !vOversized) {
					SCROLLBARINFO info;
					info.cbSize = sizeof(info);
					if (hOversized) {
						GetScrollBarInfo(myPanelWindow, OBJID_HSCROLL, &info);
						vOversized = rectangle.bottom >= info.rcScrollBar.top;
						if (vOversized) {
							bottomBound = info.rcScrollBar.top - 1;
						}
					} else {
						GetScrollBarInfo(myPanelWindow, OBJID_VSCROLL, &info);
						hOversized = rectangle.right >= info.rcScrollBar.left;
						if (hOversized) {
							rightBound = info.rcScrollBar.left - 1;
						}
					}
				}
			}
			ShowScrollBar(myPanelWindow, SB_HORZ, hOversized);
			if (hOversized) {
				SCROLLINFO info;
				info.cbSize = sizeof(info);
				info.fMask = SIF_RANGE | SIF_PAGE;
				info.nMin = 0;
				info.nMax = rectangle.right - rightBound;
				info.nPage = 1;
				SetScrollInfo(myPanelWindow, SB_HORZ, &info, true);
			}
			ShowScrollBar(myPanelWindow, SB_VERT, vOversized);
			if (hOversized) {
				SCROLLINFO info;
				info.cbSize = sizeof(info);
				info.fMask = SIF_RANGE | SIF_PAGE;
				info.nMin = 0;
				info.nMax = rectangle.bottom - bottomBound;
				info.nPage = 1;
				SetScrollInfo(myPanelWindow, SB_VERT, &info, true);
			}
		}
		updateElementSize();
		myDoLayout = false;
	}
}

const std::string &W32DialogPanel::caption() const {
	return myCaption;
}

void W32DialogPanel::hScroll(WORD command) {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	GetScrollInfo(myPanelWindow, SB_HORZ, &info);
	int position = info.nPos;
	switch (command) {
		case SB_LINELEFT:
			info.nPos -= 1;
			break;
		case SB_LINERIGHT:
			info.nPos += 1;
			break;
		case SB_PAGELEFT:
			info.nPos -= info.nPage;
			break;
		case SB_PAGERIGHT:
			info.nPos += info.nPage;
			break;
		case SB_THUMBTRACK:
			info.nPos = info.nTrackPos;
			break;
	}
	info.fMask = SIF_POS;
	SetScrollInfo(myPanelWindow, SB_HORZ, &info, true);
	GetScrollInfo(myPanelWindow, SB_HORZ, &info);
	if (info.nPos != position) {
		ScrollWindow(myPanelWindow, position - info.nPos, 0, 0, 0);
		UpdateWindow(myPanelWindow);
	}
}

void W32DialogPanel::vScroll(WORD command) {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	GetScrollInfo(myPanelWindow, SB_HORZ, &info);
	int position = info.nPos;
	switch (command) {
		case SB_TOP:
			info.nPos = info.nMin;
			break;
		case SB_BOTTOM:
			info.nPos = info.nMax;
			break;
		case SB_LINEUP:
			info.nPos -= 1;
			break;
		case SB_LINEDOWN:
			info.nPos += 1;
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
	SetScrollInfo(myPanelWindow, SB_VERT, &info, true);
	GetScrollInfo(myPanelWindow, SB_VERT, &info);
	if (info.nPos != position) {
		ScrollWindow(myPanelWindow, 0, position - info.nPos, 0, 0);
		UpdateWindow(myPanelWindow);
	}
}
