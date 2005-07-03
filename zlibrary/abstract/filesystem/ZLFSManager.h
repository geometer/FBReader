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

struct ZLFileInfo {
	bool Exists;
	unsigned long MTime;
	unsigned long Size;
};

class ZLFSManager {

public:
	static void deleteInstance() FS_SECTION;
	static ZLFSManager &instance() FS_SECTION;
	
protected:
	ZLFSManager() FS_SECTION;
	virtual ~ZLFSManager() FS_SECTION;
	
public:
	virtual void normalize(std::string &fileName) FS_SECTION = 0;
	virtual ZLFSDir *createDirectory(const std::string &name) FS_SECTION = 0;
	virtual ZLInputStream *createPlainInputStream(const std::string &name) FS_SECTION = 0;
	virtual ZLInputStream *createInputStream(const std::string &name) FS_SECTION = 0;
	virtual ZLOutputStream *createOutputStream(const std::string &name) FS_SECTION = 0;
	virtual ZLFileInfo fileInfo(const std::string &name) FS_SECTION = 0;

protected:
	static ZLFSManager *ourInstance;
};

#endif /* __ZLFSMANAGER_H__ */
