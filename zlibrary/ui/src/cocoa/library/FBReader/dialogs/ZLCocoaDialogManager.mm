/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "ZLCocoaDialogManager.h"
#import "CocoaMain.h"
#import "Controller.h"
#import "Utils.h"

void ZLCocoaDialogManager::createApplicationWindow(ZLApplication *application) const {
	cocoaMain(0, NULL);
}

void ZLCocoaDialogManager::informationBox(const std::string &title, const std::string &message) const {
	[[Controller instance] showDialogWithTitle:[Utils stringWithCxxString:title]
									   Message:[Utils stringWithCxxString:message]
										 Style:NSInformationalAlertStyle];
}

void ZLCocoaDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	[[Controller instance] showDialogWithTitle:[Utils stringWithCxxString:dialogTitle(key)]
									   Message:[Utils stringWithCxxString:message]
										 Style:NSWarningAlertStyle];
}

int ZLCocoaDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	return [[Controller instance] showQuestionWithTitle:[Utils stringWithCxxString:dialogTitle(key)]
												Message:[Utils stringWithCxxString:message]
												Button0:[Utils cocoaButtonName:button0]
												Button1:[Utils cocoaButtonName:button1]
												Button2:[Utils cocoaButtonname:button2]];	
}

void ZLCocoaDialogManager::wait(const ZLResourceKey &key, ZLRunnable &runnable) const {
	// TODO
	runnable.run();
}

#if 0
shared_ptr<ZLDialog> ZLGtkDialogManager::createDialog(const ZLResourceKey &key) const {
	return new ZLGtkDialog(resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLGtkDialogManager::createOptionsDialog(const ZLResourceKey &id, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	return new ZLGtkOptionsDialog(resource()[id], applyAction, showApplyButton);
}


bool ZLGtkDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	return ZLGtkSelectionDialog(dialogTitle(key), handler).runWithSize();
}


#endif