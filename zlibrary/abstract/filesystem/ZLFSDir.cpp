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

#include "ZLFSDir.h"

ZLFSDir::ZLFSDir(const std::string &name) : ZLDir(name) {
	myDIR = 0;
}

ZLFSDir::~ZLFSDir() {
	close();
}

void ZLFSDir::create() {
	mkdir(name().c_str(), 0x1FF);
}

bool ZLFSDir::open() {
	if (myDIR == 0) {
		myDIR = opendir(name().c_str());
	}
	return myDIR != 0;
}

void ZLFSDir::close() {
	if (myDIR != 0) {
		closedir(myDIR);
		myDIR = 0;
	}
}

void ZLFSDir::collectSubDirs(std::vector<std::string> &names) {
	if (myDIR == 0) {
		return;
	}
	const dirent *file;
	while ((file = readdir(myDIR)) != 0) {
		if (file->d_type == DT_DIR) {
			std::string fname = file->d_name;
			if ((fname != ".") && (fname != "..")) {
				names.push_back(file->d_name);
			}
		}
  }
}

void ZLFSDir::collectRegularFiles(std::vector<std::string> &names) {
	if (myDIR == 0) {
		return;
	}
	const dirent *file;
	while ((file = readdir(myDIR)) != 0) {
		if (file->d_type == DT_REG) {
			names.push_back(file->d_name);
		}
  }
}
