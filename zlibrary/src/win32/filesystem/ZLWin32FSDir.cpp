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

#include <dirent.h>
#include <stdio.h>

#include "ZLWin32FSDir.h"
#include "ZLWin32FSManager.h"

void ZLWin32RootDir::collectSubDirs(std::vector<std::string> &names, bool) {
	names.push_back("C:\\");
	names.push_back("Z:\\");
}

void ZLWin32RootDir::collectFiles(std::vector<std::string>&, bool) {
}

void ZLWin32FSDir::collectSubDirs(std::vector<std::string> &names, bool includeSymlinks) {
	ZLPosixFSDir::collectSubDirs(names, includeSymlinks);
}

void ZLWin32FSDir::collectFiles(std::vector<std::string> &names, bool includeSymlinks) {
	ZLPosixFSDir::collectFiles(names, includeSymlinks);
}
