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

#include "ZLFSCompressorGzip.h"
#include "ZLZip.h"
#include "../ZLFSPluginManager.h"

ZLFSCompressorGzip::ZLFSCompressorGzip() {
}

ZLFile::ArchiveType ZLFSCompressorGzip::prepareFile(ZLFile &file, std::string &nameWithoutExt) {
    (void)file;
    std::string lowerCaseName = ZLUnicodeUtil::toLower(nameWithoutExt);
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".gz")) {
		nameWithoutExt = nameWithoutExt.substr(0, nameWithoutExt.length() - 3);
		return signature();
	}
	return ZLFile::ArchiveType();
}

const std::string ZLFSCompressorGzip::signature() const {
	return "gz";
}

shared_ptr<ZLInputStream> ZLFSCompressorGzip::envelope(ZLFile::ArchiveType &type, shared_ptr<ZLInputStream> base) {
	ZLFile::ArchiveType lastType = ZLFSPluginManager::getLastArchiver(type);
	if (lastType == signature()) {
        type = ZLFSPluginManager::stripLastArchiver(type);
		return new ZLGzipInputStream(base);
	}
	return base;
}

ZLFSCompressorGzip::~ZLFSCompressorGzip() {
}
