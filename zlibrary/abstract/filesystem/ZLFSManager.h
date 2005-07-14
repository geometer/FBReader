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

#ifndef __ZLFSMANAGER_H__
#define __ZLFSMANAGER_H__

#include <string>

class ZLFSDir;
class ZLInputStream;
class ZLOutputStream;

class ZLFSManager {

protected:
	struct FileInfo {
		bool Exists;
		bool IsDirectory;
		unsigned long MTime;
		unsigned long Size;
	};

public:
	static void deleteInstance() FS_SECTION;
	static ZLFSManager &instance() FS_SECTION;

protected:
	static ZLFSManager *ourInstance;
	
protected:
	ZLFSManager() FS_SECTION;
	virtual ~ZLFSManager() FS_SECTION;
	
public:
	virtual void normalize(std::string &fileName) FS_SECTION = 0;
	virtual ZLFSDir *createDirectory(const std::string &name) FS_SECTION = 0;

protected:
	virtual ZLInputStream *createPlainInputStream(const std::string &path) FS_SECTION = 0;
	virtual ZLOutputStream *createOutputStream(const std::string &path) FS_SECTION = 0;
	virtual FileInfo fileInfo(const std::string &name) FS_SECTION = 0;

friend class ZLFile;
};

class ZLFile {

private:
	enum ArchiveType {
		NONE = 0,
		GZIP = 0x0001,
		BZIP2 = 0x0002,
		COMPRESSED = 0x00ff,
		ZIP = 0x0100,
		TAR = 0x0200,
		ARCHIVE = 0xff00,
	};
	
public:
	ZLFile(const std::string &path) FS_SECTION;
	~ZLFile() FS_SECTION;

	bool exists() const FS_SECTION;
	unsigned long mTime() const FS_SECTION;
	size_t size() const FS_SECTION;	

	bool isCompressed() const FS_SECTION;
	bool isDirectory() const FS_SECTION;
	bool isArchive() const FS_SECTION;

	const std::string &path() const FS_SECTION;
	const std::string &fullName() const FS_SECTION;
	const std::string &name() const FS_SECTION;
	const std::string &extension() const FS_SECTION;

	ZLInputStream *createInputStream() const FS_SECTION;
	ZLOutputStream *createOutputStream() const FS_SECTION;

private:
	void fillInfo() const FS_SECTION;

private:
	std::string myPath;
	std::string myFullName;
	std::string myName;
	std::string myExtension;
	unsigned long myArchiveType;
	mutable ZLFSManager::FileInfo myInfo;
	mutable bool myInfoIsFilled;
};

inline void ZLFSManager::deleteInstance() { delete ourInstance; }
inline ZLFSManager &ZLFSManager::instance() { return *ourInstance; }
inline ZLFSManager::ZLFSManager() {}
inline ZLFSManager::~ZLFSManager() {}

inline ZLFile::~ZLFile() {}

inline bool ZLFile::exists() const { if (!myInfoIsFilled) fillInfo(); return myInfo.Exists; }
inline unsigned long ZLFile::mTime() const { if (!myInfoIsFilled) fillInfo(); return myInfo.MTime; }
inline size_t ZLFile::size() const { if (!myInfoIsFilled) fillInfo(); return myInfo.Size; }
	
inline bool ZLFile::isCompressed() const { return myArchiveType & COMPRESSED; }
inline bool ZLFile::isDirectory() const { if (!myInfoIsFilled) fillInfo(); return myInfo.IsDirectory; }
inline bool ZLFile::isArchive() const { return myArchiveType & ARCHIVE; }

inline const std::string &ZLFile::path() const { return myPath; }
inline const std::string &ZLFile::fullName() const { return myFullName; }
inline const std::string &ZLFile::name() const { return myName; }
inline const std::string &ZLFile::extension() const { return myExtension; }

#endif /* __ZLFSMANAGER_H__ */
