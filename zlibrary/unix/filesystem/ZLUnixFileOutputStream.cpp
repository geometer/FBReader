/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

ZLUnixFileOutputStream::ZLUnixFileOutputStream(const std::string &name) : myName(name) {
	myFile = 0;
}

ZLUnixFileOutputStream::~ZLUnixFileOutputStream() {
	close();
}

bool ZLUnixFileOutputStream::open() {
	close();

	char *tmpl = new char[myName.length() + 8];
	strcpy(tmpl, myName.c_str());
	strcpy(tmpl + myName.length(), ".XXXXXX");
	if (mkstemp(tmpl) != -1) {
		myTemporaryName = tmpl;
		myFile = fopen(myTemporaryName.c_str(), "wb");
		myHasErrors = false;
	}

	delete[] tmpl;
	return myFile != 0;
}

void ZLUnixFileOutputStream::write(const std::string &str) {
	if (fwrite(str.data(), 1, str.length(), myFile) != str.length()) {
		myHasErrors = true;
	}
}

void ZLUnixFileOutputStream::close() {
	if (myFile != 0) {
		fclose(myFile);
		myFile = 0;
		if (!myHasErrors) {
			rename(myTemporaryName.c_str(), myName.c_str());
		}
	}
}
