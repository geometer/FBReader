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

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLFSDir.h"
#include "ZLFSManager.h"
#include "ZLFile.h"
#include "ZLOutputStream.h"
#include "zip/ZLZip.h"
#include "tar/ZLTar.h"
#include "bzip2/ZLBzip2InputStream.h"

ZLFSManager *ZLFSManager::ourInstance = 0;

void ZLFSManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
		ourInstance = 0;
	}
}

ZLFile::ZLFile(const std::string &path) : myPath(path), myInfoIsFilled(false) {
	ZLFSManager::instance().normalize(myPath);
	int index = myPath.rfind(':');
	if (index == -1) {
		index = myPath.rfind('/');
	}
	myFullName = myPath.substr(index + 1);
	myName = myFullName;

	std::map<std::string,ArchiveType> &forcedFiles = ZLFSManager::instance().myForcedFiles;
	std::map<std::string,ArchiveType>::iterator it = forcedFiles.find(myPath);
	if (it != forcedFiles.end()) {
		myArchiveType = it->second;
	} else {
		myArchiveType = NONE;
		std::string lowerCaseName = ZLUnicodeUtil::toLower(myName);

		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".gz")) {
			myName = myName.substr(0, myName.length() - 3);
			lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 3);
			myArchiveType = (ArchiveType)(myArchiveType | GZIP);
		}
		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".bz2")) {
			myName = myName.substr(0, myName.length() - 4);
			lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 4);
			myArchiveType = (ArchiveType)(myArchiveType | BZIP2);
		}
		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".zip")) {
			myArchiveType = (ArchiveType)(myArchiveType | ZIP);
		} else if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tar")) {
			myArchiveType = (ArchiveType)(myArchiveType | TAR);
		} else if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tgz") ||
							 ZLStringUtil::stringEndsWith(lowerCaseName, ".ipk")) {
			//myName = myName.substr(0, myName.length() - 3) + "tar";
			myArchiveType = (ArchiveType)(myArchiveType | TAR | GZIP);
		}
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
	if ((myArchiveType & BZIP2) && (stream != 0)) {
		return new ZLBzip2InputStream(stream);
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
		myInfo = ZLFSManager::instance().fileInfo(myPath.substr(0, index));
		myInfo.IsDirectory = false;
	}
	myInfoIsFilled = true;
}

bool ZLFile::remove() const {
	if (ZLFSManager::instance().removeFile(myPath)) {
		myInfoIsFilled = false;
		return true;
	} else {
		return false;
	}
}

void ZLFile::forceArchiveType(ArchiveType type) {
	if (myArchiveType != type) {
		myArchiveType = type;
		ZLFSManager::instance().myForcedFiles[myPath] = myArchiveType;
	}
}
