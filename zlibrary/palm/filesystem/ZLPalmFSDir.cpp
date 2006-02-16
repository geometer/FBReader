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

#include "ZLPalmFSDir.h"

void ZLPalmFSDir::collectSubDirs(std::vector<std::string> &names, bool includeSymlinks) {
	/*
	DIR *dir = opendir(name().c_str());
	if (dir != 0) {
		const dirent *file;
		while ((file = readdir(dir)) != 0) {
			if (file->d_type == DT_DIR) {
				std::string fname = file->d_name;
				if ((fname != ".") && (fname != "..")) {
					names.push_back(file->d_name);
				}
			} else if (includeSymlinks && (file->d_type == DT_LNK)) {
				DIR *ldir = opendir(itemName(file->d_name).c_str());
				if (ldir != 0) {
					closedir(ldir);
					names.push_back(file->d_name);
				}
			}
		}
		closedir(dir);
	}
	*/
}

void ZLPalmFSDir::collectFiles(std::vector<std::string> &names, bool includeSymlinks) {
	/*
	DIR *dir = opendir(name().c_str());
	if (dir != 0) {
		const dirent *file;
		while ((file = readdir(dir)) != 0) {
			if (file->d_type == DT_REG) {
				names.push_back(file->d_name);
			} else if (includeSymlinks && (file->d_type == DT_LNK)) {
				FILE *lfile = fopen(itemName(file->d_name).c_str(), "r");
				if (lfile != 0) {
					fclose(lfile);
					names.push_back(file->d_name);
				}
			}
  	}
		closedir(dir);
	}
	*/
}

std::string ZLPalmFSDir::delimiter() const {
	return "/";
}
