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

#ifndef __ZLZIPDIR_H__
#define __ZLZIPDIR_H__

#include "ZLDir.h"

class ZLZipDir : public ZLDir {

private:
	ZLZipDir(const std::string &name) FS_SECTION;
	~ZLZipDir() FS_SECTION;

public:
	void collectSubDirs(std::vector<std::string>&, bool) FS_SECTION;
	void collectFiles(std::vector<std::string> &names, bool includeSymlinks) FS_SECTION;

protected:
	std::string delimiter() const FS_SECTION;

friend class ZLFile;
};

inline ZLZipDir::ZLZipDir(const std::string &name) : ZLDir(name) {}
inline ZLZipDir::~ZLZipDir() {}
inline void ZLZipDir::collectSubDirs(std::vector<std::string>&, bool) {}

#endif /* __ZLZIPDIR_H__ */
