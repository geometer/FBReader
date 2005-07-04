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

#include <sys/stat.h>

#include <abstract/ZLStringUtil.h>

#include "ZLUnixFSManager.h"
#include "ZLUnixFSDir.h"
#include "ZLUnixFileInputStream.h"
#include "../../abstract/filesystem/ZLZipInputStream.h"
#include "ZLUnixFileOutputStream.h"

void ZLUnixFSManager::normalize(std::string &fileName) {
	static std::string HomeDir = getenv("HOME");
	static std::string PwdDir = getenv("PWD");

	if (fileName.empty()) {
		fileName = PwdDir;
	} else if (fileName[0] == '~') {
		if ((fileName.length() == 1) || (fileName[1] == '/')) {
			fileName = HomeDir + fileName.substr(1);
		}
	} else if (fileName[0] != '/') {
		fileName = PwdDir + '/' + fileName;
	}
	int last = fileName.length() - 1;
	while ((last > 0) && (fileName[last] == '/')) {
		last--;
	}
	if (last < (int)fileName.length() - 1) {
		fileName = fileName.substr(0, last + 1);
	}
}

ZLFSDir *ZLUnixFSManager::createDirectory(const std::string &name) {
	return new ZLUnixFSDir(name);
}

ZLInputStream *ZLUnixFSManager::createPlainInputStream(const std::string &path) {
	if (path.find(':') != (size_t)-1) {
		return new ZLZipInputStream(path);
	}

	return new ZLUnixFileInputStream(path);
}

ZLOutputStream *ZLUnixFSManager::createOutputStream(const std::string &path) {
	return new ZLUnixFileOutputStream(path);
}

ZLFSManager::FileInfo ZLUnixFSManager::fileInfo(const std::string &path) {
	FileInfo info;
	struct stat fileStat;
	info.Exists = stat(path.c_str(), &fileStat) == 0;
	if (info.Exists) {
		info.Size = fileStat.st_size;
		info.MTime = fileStat.st_mtime;
	}
	return info;
}
