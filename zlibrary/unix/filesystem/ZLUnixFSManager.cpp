/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLStringUtil.h>

#include "ZLUnixFSManager.h"
#include "ZLUnixFSDir.h"
#include "ZLUnixFileInputStream.h"
#include "ZLUnixFileOutputStream.h"

void ZLUnixFSManager::normalize(std::string &path) const {
	static std::string HomeDir = getenv("HOME");
	static std::string PwdDir = getenv("PWD");

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
}

ZLFSDir *ZLUnixFSManager::createPlainDirectory(const std::string &path) const {
	return new ZLUnixFSDir(path);
}

ZLFSDir *ZLUnixFSManager::createNewDirectory(const std::string &path) const {
	return (mkdir(path.c_str(), 0x1FF) == 0) ? new ZLUnixFSDir(path) : 0;
}

bool ZLUnixFSManager::isZipSupported() const {
	return true;
}

ZLInputStream *ZLUnixFSManager::createPlainInputStream(const std::string &path) const {
	return new ZLUnixFileInputStream(path);
}

ZLOutputStream *ZLUnixFSManager::createOutputStream(const std::string &path) const {
	return new ZLUnixFileOutputStream(path);
}

ZLFileInfo ZLUnixFSManager::fileInfo(const std::string &path) const {
	ZLFileInfo info;
	struct stat fileStat;
	info.Exists = stat(path.c_str(), &fileStat) == 0;
	if (info.Exists) {
		info.Size = fileStat.st_size;
		info.MTime = fileStat.st_mtime;
		info.IsDirectory = S_ISDIR(fileStat.st_mode);
	}
	return info;
}
