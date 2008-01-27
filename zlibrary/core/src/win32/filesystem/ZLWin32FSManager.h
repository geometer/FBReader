/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32FSMANAGER_H__
#define __ZLWIN32FSMANAGER_H__

#include "../../../../core/src/posix/filesystem/ZLPosixFSManager.h"

class ZLWin32FSManager : public ZLPosixFSManager {

public:
	static void createInstance() { ourInstance = new ZLWin32FSManager(); }
	
private:
	ZLWin32FSManager() {}
	
protected:
	void normalize(std::string &path) const;
	ZLFSDir *createPlainDirectory(const std::string &path) const;
	ZLFSDir *createNewDirectory(const std::string &path) const;

	ZLFileInfo fileInfo(const std::string &path) const;

	std::string convertFilenameToUtf8(const std::string &name) const;
	int findArchiveFileNameDelimiter(const std::string &path) const;
	shared_ptr<ZLDir> rootDirectory() const;
	const std::string &rootDirectoryPath() const;
	std::string parentPath(const std::string &path) const;

	void moveFile(const std::string &oldName, const std::string &newName);
	void getStat(const std::string &path, bool includeSymlinks, struct stat &fileInfo) const;
};

#endif /* __ZLWIN32FSMANAGER_H__ */
