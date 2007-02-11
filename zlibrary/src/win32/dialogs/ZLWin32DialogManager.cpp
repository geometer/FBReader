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

#include "ZLWin32DialogManager.h"
#include "ZLWin32Dialog.h"
#include "../application/ZLWin32ApplicationWindow.h"
#include "ZLWin32OptionsDialog.h"
#include "ZLWin32SelectionDialog.h"
/*
#include "ZLWin32WaitMessage.h"
*/
#include <ZLDialog.h>
#include <ZLSelectionDialog.h>

void ZLWin32DialogManager::createApplicationWindow(ZLApplication *application) const {
	myApplicationWindow = new ZLWin32ApplicationWindow(application);
}

shared_ptr<ZLDialog> ZLWin32DialogManager::createDialog(const std::string &title) const {
	return new ZLWin32Dialog(*myApplicationWindow, title);
}

shared_ptr<ZLOptionsDialog> ZLWin32DialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new ZLWin32OptionsDialog(*myApplicationWindow, id, title);
}

void ZLWin32DialogManager::informationBox(const std::string &title, const std::string &message) const {
	// TODO: use utf8 strings
	//MessageBox(myApplicationWindow->mainWindow(), message.c_str(), title.c_str(), MB_APPLMODAL | MB_ICONINFORMATION | MB_OK);
}

void ZLWin32DialogManager::errorBox(const std::string &title, const std::string &message) const {
	// TODO: use utf8 strings
	//MessageBox(myApplicationWindow->mainWindow(), message.c_str(), title.c_str(), MB_APPLMODAL | MB_ICONERROR | MB_OK);
}

int ZLWin32DialogManager::questionBox(const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	//return internalBox(GTK_STOCK_DIALOG_QUESTION, title, message, button0, button1, button2);
	return 0;
}

bool ZLWin32DialogManager::selectionDialog(const std::string &title, ZLTreeHandler &handler) const {
	return ZLWin32SelectionDialog(*myApplicationWindow, title, handler).runWithSize();
}

void ZLWin32DialogManager::wait(ZLRunnable &runnable, const std::string &message) const {
	//ZLWin32WaitMessage waitMessage(!myDialogs.empty() ? myDialogs.top() : myWindow, message);
	runnable.run();
}
