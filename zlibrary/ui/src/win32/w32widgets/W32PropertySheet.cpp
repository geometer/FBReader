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

#include <windows.h>
#include <prsht.h>

#include <ZLDialogManager.h>

#include "W32DialogPanel.h"

#include "../../../../core/src/win32/util/W32WCHARUtil.h"

bool W32PropertySheet::ourPropertySheetStarted = false;

W32PropertySheet::W32PropertySheet(HWND mainWindow, const std::string &caption, bool showApplyButton) : myMainWindow(mainWindow), myDialogWindow(0), myShowApplyButton(showApplyButton) {
	::createNTWCHARString(myCaption, caption);
}

W32DialogPanel &W32PropertySheet::createPanel(const std::string &name) {
	W32DialogPanel *panel = new W32DialogPanel(myMainWindow, name);
	myPanels.push_back(panel);
	return *panel;
}

bool W32PropertySheet::run(const std::string &selectedTabName) {
	PROPSHEETHEADER header;
	header.dwSize = sizeof(header);
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP | PSH_USECALLBACK;
	if (!myShowApplyButton) {
		header.dwFlags |= PSH_NOAPPLYNOW;
	}
	header.hInstance = 0;
	header.hIcon = 0;
	header.pszCaption = ::wchar(myCaption);
	header.pfnCallback = PSCallback;
	header.hwndParent = myMainWindow;
	header.nPages = myPanels.size();
	header.nStartPage = 0;

	short maxPanelWidth = 0;
	for (PanelList::const_iterator it = myPanels.begin(); it != myPanels.end(); ++it) {
		(*it)->calculateSize();
		maxPanelWidth = std::max(maxPanelWidth, (*it)->size().Width);
		if ((*it)->caption() == selectedTabName) {
			header.nStartPage = it - myPanels.begin();
		}
	}
	for (PanelList::iterator it = myPanels.begin(); it != myPanels.end(); ++it) {
		W32Widget::Size size = (*it)->size();
		size.Width = maxPanelWidth;
		(*it)->setSize(size);
	}

	PROPSHEETPAGE *pages = new PROPSHEETPAGE[header.nPages];
	for (size_t i = 0; i < header.nPages; ++i) {
		W32DialogPanel &panel = *myPanels[i];
		pages[i].dwSize = sizeof(PROPSHEETPAGE);
		pages[i].dwFlags = PSP_DLGINDIRECT;
		pages[i].hInstance = 0;
		pages[i].pResource = panel.dialogTemplate();
		pages[i].hIcon = 0;
	 	pages[i].pszTitle = 0;
		pages[i].pfnDlgProc = StaticCallback;
		pages[i].lParam = (LPARAM)&panel;
		pages[i].pfnCallback = 0;
		pages[i].pcRefParent = 0;
	}

	header.ppsp = pages;

	ourPropertySheetStarted = true;
	int code = PropertySheet(&header);
	delete[] pages;
	W32DialogPanel::ourPanels.clear();
	return code == 1;
}

int CALLBACK W32PropertySheet::PSCallback(HWND hDialog, UINT message, LPARAM lParam) {
	if (message == PSCB_INITIALIZED) {
		SendMessage(hDialog, PSM_CHANGED, 0, 0);
	}
	return 0;
}

BOOL CALLBACK W32PropertySheet::StaticCallback(HWND hPage, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_SHOWWINDOW:
			if (ourPropertySheetStarted) {
				ourPropertySheetStarted = false;
				HWND parent = GetParent(hPage);
				if (parent != 0) {
					HWND main = GetParent(parent);
					if (main != 0) {
						RECT mainRectangle;
						RECT rectangle;
						GetWindowRect(main, &mainRectangle);
						GetWindowRect(parent, &rectangle);
						int left = (mainRectangle.left + mainRectangle.right + rectangle.left - rectangle.right) / 2; 
						int top = (mainRectangle.top + mainRectangle.bottom + rectangle.top - rectangle.bottom) / 2; 
						SetWindowPos(
							parent, 0, left, top, 0, 0,
							SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE
						);
					}
					HWND okButton = GetDlgItem(GetParent(hPage), IDOK);
					if (okButton != 0) {
						::setWindowText(okButton, ZLDialogManager::buttonName(ZLDialogManager::OK_BUTTON));
					}
					HWND cancelButton = GetDlgItem(GetParent(hPage), IDCANCEL);
					if (cancelButton != 0) {
						::setWindowText(cancelButton, ZLDialogManager::buttonName(ZLDialogManager::CANCEL_BUTTON));
					}
					HWND applyButton = GetDlgItem(GetParent(hPage), 0x3021);
					if (applyButton != 0) {
						::setWindowText(applyButton, ZLDialogManager::buttonName(ZLDialogManager::APPLY_BUTTON));
					}
				}
			}
			return true;
		case WM_INITDIALOG:
			((W32DialogPanel*)((PROPSHEETPAGE*)lParam)->lParam)->init(hPage);
			return true;
		case WM_COMMAND:
		{
			W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
			if (panel != 0) {
				return panel->commandCallback(wParam);
			}
			break;
		}
		case WM_NOTIFY:
		{
			PSHNOTIFY &notification = *(PSHNOTIFY*)lParam;
			switch ((int)notification.hdr.code) {
				case PSN_APPLY:
				{
					W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
					if (panel != 0) {
						panel->fireEvent(W32DialogPanel::APPLY_BUTTON_PRESSED_EVENT);
						PostMessage(GetParent(hPage), PSM_CHANGED, (WPARAM)hPage, 0);
						return true;
					}
					break;
				}
				case PSN_SETACTIVE:
				{
					W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
					if (panel != 0) {
						panel->fireEvent(W32DialogPanel::PANEL_SELECTED_EVENT);
						return true;
					}
					break;
				}
			}
			break;
		}
		case WM_HSCROLL:
		{
			W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
			if (panel != 0) {
				panel->hScroll(LOWORD(wParam));
			}
			return true;
		}
		case WM_VSCROLL:
		{
			W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
			if (panel != 0) {
				panel->vScroll(LOWORD(wParam));
			}
			return true;
		}
		case WM_DRAWITEM:
		{
			W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
			if (panel != 0) {
				return panel->drawItemCallback(wParam, *(DRAWITEMSTRUCT*)lParam);
			}
			return false;
		}
		default:
			if (message == W32DialogPanel::LAYOUT_MESSAGE) {
				W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
				if (panel != 0) {
					panel->layout();
					return true;
				}
			}
			break;
	}
	return false;
}
