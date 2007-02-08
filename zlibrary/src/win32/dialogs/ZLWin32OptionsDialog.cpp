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

#include <iostream>

#include <windows.h>
#include <prsht.h>

#include "ZLWin32OptionsDialog.h"
#include "ZLWin32DialogContent.h"
#include "../util/ZLWin32WCHARUtil.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(HWND mainWindow, const std::string &id, const std::string &caption) : ZLOptionsDialog(id), myMainWindow(mainWindow), myTabIndex(0) {
	::createNTWCHARString(myCaption, caption);
}

ZLWin32OptionsDialog::~ZLWin32OptionsDialog() {
	for (std::vector<ZLWin32DialogContent*>::iterator tab = myTabs.begin(); tab != myTabs.end(); ++tab) {
		delete *tab;
	}
}

ZLDialogContent &ZLWin32OptionsDialog::createTab(const std::string &name) {
	ZLWin32DialogContent *tab = new ZLWin32DialogContent();
/*
	Win32Widget *label = gtk_label_new(name.c_str());

	gtk_notebook_append_page(myNotebook, tab->widget(), label);
*/

	myTabs.push_back(tab);
	myTabNames.push_back(name);

	return *tab;
}

const std::string &ZLWin32OptionsDialog::selectedTabName() const {
	return myTabNames[myTabIndex];
}

void ZLWin32OptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		myTabIndex = it - myTabNames.begin();
	}
}

void ZLWin32OptionsDialog::onEvent(const std::string &event, W32EventSender &sender) {
	if (event == W32DialogPanel::SELECTED_EVENT) {
		selectTab(((W32DialogPanel&)sender).caption());
	}
}

int CALLBACK PropSheetProc(HWND hDialog, UINT message, LPARAM lParam) {
	/*
	if (message == PSCB_BUTTONPRESSED) {
		if (lParam == PSBTN_OK) {
			std::cerr << "ACCEPT\n";
		}
	}
	*/
	/*
	if (message == PSCB_PRECREATE) {
		std::cerr << (void*)lParam << "\n";
		DLGTEMPLATE &dlg = *(DLGTEMPLATE*)lParam;
		std::cerr << dlg.style << " : " << dlg.dwExtendedStyle << "\n";
		std::cerr << dlg.x << " : " << dlg.y << " : " << dlg.cx << "x" << dlg.cy << "\n";
	}
	if (message == PSCB_INITIALIZED) {
		RECT rectanlge;
		GetWindowRect(hDialog, &rectanlge);
		//std::cerr << "dlg = " << rectanlge.right - rectanlge.left + 1 << "x" << rectanlge.bottom - rectanlge.top + 1 << "\n";
		std::cerr << hDialog << "\n";
		char buffer[100];
		SetWindowPos(hDialog, 0, 0, 0, 500, 500, 0);
		GetWindowText(hDialog, buffer, 99);
		std::cerr << buffer << "\n";
	}
	*/
	return 0;
}

bool ZLWin32OptionsDialog::run() {
	short maxPanelWidth = 0;
	for (size_t i = 0; i < myTabs.size(); ++i) {
		W32DialogPanel *panel = new W32DialogPanel(myMainWindow, myTabNames[i]);
		panel->setListener(this);
		panel->setElement(myTabs[i]->content());
		W32Table &table = (W32Table&)*myTabs[i]->content();
		const int charHeight = panel->charDimension().Height;
		table.setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
		table.setSpacings(charHeight / 2, charHeight);
		panel->calculateSize();
		maxPanelWidth = std::max(maxPanelWidth, panel->size().Width);
		myPanels.push_back(panel);
	}
	for (size_t i = 0; i < myTabs.size(); ++i) {
		W32DialogPanel &panel = *myPanels[i];
		W32Element::Size size = panel.size();
		size.Width = maxPanelWidth;
		panel.setSize(size);
	}
	PROPSHEETPAGE *pages = new PROPSHEETPAGE[myTabs.size()];
	for (size_t i = 0; i < myTabs.size(); ++i) {
		W32DialogPanel &panel = *myPanels[i];
		pages[i].dwSize = sizeof(pages[i]);
		pages[i].dwFlags = PSP_DLGINDIRECT;
		pages[i].hInstance = 0;
		pages[i].pResource = panel.dialogTemplate();
		pages[i].hIcon = 0;
	 	pages[i].pszTitle = 0;
		pages[i].pfnDlgProc = W32DialogPanel::PSStaticCallback;
		pages[i].lParam = (LPARAM)&panel;
		pages[i].pfnCallback = 0;
		pages[i].pcRefParent = 0;
	}

	PROPSHEETHEADER header;
	header.dwSize = sizeof(header);
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP | PSH_USECALLBACK;
	header.hwndParent = myMainWindow;
	header.hInstance = 0;
	header.hIcon = 0;
	header.pszCaption = ::wchar(myCaption);
	header.nPages = myTabs.size();
	header.nStartPage = myTabIndex;
	header.ppsp = pages;
	header.pfnCallback = PropSheetProc; // TODO: !!!

	bool result = PropertySheet(&header) == 1;
	if (result) {
		for (std::vector<ZLWin32DialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
			(*it)->accept();
		}
	}
	return result;
}
