/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ZLOpenFileDialog.h"

#include "../filesystem/ZLFSDir.h"
#include "../filesystem/ZLZipDir.h"
#include "../util/ZLStringUtil.h"

ZLStringOption ZLOpenFileDialog::DirectoryOption("OpenFileDialog", "Directory", "~");
	
ZLOpenFileDialog::ZLOpenFileDialog() {
	std::string dirName =	DirectoryOption.value();
	// TODO: replace this code
	if (ZLStringUtil::stringEndsWith(dirName, ".zip")) {
		myCurrentDir = new ZLZipDir(dirName);
	} else {
		myCurrentDir = new ZLFSDir(dirName);
	}
}

ZLOpenFileDialog::~ZLOpenFileDialog() {
	DirectoryOption.setValue(myCurrentDir->name());
}

bool ZLOpenFileDialog::isDirectoryVisible(const std::string &name) {
	return (name.length() > 0) && (name[0] != '.');
}

bool ZLOpenFileDialog::isFileVisible(const std::string &name) {
	if ((name.length() == 0) || (name[0] == '.')) {
		return false;
	}
	return
		ZLStringUtil::stringEndsWith(name, ".html") ||
		ZLStringUtil::stringEndsWith(name, ".zip") ||
		ZLStringUtil::stringEndsWith(name, ".fb2");
}
