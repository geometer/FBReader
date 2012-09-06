/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

class ZLZipEntryCache : public ZLUserData {

public:
	static const ZLZipEntryCache &cache(ZLInputStream &baseStream);

public:
	struct Info {
		Info();

		int Offset;
		int CompressionMethod;
		int CompressedSize;
		int UncompressedSize;
	};

public:
	ZLZipEntryCache(ZLInputStream &baseStream);
	Info info(const std::string &entryName) const;
	void collectFileNames(std::vector<std::string> &names) const;

private:
	std::map<std::string,Info> myInfoMap;
};

class ZLZipInputStream : public ZLInputStream {

private:
	ZLZipInputStream(shared_ptr<ZLInputStream> &base, const std::string &entryName);

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
	std::string myEntryName;
	bool myIsDeflated;

	size_t myUncompressedSize;
	size_t myAvailableSize;
	size_t myOffset;

	shared_ptr<ZLZDecompressor> myDecompressor;

friend class ZLFSArchiverZip;
};

class ZLFSCompresorGz;
class ZLFSArchiverZip;
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
	shared_ptr<ZLInputStream> myBaseStream;
	size_t myFileSize;

	size_t myOffset;

	shared_ptr<ZLZDecompressor> myDecompressor;

friend class ZLFSCompressorGZ;
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

friend class ZLFSArchiverZip;
};

inline ZLZipDir::ZLZipDir(const std::string &name) : ZLDir(name) {}
inline ZLZipDir::~ZLZipDir() {}
inline void ZLZipDir::collectSubDirs(std::vector<std::string>&, bool) {}

#endif /* __ZLZIP_H__ */
