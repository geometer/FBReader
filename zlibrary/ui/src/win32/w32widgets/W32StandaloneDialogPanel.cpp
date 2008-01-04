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

#include "W32DialogPanel.h"

static const int FirstControlId = 2001;

const std::string W32StandaloneDialogPanel::CANCEL_EVENT = "Standalone Dialog Panel: Cancel";
const std::string W32StandaloneDialogPanel::OK_EVENT = "Standalone Dialog Panel: Ok";

W32StandaloneDialogPanel::W32StandaloneDialogPanel(HWND mainWindow, const std::string &caption) : W32DialogPanel(mainWindow, caption), myResizeable(false), myExitOnCancel(true), myExitOnOk(true) {
}

void W32StandaloneDialogPanel::setResizeable(bool resizeable) {
	myResizeable = resizeable;
}

DWORD W32StandaloneDialogPanel::style() const {
	return myResizeable ? (W32DialogPanel::style() | WS_SIZEBOX) : W32DialogPanel::style();
}

void W32StandaloneDialogPanel::endDialog(bool code) {
	if (myPanelWindow != 0) {
		EndDialog(myPanelWindow, code);
	}
}

BOOL CALLBACK W32StandaloneDialogPanel::StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_INITDIALOG) {
		((W32DialogPanel*)lParam)->init(hDialog);
		return true;
	} else if (message == WM_COMMAND) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			return panel->commandCallback(wParam);
		}
	} else if (message == WM_NOTIFY) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			return panel->notificationCallback(wParam, lParam);
		}
	} else if (message == WM_GETMINMAXINFO) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			POINT &minTrackSize = ((MINMAXINFO*)lParam)->ptMinTrackSize;
			W32Widget::Size minSize = panel->minimumSize();
			RECT r;
			r.left = 0;
			r.top = 0;
			r.right = minSize.Width;
			r.bottom = minSize.Height;
			MapDialogRect(hDialog, &r);
			minTrackSize.x = std::max(minTrackSize.x, r.right);
			minTrackSize.y += r.bottom;
			return true;
		}
	} else if (message == WM_SIZE) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			RECT r;
			r.left = 0;
			r.top = 0;
			r.right = 100;
			r.bottom = 100;
			MapDialogRect(hDialog, &r);
			panel->setSize(W32Widget::Size(
				(int)LOWORD(lParam) * 100 / r.right,
				(int)HIWORD(lParam) * 100 / r.bottom));
			panel->updateElementSize();
		}
		return false;
	} else if (message == WM_DRAWITEM) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			return panel->drawItemCallback(wParam, *(DRAWITEMSTRUCT*)lParam);
		}
	} else if (message == LAYOUT_MESSAGE) {
		W32DialogPanel *panel = ourPanels[hDialog];
		if (panel != 0) {
			panel->layout();
			return true;
		}
	}
	return false;
}

bool W32StandaloneDialogPanel::commandCallback(WPARAM wParam) {
	if (wParam == IDOK) {
		if (myExitOnOk) {
			endDialog(true);
		} else {
			fireEvent(OK_EVENT);
		}
		return true;
	}

	if (wParam == IDCANCEL) {
		if (myExitOnCancel) {
			endDialog(false);
		} else {
			fireEvent(CANCEL_EVENT);
		}
		return true;
	}

	return W32DialogPanel::commandCallback(wParam);
}

void W32StandaloneDialogPanel::setExitOnOk(bool exit) {
	myExitOnOk = exit;
}

void W32StandaloneDialogPanel::setExitOnCancel(bool exit) {
	myExitOnCancel = exit;
}

bool W32StandaloneDialogPanel::runDialog(short width, short height) {
	if ((width > 0) && (height > 0)) {
		calculateSize();
		W32Widget::Size size = minimumSize();
		size.Width = std::max(size.Width, width);
		size.Height = std::max(size.Height, height);
		setSize(size);
	}
	return DialogBoxIndirectParam(GetModuleHandle(0), dialogTemplate(), myMainWindow, StaticCallback, (LPARAM)this);
}
