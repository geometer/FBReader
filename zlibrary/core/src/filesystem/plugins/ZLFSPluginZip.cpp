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
#include <ZLStringUtil.h>
#include <ZLFile.h>

#include "ZLFSPluginZip.h"
#include "../zip/ZLZip.h"


ZLFSArchiverZip::ZLFSArchiverZip() {
}

ZLFile::ArchiveType ZLFSArchiverZip::PrepareFile(ZLFile *file,
		std::string &nameWithoutExt,
		std::string &lowerCaseName) {
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".zip")) {
		nameWithoutExt = nameWithoutExt.substr(0, nameWithoutExt.length() - 3);
		lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 3);

		return signature();
	}
	return std::string();
}

const std::string ZLFSArchiverZip::signature() const {
	return "zip";
}

shared_ptr<ZLDir> ZLFSArchiverZip::createDirectory(
	const ZLFile *file, const std::string &path
) {
	if (file->archiveType().find("zip") != 0)
		return NULL;
	return new ZLZipDir(path);
}

shared_ptr<ZLInputStream> ZLFSArchiverZip::archiveInputStream(
	const ZLFile *file, shared_ptr<ZLInputStream> base, const std::string &subpath
) {
	if (file->archiveType().find("zip") == 0) {
		return new ZLZipInputStream(base,subpath);
	}
	return NULL;
}

ZLFSArchiverZip::~ZLFSArchiverZip() {
}