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

#include <unistd.h>

#include "ZLUnixFileOutputStream.h"

ZLUnixFileOutputStream::ZLUnixFileOutputStream(const std::string &name) : myName(name), myHasErrors(false), myFileDescriptor(-1) {
}

ZLUnixFileOutputStream::~ZLUnixFileOutputStream() {
	close();
}

bool ZLUnixFileOutputStream::open() {
	close();

	myTemporaryName = myName + ".XXXXXX" + '\0';
	myFileDescriptor = ::mkstemp((char*)myTemporaryName.data());
	myHasErrors = myFileDescriptor == -1;

	return !myHasErrors;
}

void ZLUnixFileOutputStream::write(const std::string &str) {
	if (::write(myFileDescriptor, str.data(), str.length()) != (ssize_t)str.length()) {
		myHasErrors = true;
	}
}

void ZLUnixFileOutputStream::close() {
	if (myFileDescriptor != -1) {
		::close(myFileDescriptor);
		myFileDescriptor = -1;
		if (!myHasErrors) {
			rename(myTemporaryName.c_str(), myName.c_str());
		}
	}
}
