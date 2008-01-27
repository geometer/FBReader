/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <unistd.h>
#include <sys/stat.h>
#include <windows.h>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLWin32FSManager.h"
#include "ZLWin32RootDir.h"

static std::string getPwdDir() {
	char pwd[2048];
	return (getcwd(pwd, 2047) != 0) ? pwd : "";
}

static std::string getHomeDir() {
	char *home = getenv("USERPROFILE");
	return (home != 0) ? home : "";
}

ZLFSDir *ZLWin32FSManager::createPlainDirectory(const std::string &path) const {
	if (path.empty()) {
		return new ZLWin32RootDir();
	} else {
		return ZLPosixFSManager::createPlainDirectory(path);
	}
}

void ZLWin32FSManager::normalize(std::string &path) const {
	if (path.empty()) {
		return;
	}

	static std::string HomeDir = getHomeDir();
	static std::string PwdDir = getPwdDir();
	static std::string APPattern = "%APPLICATION_PATH%";
	static std::string AP;
	if (AP.empty()) {
		AP = _pgmptr;
		int index = AP.rfind('\\');
		if (index != -1) {
			AP = AP.substr(0, index);
		}
	}

	if (path[0] == '~') {
		path = HomeDir + path.substr(1);
	} else if (ZLStringUtil::stringStartsWith(path, APPattern)) {
		path = AP + path.substr(APPattern.length());
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
	return (mkdir(path.c_str()) == 0) ? createPlainDirectory(path) : 0;
}

std::string ZLWin32FSManager::convertFilenameToUtf8(const std::string &name) const {
	int len = name.length();
	ZLUnicodeUtil::Ucs2String ucs2String;
	ucs2String.insert(ucs2String.end(), len, 0);
	int ucs2Len = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, name.c_str(), len, (WCHAR*)&ucs2String.front(), len);
	ucs2String.erase(ucs2String.begin() + ucs2Len, ucs2String.end());

	std::string utf8String;
	ZLUnicodeUtil::ucs2ToUtf8(utf8String, ucs2String);
	return utf8String;
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

void ZLWin32FSManager::moveFile(const std::string &oldName, const std::string &newName) {
	remove(newName.c_str());
	rename(oldName.c_str(), newName.c_str());
}

ZLFileInfo ZLWin32FSManager::fileInfo(const std::string &path) const {
	if (path.empty()) {
		ZLFileInfo info;
		info.Exists = true;
		info.Size = 0;
		info.MTime = 0;
		info.IsDirectory = true;
		return info;
	} else {
		return ZLPosixFSManager::fileInfo(path);
	}
}

void ZLWin32FSManager::getStat(const std::string &path, bool /*includeSymlinks*/, struct stat &fileInfo) const {
	stat(path.c_str(), &fileInfo);
}
