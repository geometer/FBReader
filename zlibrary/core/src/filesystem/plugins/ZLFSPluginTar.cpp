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

#include "ZLFSPluginTar.h"
#include "../tar/ZLTar.h"


ZLFSArchiverTar::ZLFSArchiverTar() {
}

ZLFile::ArchiveType ZLFSArchiverTar::PrepareFile(ZLFile *file,
		std::string &nameWithoutExt,
		std::string &lowerCaseName) {
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tar")) {
		nameWithoutExt = nameWithoutExt.substr(0, nameWithoutExt.length() - 4);
		lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 4);
		return "tar";
	}
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".tgz")
			||ZLStringUtil::stringEndsWith(lowerCaseName, ".ipk")) {
		nameWithoutExt = nameWithoutExt.substr(0, nameWithoutExt.length() - 4);
		lowerCaseName = lowerCaseName.substr(0, lowerCaseName.length() - 4);

		// Note, that 'tgz' and 'ipk' files are also compressed
		setCompressed(file, true);
		return "tar.gz";
	}
	return std::string();
}

const std::string ZLFSArchiverTar::signature() const {
	return "tar";
}

shared_ptr<ZLDir> ZLFSArchiverTar::createDirectory(
	const ZLFile *file, const std::string &path) {
	if (file->archiveType().find("tar") != 0)
		return NULL;
	return new ZLTarDir(path);
}

shared_ptr<ZLInputStream> ZLFSArchiverTar::archiveInputStream(
	const ZLFile *file, shared_ptr<ZLInputStream> base, const std::string &subpath) {
	if (file->archiveType().find("tar") == 0) {
		return new ZLTarInputStream(base,subpath);
	}
	return NULL;
}


ZLFSArchiverTar::~ZLFSArchiverTar() {
}