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

#include <abstract/ZLStringUtil.h>

#include "ZLFSDir.h"
#include "ZLFSManager.h"
#include "ZLOutputStream.h"
#include "zip/ZLZip.h"
#include "tar/ZLTar.h"

ZLFSManager *ZLFSManager::ourInstance = 0;

ZLFile::ZLFile(const std::string &path) : myPath(path), myInfoIsFilled(false) {
	ZLFSManager::instance().normalize(myPath);
	int index = myPath.rfind(':');
	if (index == -1) {
		index = myPath.rfind('/');
	}
	myFullName = myPath.substr(index + 1);
	myName = myFullName;

	myArchiveType = NONE;

	if (ZLStringUtil::stringEndsWith(myName, ".gz")) {
		myName = myName.substr(0, myName.length() - 3);
		myArchiveType |= GZIP;
	}
	if (ZLStringUtil::stringEndsWith(myName, ".zip")) {
		myArchiveType |= ZIP;
	} else if (ZLStringUtil::stringEndsWith(myName, ".tar")) {
		myArchiveType |= TAR;
	} else if (ZLStringUtil::stringEndsWith(myName, ".tgz")) {
		myArchiveType |= TAR | GZIP;
		myName = myName.substr(0, myName.length() - 2) + "ar";
	}

	index = myName.rfind('.');
	if (index > 0) {
		myExtension = myName.substr(index + 1);
		myName = myName.substr(0, index);
	}
}

shared_ptr<ZLInputStream> ZLFile::inputStream() const {
	if (isDirectory()) {
		return 0;
	}

	ZLInputStream *stream = 0;
	
	size_t index = myPath.rfind(':');
	if (index == (size_t)-1) {
		stream = ZLFSManager::instance().createPlainInputStream(myPath);
	} else {
		ZLFile baseFile(myPath.substr(0, index));
		if (baseFile.myArchiveType & ZIP) {
			if (ZLFSManager::instance().isZipSupported()) {
				shared_ptr<ZLInputStream> base = baseFile.inputStream();
				stream = base.isNull() ? 0 : new ZLZipInputStream(base, myPath.substr(index + 1));
			}
		} else if (baseFile.myArchiveType & TAR) {
			shared_ptr<ZLInputStream> base = baseFile.inputStream();
			stream = base.isNull() ? 0 : new ZLTarInputStream(base, myPath.substr(index + 1));
		}
	}

	if ((myArchiveType & GZIP) && (stream != 0)) {
		return ZLFSManager::instance().isZipSupported() ? new ZLGzipInputStream(stream) : 0;
	}
	return stream;
}

shared_ptr<ZLOutputStream> ZLFile::outputStream() const {
	if (isCompressed()) {
		return 0;
	}
	if (myPath.find(':') != (size_t)-1) {
		return 0;
	}
	return ZLFSManager::instance().createOutputStream(myPath);
}

shared_ptr<ZLDir> ZLFile::directory(bool createUnexisting) const {
	if (exists()) {
		if (isDirectory()) {
			return ZLFSManager::instance().createPlainDirectory(myPath);
		} else if ((myArchiveType & ZIP) && ZLFSManager::instance().isZipSupported()) {
			return new ZLZipDir(myPath);
		} else if (myArchiveType & TAR) {
			return new ZLTarDir(myPath);
		}
	} else if (createUnexisting) {
		myInfoIsFilled = false;
		return ZLFSManager::instance().createNewDirectory(myPath);
	}
	return 0;
}

void ZLFile::fillInfo() const {
	size_t index = myPath.find(':');
	if (index == (size_t)-1) {
		myInfo = ZLFSManager::instance().fileInfo(myPath);
	} else {
		myInfo = ZLFSManager::instance().fileInfo(myPath.substr(index));
		myInfo.IsDirectory = false;
	}
	myInfoIsFilled = true;
}
