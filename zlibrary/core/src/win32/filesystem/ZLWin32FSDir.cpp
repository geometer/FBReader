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

#include <windows.h>

#include "ZLWin32FSDir.h"
#include "../util/W32WCHARUtil.h"

void ZLWin32FSDir::collectSubDirs(std::vector<std::string> &names, bool /*includeSymlinks*/) {
	std::string pattern = path();
	if (pattern[pattern.length() - 1] != '\\') {
		pattern += '\\';
	}
	pattern += '*';
	ZLUnicodeUtil::Ucs2String wPattern;
	createNTWCHARString(wPattern, pattern);

	WIN32_FIND_DATAW findData;
	HANDLE handle = FindFirstFileW(::wchar(wPattern), &findData);
	ZLUnicodeUtil::Ucs2String wFileName;
	std::string fileName;
	if (handle != 0) {
		do {
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				continue;
			}
			wFileName.clear();
			fileName.erase();
			for (int i = 0; i < MAX_PATH; ++i) {
				if (findData.cFileName[i] != 0) {
					wFileName.push_back(findData.cFileName[i]);
				} else {
					break;
				}
			}
			ZLUnicodeUtil::ucs2ToUtf8(fileName, wFileName);
			if ((fileName != ".") && (fileName != "..")) {
				names.push_back(fileName);
			}
		} while (FindNextFileW(handle, &findData));
	}
}

void ZLWin32FSDir::collectFiles(std::vector<std::string> &names, bool /*includeSymlinks*/) {
	std::string pattern = path();
	if (pattern[pattern.length() - 1] != '\\') {
		pattern += '\\';
	}
	pattern += '*';
	ZLUnicodeUtil::Ucs2String wPattern;
	createNTWCHARString(wPattern, pattern);

	WIN32_FIND_DATAW findData;
	HANDLE handle = FindFirstFileW(::wchar(wPattern), &findData);
	ZLUnicodeUtil::Ucs2String wFileName;
	std::string fileName;
	if (handle != 0) {
		do {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}
			wFileName.clear();
			fileName.erase();
			for (int i = 0; i < MAX_PATH; ++i) {
				if (findData.cFileName[i] != 0) {
					wFileName.push_back(findData.cFileName[i]);
				} else {
					break;
				}
			}
			ZLUnicodeUtil::ucs2ToUtf8(fileName, wFileName);
			names.push_back(fileName);
		} while (FindNextFileW(handle, &findData));
	}
}
