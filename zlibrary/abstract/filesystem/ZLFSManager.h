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

public:
	ZLFile(const std::string &path) FS_SECTION;
	~ZLFile() FS_SECTION;

	bool exists() const FS_SECTION;
	unsigned long mTime() const FS_SECTION;
	size_t size() const FS_SECTION;	

	const std::string &path() const FS_SECTION;
	const std::string &fullName() const FS_SECTION;
	const std::string &name() const FS_SECTION;
	const std::string &extension() const FS_SECTION;
	bool isCompressed() const FS_SECTION;

	ZLInputStream *createInputStream() const FS_SECTION;
	ZLOutputStream *createOutputStream() const FS_SECTION;

private:
	void fillInfo() const FS_SECTION;

private:
	std::string myPath;
	std::string myFullName;
	std::string myName;
	std::string myExtension;
	bool myIsCompressed;
	mutable ZLFSManager::FileInfo myInfo;
	mutable bool myInfoIsFilled;
};

inline void ZLFSManager::deleteInstance() { delete ourInstance; }
inline ZLFSManager &ZLFSManager::instance() { return *ourInstance; }
inline ZLFSManager::ZLFSManager() {}
inline ZLFSManager::~ZLFSManager() {}

inline ZLFile::~ZLFile() {}

inline bool ZLFile::exists() const { fillInfo(); return myInfo.Exists; }
inline unsigned long ZLFile::mTime() const { fillInfo(); return myInfo.MTime; }
inline size_t ZLFile::size() const { fillInfo(); return myInfo.Size; }
	
inline const std::string &ZLFile::path() const { return myPath; }
inline const std::string &ZLFile::fullName() const { return myFullName; }
inline const std::string &ZLFile::name() const { return myName; }
inline const std::string &ZLFile::extension() const { return myExtension; }
inline bool ZLFile::isCompressed() const { return myIsCompressed; }

inline void ZLFile::fillInfo() const {
	if (!myInfoIsFilled) {
		myInfo = ZLFSManager::instance().fileInfo(myPath);
		myInfoIsFilled = true;
	}
}

#endif /* __ZLFSMANAGER_H__ */
