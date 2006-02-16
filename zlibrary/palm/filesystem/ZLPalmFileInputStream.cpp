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

#include <Extensions/ExpansionMgr/VFSMgr.h>

#include "ZLPalmFileInputStream.h"

ZLPalmFileInputStream::ZLPalmFileInputStream(const std::string &name) : myName(name) {
	myFileRef = 0;
}

ZLPalmFileInputStream::~ZLPalmFileInputStream() {
	close();
}

bool ZLPalmFileInputStream::open() {
	close();

	UInt16  unVol;
	UInt32  ulIter;
	UInt32  ulVFSMgrVer;

	FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &ulVFSMgrVer);
	ulIter = vfsIteratorStart;
	VFSVolumeEnumerate(&unVol, &ulIter);

	VFSFileOpen(unVol, myName.c_str(), vfsModeRead, &myFileRef);

	return myFileRef != 0;
}

size_t ZLPalmFileInputStream::read(char *buffer, size_t maxSize) {
	if (buffer != 0) {
		UInt32 size;
		VFSFileRead(myFileRef, maxSize, buffer, &size);
		return size;
	} else {
		VFSFileSeek(myFileRef, vfsOriginCurrent, maxSize);
		// TODO: return correct value
		return maxSize;
	}
}

void ZLPalmFileInputStream::close() {
	if (myFileRef != 0) {
		VFSFileClose(myFileRef);
		myFileRef = 0;
	}
}

void ZLPalmFileInputStream::seek(size_t offset) {
	VFSFileSeek(myFileRef, vfsOriginCurrent, offset);
}

size_t ZLPalmFileInputStream::offset() const {
	UInt32 pos;
	VFSFileTell(myFileRef, &pos);
	return pos;
}

size_t ZLPalmFileInputStream::sizeOfOpened() {
	UInt32 size;
	VFSFileSize(myFileRef, &size);
	return size;
}
