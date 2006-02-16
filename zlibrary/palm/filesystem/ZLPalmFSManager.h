/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLPALMFSMANAGER_H__
#define __ZLPALMFSMANAGER_H__

#include <abstract/ZLFSManager.h>

class ZLPalmFSManager : public ZLFSManager {

public:
	static void createInstance() FS_SECTION;
	~ZLPalmFSManager() FS_SECTION;
	
private:
	ZLPalmFSManager() FS_SECTION;

protected:
	ZLInputStream *createPlainInputStream(const std::string &path) const FS_SECTION;
	ZLOutputStream *createOutputStream(const std::string &path) const FS_SECTION;
	ZLFSDir *createPlainDirectory(const std::string &path) const FS_SECTION;
	ZLFSDir *createNewDirectory(const std::string &path) const FS_SECTION;
	FileInfo fileInfo(const std::string &path) const FS_SECTION;
	bool isZipSupported() const FS_SECTION;
};

#endif /* __ZLPALMFSMANAGER_H__ */
