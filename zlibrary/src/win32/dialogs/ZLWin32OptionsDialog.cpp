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

#include "ZLWin32OptionsDialog.h"
#include "ZLWin32DialogContent.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(HWND mainWindow, const std::string &id, const std::string &caption) : ZLOptionsDialog(id), myMainWindow(mainWindow), myCaption(caption) {
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
	//return myTabNames[gtk_notebook_get_current_page(myNotebook)];
	// TODO: !!!
	return myTabNames[0];
}

void ZLWin32OptionsDialog::selectTab(const std::string &name) {
	/*
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		gtk_notebook_set_current_page(myNotebook, it - myTabNames.begin());
	}
	*/
}

bool ZLWin32OptionsDialog::run() {
	short maxPanelWidth = 0;
	for (size_t i = 0; i < myTabs.size(); ++i) {
		W32DialogPanel *panel = new W32DialogPanel(myMainWindow, myTabNames[i]);
		panel->setElement(myTabs[i]->content());
		W32Box &box = (W32Box&)*myTabs[i]->content();
		const int charHeight = panel->charDimension().Height;
		box.setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
		box.setSpacing(charHeight / 2);
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
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
	header.hwndParent = myMainWindow;
	header.hInstance = 0;
	header.hIcon = 0;
	header.pszCaption = myCaption.c_str();
	header.nPages = myTabs.size();
	header.nStartPage = 0; // TODO: !!!
	header.ppsp = pages;
	header.pfnCallback = 0; // TODO: !!!

	PropertySheet(&header);
	/*
	gint response = gtk_dialog_run(myDialog);

	switch (response) {
		case GTK_RESPONSE_ACCEPT:
			for (std::vector<ZLWin32DialogContent*>::iterator tab = myTabs.begin(); tab != myTabs.end(); ++tab)
				(*tab)->accept();
			break;
		case GTK_RESPONSE_REJECT:
			break;
	}

	gtk_widget_hide(GTK_WIDGET(myDialog));

	return response == GTK_RESPONSE_ACCEPT;
	*/
	return false;
}
