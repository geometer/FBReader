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

#include <dirent.h>
#include <stdio.h>

#include "ZLPosixFSDir.h"
#include "ZLPosixFSManager.h"

void ZLPosixFSDir::collectSubDirs(std::vector<std::string> &names, bool includeSymlinks) {
	DIR *dir = opendir(path().c_str());
	if (dir != 0) {
		const std::string namePrefix = path() + delimiter();
		const dirent *file;
		struct stat fileInfo;
		std::string shortName;
		while ((file = readdir(dir)) != 0) {
			shortName = file->d_name;
			if ((shortName == ".") || (shortName == "..")) {
				continue;
			}
			((ZLPosixFSManager&)ZLFSManager::instance()).getStat(namePrefix + shortName, includeSymlinks, fileInfo);
			if (S_ISDIR(fileInfo.st_mode)) {
				names.push_back(shortName);
			}
		}
		closedir(dir);
	}
}

void ZLPosixFSDir::collectFiles(std::vector<std::string> &names, bool includeSymlinks) {
	DIR *dir = opendir(path().c_str());
	if (dir != 0) {
		const std::string namePrefix = path() + delimiter();
		const dirent *file;
		struct stat fileInfo;
		std::string shortName;
		while ((file = readdir(dir)) != 0) {
			shortName = file->d_name;
			if ((shortName == ".") || (shortName == "..")) {
				continue;
			}
			((ZLPosixFSManager&)ZLFSManager::instance()).getStat(namePrefix + shortName, includeSymlinks, fileInfo);
			if (S_ISREG(fileInfo.st_mode)) {
				names.push_back(shortName);
			}
		}
		closedir(dir);
	}
}
