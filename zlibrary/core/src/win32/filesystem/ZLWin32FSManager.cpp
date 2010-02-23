/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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
#include <shlobj.h>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLWin32FSManager.h"
#include "ZLWin32RootDir.h"
#include "ZLWin32FSDir.h"
#include "ZLWin32FileInputStream.h"
#include "ZLWin32FileOutputStream.h"
#include "../util/W32WCHARUtil.h"

#define CSIDL_MYDOCUMENTS 5

static std::string getPwdDir() {
	ZLUnicodeUtil::Ucs2String buffer;
	buffer.assign(2048, 0);
	GetCurrentDirectoryW(2047, (WCHAR*)&buffer[0]);
	std::string result;
	ZLUnicodeUtil::ucs2ToUtf8(result, buffer);
	result.erase(result.find('\0'));
	return result;
}

static std::string getHomeDir() {
	ZLUnicodeUtil::Ucs2String buffer;
	buffer.assign(2048, 0);
	SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, (WCHAR*)&buffer[0]);
	std::string result;
	ZLUnicodeUtil::ucs2ToUtf8(result, buffer);
	result.erase(result.find('\0'));
	return result;
}

static std::string getAppDir() {
	ZLUnicodeUtil::Ucs2String buffer;
	buffer.assign(2048, 0);
	GetModuleFileNameW(0, (WCHAR*)&buffer[0], 2047);
	std::string result;
	ZLUnicodeUtil::ucs2ToUtf8(result, buffer);
	result.erase(result.rfind('\\'));
	return result;
}

ZLFSDir *ZLWin32FSManager::createPlainDirectory(const std::string &path) const {
	if (path.empty()) {
		return new ZLWin32RootDir();
	} else {
		return new ZLWin32FSDir(path);
	}
}

ZLInputStream *ZLWin32FSManager::createPlainInputStream(const std::string &path) const {
	return new ZLWin32FileInputStream(path);
}

ZLOutputStream *ZLWin32FSManager::createOutputStream(const std::string &path) const {
	return new ZLWin32FileOutputStream(path);
}

std::string ZLWin32FSManager::resolveSymlink(const std::string &path) const {
	return path;
}

void ZLWin32FSManager::normalizeRealPath(std::string &path) const {
	if (path.empty()) {
		return;
	}

	static std::string HomeDir = getHomeDir();
	static std::string PwdDir = getPwdDir();
	static std::string APPattern = "~~";
	static std::string AP = getAppDir();

	if (ZLStringUtil::stringStartsWith(path, APPattern)) {
		path = AP + path.substr(APPattern.length());
	} else if (path[0] == '~') {
		path = HomeDir + path.substr(1);
	} else if ((path.length() > 1) && (path[1] != ':') &&
							!ZLStringUtil::stringStartsWith(path, "\\\\")) {
		path = PwdDir + "\\" + path;
	}
	
	int index;
	while ((index = path.find("\\..")) != -1) {
		int prevIndex = path.rfind('\\', index - 1);
		if (prevIndex == -1) {
			break;
		}
		path.erase(prevIndex, index + 3 - prevIndex);
	}
	while ((index = path.find("\\\\", 1)) > 0) {
		path.erase(index, 1);
	}

	if (!path.empty()) {
		path[0] = toupper(path[0]);
	}
}

ZLFSDir *ZLWin32FSManager::createNewDirectory(const std::string &path) const {
	std::vector<ZLUnicodeUtil::Ucs2String> subpaths;
	std::string current = path;

	while (current.length() > 3) {
		WIN32_FILE_ATTRIBUTE_DATA data;
		ZLUnicodeUtil::Ucs2String wPath = longFilePath(current);
		const bool exists = GetFileAttributesEx(::wchar(wPath), GetFileExInfoStandard, &data);
		if (exists && ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)) {
			return 0;
		}
		subpaths.push_back(wPath);
		int index = current.rfind('\\');
		if (index == -1) {
			return 0;
		}
		current.erase(index);
	}

	for (int i = subpaths.size() - 1; i >= 0; --i) {
		if (!CreateDirectory(::wchar(subpaths[i]), 0) && (GetLastError() != ERROR_ALREADY_EXISTS)) {
			return 0;
		}
	}
	return createPlainDirectory(path);
}

std::string ZLWin32FSManager::convertFilenameToUtf8(const std::string &name) const {
	return name;
}

int ZLWin32FSManager::findArchiveFileNameDelimiter(const std::string &path) const {
	int index = path.rfind(':');
	return (index == 1) ? -1 : index;
}

static const std::string RootPath = "";

shared_ptr<ZLDir> ZLWin32FSManager::rootDirectory() const {
	return new ZLWin32RootDir();
}

const std::string &ZLWin32FSManager::rootDirectoryPath() const {
	return RootPath;
}

std::string ZLWin32FSManager::parentPath(const std::string &path) const {
	if (path.length() <= 3) {
		return RootPath;
	}
	int index = findLastFileNameDelimiter(path);
	std::string result = path.substr(0, index);
	return (result.length() == 2) ? result + '\\' : result;
}

ZLFileInfo ZLWin32FSManager::fileInfo(const std::string &path) const {
	ZLFileInfo info;
	if (path.empty()) {
		info.Exists = true;
		info.Size = 0;
		info.IsDirectory = true;
	} else {
		ZLUnicodeUtil::Ucs2String wPath = longFilePath(path);
		WIN32_FILE_ATTRIBUTE_DATA data;
		info.Exists = GetFileAttributesEx(::wchar(wPath), GetFileExInfoStandard, &data);
		if (info.Exists) {
			info.IsDirectory = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			info.Size = info.IsDirectory ? 0 : data.nFileSizeLow;
		}
	}
	return info;
}

bool ZLWin32FSManager::removeFile(const std::string &path) const {
	ZLUnicodeUtil::Ucs2String wPath = longFilePath(path);
	return DeleteFileW(::wchar(wPath));
}

ZLUnicodeUtil::Ucs2String ZLWin32FSManager::longFilePath(const std::string &path) {
	ZLUnicodeUtil::Ucs2String lfp;
	if (ZLStringUtil::stringStartsWith(path, "\\\\")) {
		::createNTWCHARString(lfp, "\\\\?\\UNC" + path.substr(1));
	} else {
		::createNTWCHARString(lfp, "\\\\?\\" + path);
	}
	return lfp;
}

bool ZLWin32FSManager::canRemoveFile(const std::string &path) const {
	ZLUnicodeUtil::Ucs2String wPath = longFilePath(path);
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExW(::wchar(wPath), GetFileExInfoStandard, &data)) {
		return false;
	}
	return (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0;
}

