/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>

#include "../../filesystem/ZLFSManager.h"

class ZLWin32FSManager : public ZLFSManager {

public:
	static void createInstance() { ourInstance = new ZLWin32FSManager(); }

	static ZLUnicodeUtil::Ucs2String longFilePath(const std::string &path);
	
private:
	ZLWin32FSManager() {}
	
private:
	void normalizeRealPath(std::string &path) const;
	std::string resolveSymlink(const std::string &path) const;
	ZLFSDir *createPlainDirectory(const std::string &path) const;
	ZLInputStream *createPlainInputStream(const std::string &path) const;
	ZLOutputStream *createOutputStream(const std::string &path) const;
	ZLFSDir *createNewDirectory(const std::string &path) const;
	bool removeFile(const std::string &path) const;

	ZLFileInfo fileInfo(const std::string &path) const;

	std::string convertFilenameToUtf8(const std::string &name) const;
	int findArchiveFileNameDelimiter(const std::string &path) const;
	shared_ptr<ZLDir> rootDirectory() const;
	const std::string &rootDirectoryPath() const;
	std::string parentPath(const std::string &path) const;

	bool canRemoveFile(const std::string &path) const;
};

#endif /* __ZLWIN32FSMANAGER_H__ */
