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

#include "ZLWin32FileInputStream.h"
#include "ZLWin32FSManager.h"
#include "../util/W32WCHARUtil.h"

ZLWin32FileInputStream::ZLWin32FileInputStream(const std::string &name) : myName(name), myNeedRepositionToStart(false) {
	myFile = 0;
}

ZLWin32FileInputStream::~ZLWin32FileInputStream() {
	//close();
	if (myFile != 0) {
		CloseHandle(myFile);
	}
}

bool ZLWin32FileInputStream::open() {
	//close();
	if (myFile == 0) {
		ZLUnicodeUtil::Ucs2String wPath = ZLWin32FSManager::longFilePath(myName);
		myFile = CreateFileW(::wchar(wPath), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		myOffset = 0;
	} else {
		//fseek(myFile, 0, SEEK_SET);
		myNeedRepositionToStart = true;
	}
	return myFile != 0;
}

size_t ZLWin32FileInputStream::read(char *buffer, size_t maxSize) {
	if (buffer != 0) {
		if (myNeedRepositionToStart) {
			myOffset = SetFilePointer(myFile, 0, 0, FILE_BEGIN);
			myNeedRepositionToStart = false;
		}
		DWORD realSize;
		ReadFile(myFile, buffer, maxSize, &realSize, 0);
		myOffset += realSize;
		return realSize;
	} else {
		if (myNeedRepositionToStart) {
			myOffset = SetFilePointer(myFile, maxSize, 0, FILE_BEGIN);
			myNeedRepositionToStart = false;
			return myOffset;
		} else {
			size_t oldOffset = myOffset;
			myOffset = SetFilePointer(myFile, maxSize, 0, FILE_CURRENT);
			return myOffset - oldOffset;
		}
	}
}

void ZLWin32FileInputStream::close() {
	//if (myFile != 0) {
	//	fclose(myFile);
	//	myFile = 0;
	//}
}

size_t ZLWin32FileInputStream::sizeOfOpened() {
	if (myFile == 0) {
		return 0;
	}
	return GetFileSize(myFile, 0);
}

void ZLWin32FileInputStream::seek(int offset, bool absoluteOffset) {
	if (myNeedRepositionToStart) {
		absoluteOffset = true;
		myNeedRepositionToStart = false;
	}
	myOffset = SetFilePointer(myFile, offset, 0, absoluteOffset ? FILE_BEGIN : FILE_CURRENT);
}

size_t ZLWin32FileInputStream::offset() const {
	return myNeedRepositionToStart ? 0 : myOffset;
}
