/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLDialog.h>
#include <ZLOptionsDialog.h>
#include <ZLProgressDialog.h>

//#include "../image/ZLCocoaImageManager.h"
#include "ZLCocoaDialogManager.h"
//#include "ZLCocoaDialog.h"
//#include "ZLCocoaOptionsDialog.h"
//#include "ZLCocoaSelectionDialog.h"
//#include "ZLCocoaProgressDialog.h"
//#include "ZLCocoaUtil.h"

shared_ptr<ZLDialog> ZLCocoaDialogManager::createDialog(const ZLResourceKey &key) const {
	//return new ZLCocoaDialog(resource()[key]);
	return 0;
}

shared_ptr<ZLOptionsDialog> ZLCocoaDialogManager::createOptionsDialog(const ZLResourceKey &id, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	//return new ZLCocoaOptionsDialog(resource()[id], applyAction, showApplyButton);
	return 0;
}

void ZLCocoaDialogManager::informationBox(const std::string &title, const std::string &message) const {
	//internalBox(GTK_STOCK_DIALOG_INFO, title, message);
}

void ZLCocoaDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	//internalBox(GTK_STOCK_DIALOG_ERROR, dialogTitle(key), message);
}

int ZLCocoaDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	//return internalBox(GTK_STOCK_DIALOG_QUESTION, dialogTitle(key), message, button0, button1, button2);
	return 0;
}

/*
int ZLCocoaDialogManager::internalBox(const gchar *icon, const std::string &title, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
}
*/

bool ZLCocoaDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	//return ZLCocoaSelectionDialog(dialogTitle(key), handler).runWithSize();
	return 0;
}

shared_ptr<ZLProgressDialog> ZLCocoaDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	/*
	return new ZLCocoaProgressDialog(
		!myDialogs.empty() ? myDialogs.top() : myWindow, key
	);
	*/
	return 0;
}

bool ZLCocoaDialogManager::isClipboardSupported(ClipboardType) const {
	return false;
}

void ZLCocoaDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
}

void ZLCocoaDialogManager::setClipboardImage(const ZLImageData &image, ClipboardType type) const {
}
