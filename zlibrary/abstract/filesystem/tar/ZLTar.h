/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLTAR_H__
#define __ZLTAR_H__

#include <abstract/shared_ptr.h>

#include "../ZLInputStream.h"
#include "../ZLDir.h"

class ZLTarInputStream : public ZLInputStream {

private:
	ZLTarInputStream(shared_ptr<ZLInputStream> &base, const std::string &name) FS_SECTION;

public:
	~ZLTarInputStream() FS_SECTION;
	bool open() FS_SECTION;
	size_t read(char *buffer, size_t maxSize) FS_SECTION;
	void close() FS_SECTION;

	void seek(size_t offset) FS_SECTION;
	size_t offset() const FS_SECTION;
	size_t sizeOfOpened() FS_SECTION;

private:
	shared_ptr<ZLInputStream> myBaseStream;
	std::string myCompressedFileName;
	size_t myCompressedFileSize;
	size_t myOffset;

friend class ZLFile;
};

class ZLTarDir : public ZLDir {

private:
	ZLTarDir(const std::string &path) FS_SECTION;

public:
	~ZLTarDir() FS_SECTION;
	void collectSubDirs(std::vector<std::string>&, bool includeSymlinks) FS_SECTION;
	void collectFiles(std::vector<std::string> &names, bool includeSymlinks) FS_SECTION;

protected:
	std::string delimiter() const FS_SECTION;

friend class ZLFile;
};

inline ZLTarDir::ZLTarDir(const std::string &path) : ZLDir(path) {}
inline ZLTarDir::~ZLTarDir() {}
inline void ZLTarDir::collectSubDirs(std::vector<std::string>&, bool) {}

#endif /* __ZLTAR_H__ */
