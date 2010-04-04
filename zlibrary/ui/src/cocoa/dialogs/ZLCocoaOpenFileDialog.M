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

#include "ZLCocoaOpenFileDialog.h"

ZLCocoaOpenFileDialog::ZLCocoaOpenFileDialog(const std::string &title, const std::string &filePath, const Filter &filter) : myTitle(title), myFilePath(filePath), myFilter(filter) {
}

bool ZLCocoaOpenFileDialog::run() {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setTitle:[NSString stringWithUTF8String:myTitle.c_str()]];
	bool code = [panel runModal] == NSFileHandlingPanelOKButton;

	myFilePath.erase();
	NSArray *urls = [panel URLs];
	if ([urls count] > 0) {
		myFilePath = [[[urls objectAtIndex:0] path] UTF8String];
	}

	return code;
}

std::string ZLCocoaOpenFileDialog::filePath() const {
	return myFilePath;
}

std::string ZLCocoaOpenFileDialog::directoryPath() const {
	return std::string();
}
