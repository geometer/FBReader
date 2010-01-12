/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLWin32FileOutputStream.h"
#include "ZLWin32FSManager.h"
#include "../util/W32WCHARUtil.h"

ZLWin32FileOutputStream::ZLWin32FileOutputStream(const std::string &name) : myName(name), myHasErrors(false), myFile(0) {
}

ZLWin32FileOutputStream::~ZLWin32FileOutputStream() {
	close();
}

bool ZLWin32FileOutputStream::open() {
	close();

	myTemporaryName = myName + ".XYZT";
	ZLUnicodeUtil::Ucs2String wPath = ZLWin32FSManager::longFilePath(myTemporaryName);
	myFile = CreateFileW(::wchar(wPath), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	return myFile != 0;
}

void ZLWin32FileOutputStream::write(const char *data, size_t len) {
	DWORD size;
	myHasErrors |= !WriteFile(myFile, data, len, &size, 0);
}

void ZLWin32FileOutputStream::write(const std::string &str) {
	write(str.data(), str.length());
}

void ZLWin32FileOutputStream::close() {
	if (myFile != 0) {
		CloseHandle(myFile);
		myFile = 0;
		if (!myHasErrors) {
			ZLUnicodeUtil::Ucs2String wPath = ZLWin32FSManager::longFilePath(myName);
			ZLUnicodeUtil::Ucs2String wTPath = ZLWin32FSManager::longFilePath(myTemporaryName);
			MoveFileExW(::wchar(wTPath), ::wchar(wPath), MOVEFILE_REPLACE_EXISTING);
		}
	}
}
