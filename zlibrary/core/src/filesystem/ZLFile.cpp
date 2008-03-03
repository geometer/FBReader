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

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLFile.h"
#include "ZLFSDir.h"
#include "ZLOutputStream.h"
#include "zip/ZLZip.h"
#include "tar/ZLTar.h"
#include "bzip2/ZLBzip2InputStream.h"
#include "ZLFSManager.h"

ZLFile::ZLFile(const std::string &path) : myPath(path), myInfoIsFilled(false) {
	ZLFSManager::instance().normalize(myPath);
	{
		size_t index = ZLFSManager::instance().findLastFileNameDelimiter(myPath);
		if (index < myPath.length() - 1) {
			myNameWithExtension = myPath.substr(index + 1);
		} else {
			myNameWithExtension = myPath;
		}
	}
	myNameWithoutExtension = myNameWithExtension;

	std::map<std::string,ArchiveType> &forcedFiles = ZLFSManager::instance().myForcedFiles;
	std::map<std::string,ArchiveType>::iterator it = forcedFiles.find(myPath);
	if (it != forcedFiles.end()) {
		myArchiveType = it->second;
	} else {
		myArchiveType = NONE;
		std::string lowerCaseName = ZLUnicodeUtil::toLower(myNameWithoutExtension);

		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".gz")) {
			myNameWithoutExtension = myNameWithoutExtension.substr(0, myNameWithoutExtension.length() - 3);
			lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 3);
			myArchiveType = (ArchiveType)(myArchiveType | GZIP);
		}
		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".bz2")) {
			myNameWithoutExtension = myNameWithoutExtension.substr(0, myNameWithoutExtension.length() - 4);
			lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 4);
			myArchiveType = (ArchiveType)(myArchiveType | BZIP2);
		}
		if (ZLStringUtil::stringEndsWith(lowerCaseName, ".zip")) {
			myArchiveType = (ArchiveType)(myArchiveType | ZIP);
		} else if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tar")) {
			myArchiveType = (ArchiveType)(myArchiveType | TAR);
		} else if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tgz") ||
							 ZLStringUtil::stringEndsWith(lowerCaseName, ".ipk")) {
			//myNameWithoutExtension = myNameWithoutExtension.substr(0, myNameWithoutExtension.length() - 3) + "tar";
			myArchiveType = (ArchiveType)(myArchiveType | TAR | GZIP);
		}
	}

	int index = myNameWithoutExtension.rfind('.');
	if (index > 0) {
		myExtension = myNameWithoutExtension.substr(index + 1);
		myNameWithoutExtension = myNameWithoutExtension.substr(0, index);
	}
}

shared_ptr<ZLInputStream> ZLFile::inputStream() const {
	if (isDirectory()) {
		return 0;
	}

	ZLInputStream *stream = 0;
	
	int index = ZLFSManager::instance().findArchiveFileNameDelimiter(myPath);
	if (index == -1) {
		stream = ZLFSManager::instance().createPlainInputStream(myPath);
	} else {
		ZLFile baseFile(myPath.substr(0, index));
		shared_ptr<ZLInputStream> base = baseFile.inputStream();
		if (!base.isNull()) {
			if (baseFile.myArchiveType & ZIP) {
				stream = new ZLZipInputStream(base, myPath.substr(index + 1));
			} else if (baseFile.myArchiveType & TAR) {
				stream = new ZLTarInputStream(base, myPath.substr(index + 1));
			}
		}
	}

	if (stream != 0) {
		if (myArchiveType & GZIP) {
			return new ZLGzipInputStream(stream);
		}
		if (myArchiveType & BZIP2) {
			return new ZLBzip2InputStream(stream);
		}
	}
	return stream;
}

shared_ptr<ZLOutputStream> ZLFile::outputStream() const {
	if (isCompressed()) {
		return 0;
	}
	if (ZLFSManager::instance().findArchiveFileNameDelimiter(myPath) != -1) {
		return 0;
	}
	return ZLFSManager::instance().createOutputStream(myPath);
}

shared_ptr<ZLDir> ZLFile::directory(bool createUnexisting) const {
	if (exists()) {
		if (isDirectory()) {
			return ZLFSManager::instance().createPlainDirectory(myPath);
		} else if (myArchiveType & ZIP) {
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
	int index = ZLFSManager::instance().findArchiveFileNameDelimiter(myPath);
	if (index == -1) {
		myInfo = ZLFSManager::instance().fileInfo(myPath);
	} else {
		std::string archivePath = myPath.substr(0, index);
		ZLFile archive(archivePath);
		if (archive.exists()) {
			std::string itemName = myPath.substr(index + 1);
			myInfo = archive.myInfo;
			myInfo.IsDirectory = false;
			myInfo.Exists = false;
			std::vector<std::string> items;
			archive.directory()->collectFiles(items, true);
			for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
				if (*it == itemName) {
					myInfo.Exists = true;
					break;
				}
			}
		} else {
			myInfo.Exists = false;
		}
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

std::string ZLFile::physicalFilePath() const {
	std::string path = myPath;
	int index;
	while ((index = ZLFSManager::instance().findArchiveFileNameDelimiter(path)) != -1) {
		path = path.substr(0, index);
	}
	return path;
}

std::string ZLFile::fileNameToUtf8(const std::string &fileName) {
	return ZLFSManager::instance().convertFilenameToUtf8(fileName);
}
