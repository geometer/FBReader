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

#include <unistd.h>
#include <stdlib.h>

#include "ZLPosixFileOutputStream.h"
#include "ZLPosixFSManager.h"

ZLPosixFileOutputStream::ZLPosixFileOutputStream(const std::string &name) : myName(name), myHasErrors(false), myFile(0) {
}

ZLPosixFileOutputStream::~ZLPosixFileOutputStream() {
	close();
}

bool ZLPosixFileOutputStream::open() {
	close();

	myTemporaryName = myName + ".XXXXXX" + '\0';
	if (::mktemp((char*)myTemporaryName.data()) == 0) {
		return false;
	}

	myFile = fopen(myTemporaryName.c_str(), "wb");
	return myFile != 0;
}

void ZLPosixFileOutputStream::write(const std::string &str) {
	if (::fwrite(str.data(), 1, str.length(), myFile) != (size_t)str.length()) {
		myHasErrors = true;
	}
}

void ZLPosixFileOutputStream::close() {
	if (myFile != 0) {
		::fclose(myFile);
		myFile = 0;
		if (!myHasErrors) {
			((ZLPosixFSManager&)ZLFSManager::instance()).moveFile(myTemporaryName, myName);
		}
	}
}
