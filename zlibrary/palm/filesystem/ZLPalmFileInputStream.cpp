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

int ZLPalmFileInputStream::read(char *buffer, int maxSize) {
	UInt32 size;
	VFSFileRead(myFileRef, maxSize, buffer, &size);
	return size;
}

// TODO: optimize it
int ZLPalmFileInputStream::read(unsigned short *p) {
	unsigned short N = 0;
	int code = read((char*)&N, 2);
	unsigned char first = N;
	unsigned char second = N >> 8;
	*p = (((unsigned short)first) << 8) + second;
	return code;
}

// TODO: optimize it
int ZLPalmFileInputStream::read(unsigned long *p) {
	unsigned long N = 0;
	int code = read((char*)&N, 4);
	unsigned char first = N;
	unsigned char second = N >> 8;
	unsigned char third = N >> 16;
	unsigned char fourth = N >> 24;
	*p = (((unsigned long)first) << 24) + (((unsigned long)second) << 16) + (((unsigned long)third) << 8) + fourth;
	return code;
}

void ZLPalmFileInputStream::close() {
	if (myFileRef != 0) {
		VFSFileClose(myFileRef);
		myFileRef = 0;
	}
}

void ZLPalmFileInputStream::seek(int offset) {
	VFSFileSeek(myFileRef, vfsOriginCurrent, offset);
}

int ZLPalmFileInputStream::offset() const {
	UInt32 pos;
	VFSFileTell(myFileRef, &pos);
	return pos;
}
