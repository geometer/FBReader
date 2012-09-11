/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "ZLFSArchiverZip.h"
#include "ZLZip.h"

ZLFSArchiverZip::ZLFSArchiverZip() {
}

ZLFile::ArchiveType ZLFSArchiverZip::prepareFile(ZLFile &file, std::string &nameWithoutExt) {
    (void)file;
    std::string lowerCaseName = ZLUnicodeUtil::toLower(nameWithoutExt);
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".zip")) {
		return signature();
	}
	return std::string();
}

const std::string ZLFSArchiverZip::signature() const {
	return "zip";
}

shared_ptr<ZLDir> ZLFSArchiverZip::createDirectory(const ZLFile &file, const std::string &path) {
    if (ZLStringUtil::stringStartsWith(file.archiveType(), signature())) {
        return new ZLZipDir(path);
    }
    return 0;
}

shared_ptr<ZLInputStream> ZLFSArchiverZip::archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath) {
    if (ZLStringUtil::stringStartsWith(file.archiveType(), signature())) {
        return new ZLZipInputStream(base, file.physicalFilePath(), subpath);
	}
    return 0;
}

ZLFSArchiverZip::~ZLFSArchiverZip() {
}
