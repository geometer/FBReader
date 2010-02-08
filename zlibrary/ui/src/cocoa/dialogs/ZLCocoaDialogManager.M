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

#import <Cocoa/Cocoa.h>

#include <ZLDialog.h>
#include <ZLOptionsDialog.h>
#include <ZLProgressDialog.h>

//#include "../image/ZLCocoaImageManager.h"
#include "ZLCocoaDialogManager.h"
//#include "ZLCocoaDialog.h"
//#include "ZLCocoaOptionsDialog.h"
//#include "ZLCocoaSelectionDialog.h"
#include "ZLCocoaProgressDialog.h"
#include "ZLCocoaOpenFileDialog.h"
//#include "ZLCocoaUtil.h"

static std::string cleanString(const std::string &str) {
	int index = str.find('&');
	if (index == -1) {
		return str;
	}
	std::string result = str;
	result.erase(index, 1);
	return result;
}

shared_ptr<ZLDialog> ZLCocoaDialogManager::createDialog(const ZLResourceKey &key) const {
	//return new ZLCocoaDialog(resource()[key]);
	return 0;
}

shared_ptr<ZLOptionsDialog> ZLCocoaDialogManager::createOptionsDialog(const ZLResourceKey &id, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	//return new ZLCocoaOptionsDialog(resource()[id], applyAction, showApplyButton);
	return 0;
}

void ZLCocoaDialogManager::informationBox(const std::string &title, const std::string &message) const {
	NSRunInformationalAlertPanel(
		[NSString stringWithUTF8String:title.c_str()],
		[NSString stringWithUTF8String:message.c_str()],
		[NSString stringWithUTF8String:cleanString(buttonName(OK_BUTTON)).c_str()],
		nil,
		nil
	);
}

void ZLCocoaDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	NSRunCriticalAlertPanel(
		[NSString stringWithUTF8String:dialogTitle(key).c_str()],
		[NSString stringWithUTF8String:message.c_str()],
		[NSString stringWithUTF8String:cleanString(buttonName(OK_BUTTON)).c_str()],
		nil,
		nil
	);
}

int ZLCocoaDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	const int code = NSRunInformationalAlertPanel(
		[NSString stringWithUTF8String:dialogTitle(key).c_str()],
		[NSString stringWithUTF8String:message.c_str()],
		button0.Name.empty() ? nil : [NSString stringWithUTF8String:cleanString(buttonName(button0)).c_str()],
		button1.Name.empty() ? nil : [NSString stringWithUTF8String:cleanString(buttonName(button1)).c_str()],
		button2.Name.empty() ? nil : [NSString stringWithUTF8String:cleanString(buttonName(button2)).c_str()]
	);
	switch (code) {
		case NSAlertDefaultReturn:
			return 0;
		case NSAlertAlternateReturn:
			return 1;
		case NSAlertOtherReturn:
			return 2;
	}
	return -1;
}

bool ZLCocoaDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	//return ZLCocoaSelectionDialog(dialogTitle(key), handler).runWithSize();
	return 0;
}

shared_ptr<ZLOpenFileDialog> ZLCocoaDialogManager::createOpenFileDialog(const std::string &directoryPath, const ZLOpenFileDialog::Filter &filter) const {
	return new ZLCocoaOpenFileDialog(directoryPath, filter);
}

shared_ptr<ZLProgressDialog> ZLCocoaDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	return new ZLCocoaProgressDialog(key);
}

bool ZLCocoaDialogManager::isClipboardSupported(ClipboardType) const {
	return false;
}

void ZLCocoaDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
}

void ZLCocoaDialogManager::setClipboardImage(const ZLImageData &image, ClipboardType type) const {
}
