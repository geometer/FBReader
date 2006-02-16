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

#define NOZLIBDEFS
#include <zlib.h>

#include <Extensions/ExpansionMgr/VFSMgr.h>

#include <abstract/ZLStringUtil.h>

#include "ZLPalmFSManager.h"
#include "ZLPalmFSDir.h"
#include "ZLPalmFileInputStream.h"
#include "ZLPalmFileOutputStream.h"

void ZLPalmFSManager::createInstance() {
	ourInstance = new ZLPalmFSManager();
}

ZLPalmFSManager::ZLPalmFSManager() : ZLFSManager() {
}

ZLPalmFSManager::~ZLPalmFSManager() {
}

ZLFSDir *ZLPalmFSManager::createPlainDirectory(const std::string &path) const {
	return new ZLPalmFSDir(path);
}

ZLFSDir *ZLPalmFSManager::createNewDirectory(const std::string &path) const {
	// TODO: implement
	return 0;
}

bool ZLPalmFSManager::isZipSupported() const {
	return ZLibRef != sysInvalidRefNum;
}

ZLInputStream *ZLPalmFSManager::createPlainInputStream(const std::string &path) const {
	return new ZLPalmFileInputStream(path);
}

ZLOutputStream *ZLPalmFSManager::createOutputStream(const std::string &path) const {
	return new ZLPalmFileOutputStream(path);
}

ZLFSManager::FileInfo ZLPalmFSManager::fileInfo(const std::string &path) const {
	FileInfo info;

	UInt16  unVol;
	UInt32  ulIter;
	UInt32  ulVFSMgrVer;

	FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &ulVFSMgrVer);
	ulIter = vfsIteratorStart;
	VFSVolumeEnumerate(&unVol, &ulIter);

	UInt32 fileRef;
	VFSFileOpen(unVol, path.c_str(), vfsModeRead, &fileRef);

	if (fileRef != 0) {
		info.Exists = true;
		VFSFileSize(fileRef, &info.Size);
		UInt32 attributes;
		VFSFileGetAttributes(fileRef, &attributes);
		info.IsDirectory = attributes & vfsFileAttrDirectory;
		VFSFileGetDate(fileRef, vfsFileDateModified, &info.MTime);
		VFSFileClose(fileRef);
	} else {
		info.Exists = false;
		info.IsDirectory = false;
		info.Size = 0;
		info.MTime = 0;
	}
	return info;
}
