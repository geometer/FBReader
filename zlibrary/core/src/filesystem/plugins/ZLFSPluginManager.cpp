/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
 * Contributed by Serge Osnach <serge.osnach@gmail.com>
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

#include "ZLFSPluginGz.h"
#include "ZLFSPluginBzip2.h"
#include "ZLFSPluginZip.h"
#include "ZLFSPluginTar.h"

ZLFSPluginManager::ZLFSPluginManager() {
	registerPlugin(new ZLFSCompressorGZ);
	registerPlugin(new ZLFSCompressorBzip2);
	registerPlugin(new ZLFSArchiverZip);
	registerPlugin(new ZLFSArchiverTar);
}

void ZLFSPluginManager::registerPlugin(ZLFSCompressor *plugin) {
	shared_ptr<ZLFSCompressor>ptr(plugin);
	myCompressors.push_back(ptr);
}

void ZLFSPluginManager::registerPlugin(ZLFSArchiver *plugin) {
	shared_ptr<ZLFSArchiver>ptr(plugin);
	myArchivers.push_back(ptr);
}

std::string ZLFSPluginManager::stripLastArchiver(const std::string &archivers_desc) {
	size_t last_dot = archivers_desc.rfind(ZLFSPluginManager::ArchiversSeparator);
	if (last_dot != std::string::npos) {
		return std::string(archivers_desc.begin(), archivers_desc.begin() + last_dot);
	}
	return std::string();
}

std::string ZLFSPluginManager::getLastArchiver(const std::string &archivers_desc) {
	std::string last_archiver;
	size_t last_dot = archivers_desc.rfind(ZLFSPluginManager::ArchiversSeparator);
	if (last_dot != std::string::npos) {
		last_archiver = std::string(archivers_desc.begin() + last_dot + 1, archivers_desc.end());
	} else {
		last_archiver = archivers_desc;
	}
	return last_archiver;
}

ZLFile::ArchiveType ZLFSPluginManager::combineArchiveTypes(ZLFile::ArchiveType a, ZLFile::ArchiveType b) {
	if (b.empty())
		return a;
	return a + ArchiversSeparator + b;
}

ZLFile::ArchiveType ZLFSPluginManager::PrepareFile(ZLFile *file,
		std::string &nameWithoutExt,
		std::string lowerCaseName) {
	ZLFile::ArchiveType result;
	bool any_recognized;
	do {
		any_recognized = false;
		for (myCompressors_t::iterator it = myCompressors.begin(); it != myCompressors.end(); ++it) {
			ZLFile::ArchiveType detected =
				(*it)->PrepareFile(file, nameWithoutExt, lowerCaseName);
			if (!detected.empty()) {
				result = combineArchiveTypes(detected, result);
				file->setCompressed(true);
				any_recognized = true;
			}
		}
	} while (any_recognized);

	for (myArchivers_t::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		ZLFile::ArchiveType detected =
			(*it)->PrepareFile(file, nameWithoutExt, lowerCaseName);
		if (!detected.empty()) {
			result = combineArchiveTypes(detected, result);
			file->setArchived(true);
		}
	}
	return result;
}

shared_ptr<ZLInputStream> ZLFSPluginManager::envelope(
	const ZLFile *file, shared_ptr<ZLInputStream> & base
) {
	ZLFile::ArchiveType archivers_desc = file->archiveType();
	ZLFile::ArchiveType archivers_desc_orig;

	do {
		archivers_desc_orig = archivers_desc;
		for (myCompressors_t::iterator it = myCompressors.begin(); it != myCompressors.end(); ++it) {
			base = (*it)->envelope(archivers_desc, base);
		}
	} while (!archivers_desc.empty() && archivers_desc_orig != archivers_desc);

	return base;
}

shared_ptr<ZLDir> ZLFSPluginManager::createDirectory(
	const ZLFile *file, const std::string &path) {
	for (myArchivers_t::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		shared_ptr<ZLDir> directory = (*it)->createDirectory(file, path);
		if (!directory.isNull())
			return directory;
	}
	return shared_ptr<ZLDir>();
}

shared_ptr<ZLInputStream> ZLFSPluginManager::archiveInputStream(const ZLFile *file, shared_ptr<ZLInputStream> base, const std::string &subpath) {
	for (myArchivers_t::iterator it = myArchivers.begin(); it != myArchivers.end(); ++it) {
		shared_ptr<ZLInputStream> stream = (*it)->archiveInputStream(file, base, subpath);
		if (!stream.isNull())
			return stream;
	}
	return base;
}

ZLFSPluginManager::~ZLFSPluginManager() {
}