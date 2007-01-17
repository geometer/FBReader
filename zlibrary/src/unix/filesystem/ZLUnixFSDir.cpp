/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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
#include <dirent.h>
#include <stdio.h>

#include "ZLUnixFSDir.h"

void ZLUnixFSDir::collectSubDirs(std::vector<std::string> &names, bool includeSymlinks) {
	DIR *dir = opendir(name().c_str());
	if (dir != 0) {
		const std::string namePrefix = name() + "/";
		const dirent *file;
		struct stat fileInfo;
		std::string shortName;
		std::string fullName;
		while ((file = readdir(dir)) != 0) {
			shortName = file->d_name;
			if ((shortName == ".") || (shortName == "..")) {
				continue;
			}
			fullName = namePrefix + shortName;
			if (includeSymlinks) {
				stat(fullName.c_str(), &fileInfo);
			} else {
				lstat(fullName.c_str(), &fileInfo);
			}
			if (S_ISDIR(fileInfo.st_mode)) {
				names.push_back(shortName);
			}
		}
		closedir(dir);
	}
}

void ZLUnixFSDir::collectFiles(std::vector<std::string> &names, bool includeSymlinks) {
	DIR *dir = opendir(name().c_str());
	if (dir != 0) {
		const std::string namePrefix = name() + "/";
		const dirent *file;
		struct stat fileInfo;
		std::string shortName;
		std::string fullName;
		while ((file = readdir(dir)) != 0) {
			shortName = file->d_name;
			if ((shortName == ".") || (shortName == "..")) {
				continue;
			}
			fullName = namePrefix + shortName;
			if (includeSymlinks) {
				stat(fullName.c_str(), &fileInfo);
			} else {
				lstat(fullName.c_str(), &fileInfo);
			}
			if (S_ISREG(fileInfo.st_mode)) {
				names.push_back(shortName);
			}
		}
		closedir(dir);
	}
}
