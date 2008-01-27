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

#ifndef __ZLWIN32ROOTDIR_H__
#define __ZLWIN32ROOTDIR_H__

#include "../../../../core/src/filesystem/ZLFSDir.h"

class ZLWin32RootDir : public ZLFSDir {

public:
	ZLWin32RootDir() : ZLFSDir(std::string()) {}

	void collectSubDirs(std::vector<std::string> &names, bool includeSymlinks);
	void collectFiles(std::vector<std::string> &names, bool includeSymlinks);
};

#endif /* __ZLWIN32ROOTDIR_H__ */
