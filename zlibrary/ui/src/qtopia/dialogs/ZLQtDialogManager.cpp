/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qclipboard.h>

#include "ZLQtDialogManager.h"
#include "ZLQtDialog.h"
#include "ZLQtOptionsDialog.h"
#include "ZLQtSelectionDialog.h"
#include "ZLQtWaitMessage.h"
#include "ZLQtUtil.h"

#include "../application/ZLQtApplicationWindow.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	myApplicationWindow = new ZLQtApplicationWindow(application);
}

void ZLQtDialogManager::fullScreenWorkaround() const {
	if (myApplicationWindow != 0) {
		myApplicationWindow->fullScreenWorkaround();
	}
}

shared_ptr<ZLOptionsDialog> ZLQtDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool) const {
	return new ZLQtOptionsDialog(resource()[key], applyAction);
}

shared_ptr<ZLDialog> ZLQtDialogManager::createDialog(const ZLResourceKey &key) const {
	return new ZLQtDialog(myApplicationWindow->mainWindow(), resource()[key]);
}

void ZLQtDialogManager::informationBox(const ZLResourceKey &key, const std::string &message) const {
	QMessageBox::information(myApplicationWindow->mainWindow(), ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(OK_BUTTON));
	fullScreenWorkaround();
}

void ZLQtDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	QMessageBox::critical(myApplicationWindow->mainWindow(), ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(OK_BUTTON));
	fullScreenWorkaround();
}

int ZLQtDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	int code = QMessageBox::information(myApplicationWindow->mainWindow(), ::qtString(dialogTitle(key)), ::qtString(message), ::qtButtonName(button0), ::qtButtonName(button1), ::qtButtonName(button2));
	fullScreenWorkaround();
	return code;
}

bool ZLQtDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	bool result = ZLQtSelectionDialog(dialogTitle(key), handler).run();
	fullScreenWorkaround();
	return result;
}

void ZLQtDialogManager::wait(const ZLResourceKey &key, ZLRunnable &runnable) const {
	ZLQtWaitMessage waitMessage(waitMessageText(key));
	runnable.run();
	fullScreenWorkaround();
}

bool ZLQtDialogManager::isClipboardSupported(ClipboardType type) const {
	return type == CLIPBOARD_MAIN;
}

void ZLQtDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if ((type == CLIPBOARD_MAIN) && !text.empty()) {
		qApp->clipboard()->setText(::qtString(text));
	}
}
