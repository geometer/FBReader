/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLInputStream.h>
#include "ZLFSPlugin.h"
#include "ZLFSPluginManager.h"

#include "zip/ZLFSCompressorGzip.h"
#include "bzip2/ZLFSCompressorBzip2.h"
#include "zip/ZLFSArchiverZip.h"
#include "tar/ZLFSArchiverTar.h"

static const char ARCHIVERS_SEPARATOR = '.';

ZLFSPluginManager::ZLFSPluginManager() {
	registerPlugin(new ZLFSCompressorGzip);
	registerPlugin(new ZLFSCompressorBzip2);
	registerPlugin(new ZLFSArchiverZip);
	registerPlugin(new ZLFSArchiverTar);
}

void ZLFSPluginManager::registerPlugin(shared_ptr<ZLFSCompressor> plugin) {
	myCompressors.push_back(plugin);
}

void ZLFSPluginManager::registerPlugin(shared_ptr<ZLFSArchiver> plugin) {
	myArchivers.push_back(plugin);
}

std::string ZLFSPluginManager::stripLastArchiver(const std::string &archiveType) {
	size_t lastDotIndex = archiveType.rfind(ARCHIVERS_SEPARATOR);
	if (lastDotIndex != std::string::npos) {
		return std::string(archiveType.begin(), archiveType.begin() + lastDotIndex);
	}
	return std::string();
}

std::string ZLFSPluginManager::getLastArchiver(const std::string &archiveType) {
	std::string lastArchiver;
	size_t lastDotIndex = archiveType.rfind(ARCHIVERS_SEPARATOR);
	if (lastDotIndex != std::string::npos) {
		lastArchiver = std::string(archiveType.begin() + lastDotIndex + 1, archiveType.end());
	} else {
		lastArchiver = archiveType;
	}
	return lastArchiver;
}

ZLFile::ArchiveType ZLFSPluginManager::combineArchiveTypes(ZLFile::ArchiveType type1, ZLFile::ArchiveType type2) {
	if (type2.empty()) {
		return type1;
	}
	return type1 + ARCHIVERS_SEPARATOR + type2;
}

ZLFile::ArchiveType ZLFSPluginManager::prepareFile(ZLFile &file, std::string &nameWithoutExt) {
	ZLFile::ArchiveType result;
	bool anyRecognized;
	do {
		anyRecognized = false;
		for (Compressors::iterator it = myCompressors.begin(); it != myCompressors.end(); ++it) {
			ZLFile::ArchiveType detected = (*it)->prepareFile(file, nameWithoutExt);
			if (!detected.empty()) {
				result = combineArchiveTypes(detected, result);
				file.setCompressed(true);
				anyRecognized = true;
			}
		}
	} while (anyRecognized);

	for (Archivers::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		ZLFile::ArchiveType detected = (*it)->prepareFile(file, nameWithoutExt);
		if (!detected.empty()) {
			result = combineArchiveTypes(detected, result);
			file.setArchived(true);
		}
	}
	return result;
}

shared_ptr<ZLInputStream> ZLFSPluginManager::envelope(const ZLFile &file, shared_ptr<ZLInputStream> &base) {
	ZLFile::ArchiveType archiveType = file.archiveType();
	ZLFile::ArchiveType archiveTypeBefore;

	do {
		archiveTypeBefore = archiveType;
		for (Compressors::iterator it = myCompressors.begin(); it != myCompressors.end(); ++it) {
			base = (*it)->envelope(archiveType, base);
		}
	} while (!archiveType.empty() && archiveTypeBefore != archiveType);

	return base;
}

shared_ptr<ZLDir> ZLFSPluginManager::createDirectory(const ZLFile &file, const std::string &path) {
	for (Archivers::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		shared_ptr<ZLDir> directory = (*it)->createDirectory(file, path);
		if (!directory.isNull()) {
			return directory;
		}
	}
	return 0;
}

shared_ptr<ZLInputStream> ZLFSPluginManager::archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath) {
	for (Archivers::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		shared_ptr<ZLInputStream> stream = (*it)->archiveInputStream(file, base, subpath);
		if (!stream.isNull()) {
			return stream;
		}
	}
	return base;
}

ZLFSPluginManager::~ZLFSPluginManager() {
}
