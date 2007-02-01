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

#include "../dialogElements/W32DialogPanel.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(HWND mainWindow, const std::string &id, const std::string &caption) : ZLOptionsDialog(id), myMainWindow(mainWindow), myCaption(caption) {
/*
	myDialog = createWin32Dialog(caption);

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
	gtk_dialog_add_button (myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button (myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), TRUE, TRUE, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
*/
}

ZLWin32OptionsDialog::~ZLWin32OptionsDialog() {
	for (std::vector<ZLWin32DialogContent*>::iterator tab = myTabs.begin(); tab != myTabs.end(); ++tab) {
		delete *tab;
	}

/*
	destroyWin32Dialog(myDialog);
*/
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
	PROPSHEETPAGE *pages = new PROPSHEETPAGE[myTabs.size()];
	for (size_t i = 0; i < myTabs.size(); ++i) {
		pages[i].dwSize = sizeof(pages[i]);
		pages[i].dwFlags = PSP_DLGINDIRECT;
		pages[i].hInstance = 0;
		// TODO: !!!
		W32DialogPanel *panel = new W32DialogPanel(myMainWindow, myTabNames[i]);
		panel->setElement(myTabs[i]->content());
		pages[i].pResource = panel->dialogTemplate();
		pages[i].hIcon = 0;
	 	pages[i].pszTitle = 0; // TODO: !!!
		pages[i].pfnDlgProc = W32DialogPanel::StaticCallback;
		pages[i].lParam = (LPARAM)panel;
		pages[i].pfnCallback = 0;
		pages[i].pcRefParent = 0;
	}

	PROPSHEETHEADER header;
	header.dwSize = sizeof(header);
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
	header.hwndParent = myMainWindow;
	header.hInstance = 0;
	header.hIcon = 0;
	header.pszCaption = TEXT(myCaption.c_str());
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
