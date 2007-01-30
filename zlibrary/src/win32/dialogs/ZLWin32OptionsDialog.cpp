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
//#include "ZLWin32Util.h"

ZLWin32OptionsDialog::ZLWin32OptionsDialog(HWND mainWindow, const std::string &id, const std::string &caption) : ZLDesktopOptionsDialog(id), myMainWindow(mainWindow), myCaption(caption) {
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
	return "";
}

void ZLWin32OptionsDialog::selectTab(const std::string &name) {
	/*
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		gtk_notebook_set_current_page(myNotebook, it - myTabNames.begin());
	}
	*/
}

static BOOL CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	return false;
}


bool ZLWin32OptionsDialog::run() {
	PROPSHEETPAGE *pages = new PROPSHEETPAGE[myTabs.size()];
	for (int i = 0; i < myTabs.size(); ++i) {
		pages[i].dwSize = sizeof(pages[i]);
		pages[i].dwFlags = PSP_DLGINDIRECT;
		pages[i].hInstance = 0;
		ZLWin32DialogPanel *panel = new ZLWin32DialogPanel(DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION, 20, 20, 120, 120, myTabNames[i]);
		pages[i].pResource = panel->allocate(); // TODO: !!!
		pages[i].hIcon = 0;
	 	pages[i].pszTitle = 0; // TODO: !!!
		pages[i].pfnDlgProc = DialogProc; // TODO: !!!
		pages[i].lParam = 0;
		pages[i].pfnCallback = 0;
		pages[i].pcRefParent = 0;
	}

	PROPSHEETHEADER header;
	header.dwSize = sizeof(header);
	header.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
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

void ZLWin32OptionsDialog::setSize(int width, int height) {
	//gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int ZLWin32OptionsDialog::width() const {
	/*
	int _width, _height;

	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, &_height);

	return _width;
	*/
	return 200;
}

int ZLWin32OptionsDialog::height() const {
/*
	int _width, _height;

	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, &_height);

	return _height;
*/
	return 200;
}
