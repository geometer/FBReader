/*
 * Copyright (C) 2010-2011 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>

#include "ZLCocoaOpenFileDialog.h"

@interface ZLCocoaOpenFileDialogDelegate : NSObject /*<NSOpenSavePanelDelegate>*/ {
	const void *myFilter;
}

- (id)initWithFilter:(const void*)filter;
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL*)url;

@end

@implementation ZLCocoaOpenFileDialogDelegate

- (id)initWithFilter:(const void*)filter {
	self = [super init];
	if (self) {
		myFilter = filter;
	}
	return self;
}

- (BOOL)panel:(id)sender shouldEnableURL:(NSURL*)url {
	ZLFile file([[url path] UTF8String]);
	return
		file.isDirectory() ||
		((const ZLOpenFileDialog::Filter*)myFilter)->accepts(file);
}

@end

ZLCocoaOpenFileDialog::ZLCocoaOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter) : myTitle(title), myDirectoryPath(directoryPath), myFilePath(filePath), myFilter(filter) {
}

bool ZLCocoaOpenFileDialog::run() {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setDelegate:[[ZLCocoaOpenFileDialogDelegate alloc] initWithFilter:&myFilter]];
	[panel setTitle:[NSString stringWithUTF8String:myTitle.c_str()]];

	if ([panel respondsToSelector:@selector(setDirectoryURL)]) {
		[panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:myDirectoryPath.c_str()]]];
	} else {
		[panel setDirectory:[NSString stringWithUTF8String:myDirectoryPath.c_str()]];
	}

	bool code = [panel runModal] == NSFileHandlingPanelOKButton;

	myFilePath.erase();
	const char *filePath = [[[panel URL] path] UTF8String];
	if (filePath != 0) {
		myFilePath = filePath;
	}

	if ([panel respondsToSelector:@selector(directoryURL)]) {
		myDirectoryPath = [[[panel directoryURL] path] UTF8String];
	} else {
		myDirectoryPath = [[panel directory] UTF8String];
	}

	return code;
}

std::string ZLCocoaOpenFileDialog::filePath() const {
	return myFilePath;
}

std::string ZLCocoaOpenFileDialog::directoryPath() const {
	return myDirectoryPath;
}
