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

#ifndef __ZLPOSIXFSMANAGER_H__
#define __ZLPOSIXFSMANAGER_H__

#include "../../filesystem/ZLFSManager.h"

class ZLPosixFSManager : public ZLFSManager {

protected:
	ZLFSDir *createPlainDirectory(const std::string &path) const;
	ZLInputStream *createPlainInputStream(const std::string &path) const;
	ZLOutputStream *createOutputStream(const std::string &path) const;
	ZLFileInfo fileInfo(const std::string &path) const;
	bool removeFile(const std::string &path) const;

	int findLastFileNameDelimiter(const std::string &path) const;

public:
	virtual void moveFile(const std::string &oldName, const std::string &newName) = 0;
	virtual void getStat(const std::string &path, bool includeSymlinks, struct stat &fileInfo) const = 0;
};

#endif /* __ZLPOSIXFSMANAGER_H__ */
