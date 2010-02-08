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

ZLCocoaOpenFileDialog::ZLCocoaOpenFileDialog(const std::string &directoryPath, const Filter &filter) : myDirectoryPath(directoryPath), myFilter(filter) {
}

bool ZLCocoaOpenFileDialog::run() {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	return [panel runModal] == NSFileHandlingPanelOKButton;
}

std::string ZLCocoaOpenFileDialog::filePath() const {
	return std::string();
}

std::string ZLCocoaOpenFileDialog::directoryPath() const {
	return std::string();
}
