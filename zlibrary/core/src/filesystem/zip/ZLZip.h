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

#ifndef __ZLZIP_H__
#define __ZLZIP_H__

#include <map>

#include <shared_ptr.h>

#include "../ZLInputStream.h"
#include "../ZLDir.h"

class ZLZDecompressor;
class ZLFile;

class ZLZipCache {

public:
	static ZLZipCache Instance;

	struct Info {
		Info();

		int Offset;
		int CompressionMethod;
		int CompressedSize;
		int UncompressedSize;
	};

public:
	void addToCache(const ZLFile &file);
	bool contains(const std::string &fileName) const;
	Info info(const std::string &fileName, const std::string &entryName) const;

private:
	std::map<std::string,std::map<std::string,Info> > myMap;
};

class ZLZipInputStream : public ZLInputStream {

private:
	ZLZipInputStream(shared_ptr<ZLInputStream> &base, const std::string &fileName, const std::string &entryName);

public:
	~ZLZipInputStream();
	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(int offset, bool absoluteOffset);
	size_t offset() const;
	size_t sizeOfOpened();

private:
	shared_ptr<ZLInputStream> myBaseStream;
	std::string myFileName;
	std::string myEntryName;
	bool myIsDeflated;

	size_t myUncompressedSize;
	size_t myAvailableSize;
	size_t myOffset;

	shared_ptr<ZLZDecompressor> myDecompressor;

friend class ZLFile;
};

class ZLGzipInputStream : public ZLInputStream {

private:
	ZLGzipInputStream(shared_ptr<ZLInputStream> stream);

public:
	~ZLGzipInputStream();
	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(int offset, bool absoluteOffset);
	size_t offset() const;
	size_t sizeOfOpened();

private:
	shared_ptr<ZLInputStream> myFileStream;
	size_t myFileSize;

	size_t myOffset;

	shared_ptr<ZLZDecompressor> myDecompressor;

friend class ZLFile;
};

class ZLZipDir : public ZLDir {

private:
	ZLZipDir(const std::string &name);

public:
	~ZLZipDir();
	void collectSubDirs(std::vector<std::string>&, bool);
	void collectFiles(std::vector<std::string> &names, bool includeSymlinks);

protected:
	std::string delimiter() const;

friend class ZLFile;
};

inline ZLZipDir::ZLZipDir(const std::string &name) : ZLDir(name) {}
inline ZLZipDir::~ZLZipDir() {}
inline void ZLZipDir::collectSubDirs(std::vector<std::string>&, bool) {}

#endif /* __ZLZIP_H__ */
