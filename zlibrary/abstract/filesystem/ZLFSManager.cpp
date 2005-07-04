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

//#include <iostream>
#include <algorithm>

#include <abstract/ZLStringUtil.h>

#include "ZLFSManager.h"
#include "ZLGzipInputStream.h"

ZLFSManager *ZLFSManager::ourInstance = 0;

ZLFile::ZLFile(const std::string &path) : myPath(path), myInfoIsFilled(false) {
	ZLFSManager::instance().normalize(myPath);
	int index = std::max((int)myPath.rfind('/'), (int)myPath.rfind(':'));
	myFullName = myPath.substr(index + 1);
	if (ZLStringUtil::stringEndsWith(myFullName, ".gz")) {
		myName = myFullName.substr(0, myFullName.length() - 3);
		myIsCompressed = true;
	} else {
		myName = myFullName;
		myIsCompressed = false;
	}
	index = myName.rfind('.');
	if (index > 0) {
		myExtension = myName.substr(index + 1);
		myName = myName.substr(0, index);
	}
}

ZLInputStream *ZLFile::createInputStream() const {
	//std::cerr << "path = " << myPath << "\n";
	ZLInputStream *stream = ZLFSManager::instance().createPlainInputStream(myPath);
	if (myIsCompressed && (stream != 0)) {
		stream = new ZLGzipInputStream(stream, size());
	}
	return stream;
}

ZLOutputStream *ZLFile::createOutputStream() const {
	if (myIsCompressed) {
		return 0;
	}
	if (myPath.find(":") != (size_t)-1) {
		return 0;
	}
	return ZLFSManager::instance().createOutputStream(myPath);
}
