/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <ZLStringUtil.h>

#include "ZLUnixFSManager.h"
#include "../../filesystem/ZLFSDir.h"

static std::string getPwdDir() {
	char *pwd = getenv("PWD");
	return (pwd != 0) ? pwd : "";
}

static std::string getHomeDir() {
	char *home = getenv("HOME");
	return (home != 0) ? home : "";
}

void ZLUnixFSManager::normalize(std::string &path) const {
	static std::string HomeDir = getHomeDir();
	static std::string PwdDir = getPwdDir();

	if (path.empty()) {
		path = PwdDir;
	} else if (path[0] == '~') {
		if ((path.length() == 1) || (path[1] == '/')) {
			path = HomeDir + path.substr(1);
		}
	} else if (path[0] != '/') {
		path = PwdDir + '/' + path;
	}
	int last = path.length() - 1;
	while ((last > 0) && (path[last] == '/')) {
		--last;
	}
	if (last < (int)path.length() - 1) {
		path = path.substr(0, last + 1);
	}

	int index;
	while ((index = path.find("/../")) != -1) {
		int prevIndex = std::max((int)path.rfind('/', index - 1), 0);
		path.erase(prevIndex, index + 3 - prevIndex);
	}
	const int len = path.length();
	if ((len >= 3) && (path.substr(len - 3) == "/..")) {
		int prevIndex = std::max((int)path.rfind('/', len - 4), 0);
		path.erase(prevIndex, len - prevIndex);
	}
	while ((index = path.find("/./")) != -1) {
		path.erase(index, 2);
	}
	while ((index = path.find("//")) != -1) {
		path.erase(index, 1);
	}
}

ZLFSDir *ZLUnixFSManager::createNewDirectory(const std::string &path) const {
	return (mkdir(path.c_str(), 0x1FF) == 0) ? createPlainDirectory(path) : 0;
}

int ZLUnixFSManager::findArchiveFileNameDelimiter(const std::string &path) const {
	return path.rfind(':');
}

static const std::string RootPath = "/";

shared_ptr<ZLDir> ZLUnixFSManager::rootDirectory() const {
	return createPlainDirectory(RootPath);
}

const std::string &ZLUnixFSManager::rootDirectoryPath() const {
	return RootPath;
}

std::string ZLUnixFSManager::parentPath(const std::string &path) const {
	if (path == RootPath) {
		return path;
	}
	int index = findLastFileNameDelimiter(path);
	return (index <= 0) ? RootPath : path.substr(0, index);
}

void ZLUnixFSManager::moveFile(const std::string &oldName, const std::string &newName) {
	rename(oldName.c_str(), newName.c_str());
}

void ZLUnixFSManager::getStat(const std::string &path, bool includeSymlinks, struct stat &fileInfo) const {
	if (includeSymlinks) {
		stat(path.c_str(), &fileInfo);
	} else {
		lstat(path.c_str(), &fileInfo);
	}
}
