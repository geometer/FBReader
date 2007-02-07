/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLWin32FSManager.h"

static std::string getPwdDir() {
	char pwd[2048];
	return (getcwd(pwd, 2047) != 0) ? pwd : "";
}

static std::string getHomeDir() {
	char *home = getenv("USERPROFILE");
	return (home != 0) ? home : "";
}

void ZLWin32FSManager::normalize(std::string &path) const {
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

	if (path.empty()) {
		path = PwdDir;
	} else if (path[0] == '~') {
		path = HomeDir + path.substr(1);
	} else if (path.substr(0, APPattern.length()) == APPattern) {
		path = AP + path.substr(APPattern.length());
	} else if ((path.length() > 1) && (path[1] != ':')) {
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
	while ((index = path.find("\\\\")) != -1) {
		path.erase(index, 1);
	}
}

ZLFSDir *ZLWin32FSManager::createNewDirectory(const std::string &path) const {
	return (mkdir(path.c_str()) == 0) ? createPlainDirectory(path) : 0;
}

std::string ZLWin32FSManager::convertFilenameToUtf8(const std::string &name) const {
	/*
	char *gtkString = g_locale_to_utf8(name.data(), name.length(), 0, 0, 0);
	std::string convertedName = gtkString;
	g_free(gtkString);
	return convertedName;
	*/
	return name;
}

int ZLWin32FSManager::findArchivePathDelimiter(const std::string &path) const {
	int index = path.rfind(':');
	return (index == 1) ? -1 : index;
}

void ZLWin32FSManager::moveFile(const std::string &oldName, const std::string &newName) {
	remove(newName.c_str());
	rename(oldName.c_str(), newName.c_str());
}

void ZLWin32FSManager::getStat(const std::string fullName, bool /*includeSymlinks*/, struct stat &fileInfo) const {
	stat(fullName.c_str(), &fileInfo);
}
