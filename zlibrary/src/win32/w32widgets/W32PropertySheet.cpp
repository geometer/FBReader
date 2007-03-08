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

#include <windows.h>
#include <prsht.h>

#include "W32DialogPanel.h"

#include "W32WCHARUtil.h"

W32PropertySheet::W32PropertySheet(HWND mainWindow, const std::string &caption) : myMainWindow(mainWindow) {
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
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP | PSH_USECALLBACK;
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

	int code = PropertySheet(&header);
	delete[] pages;
	return code == 1;
}

int CALLBACK W32PropertySheet::PSCallback(HWND, UINT message, LPARAM lParam) {
	if (message == PSCB_PRECREATE) {
		DLGTEMPLATE &dlgTemplate = *(DLGTEMPLATE*)lParam;
		dlgTemplate.style |= DS_CENTER;
	}
	return 0;
}

BOOL CALLBACK W32PropertySheet::StaticCallback(HWND hPage, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:
			((W32DialogPanel*)((PROPSHEETPAGE*)lParam)->lParam)->init(hPage);
			return true;
		case WM_COMMAND:
		{
			W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
			if (panel != 0) {
				return panel->commandCallback(wParam);
			}
		}
		case WM_NOTIFY:
		{
			PSHNOTIFY &notification = *(PSHNOTIFY*)lParam;
			if ((int)notification.hdr.code == PSN_SETACTIVE) {
				W32DialogPanel *panel = W32DialogPanel::ourPanels[hPage];
				if (panel != 0) {
					panel->fireEvent(W32DialogPanel::PANEL_SELECTED_EVENT);
					return true;
				}
			}
			break;
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
