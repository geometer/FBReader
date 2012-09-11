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

#include "ZLFSCompressorBzip2.h"
#include "ZLBzip2InputStream.h"
#include "../ZLFSPluginManager.h"

ZLFSCompressorBzip2::ZLFSCompressorBzip2() {
}

ZLFile::ArchiveType ZLFSCompressorBzip2::prepareFile(ZLFile &file, std::string &nameWithoutExt) {
    (void)file;
    std::string lowerCaseName = ZLUnicodeUtil::toLower(nameWithoutExt);
	if (ZLStringUtil::stringEndsWith(lowerCaseName, ".bz2")) {
		nameWithoutExt = nameWithoutExt.substr(0, nameWithoutExt.length() - 4);
		return signature();
	}
	return ZLFile::ArchiveType();
}

const std::string ZLFSCompressorBzip2::signature() const {
	return "bz2";
}

shared_ptr<ZLInputStream> ZLFSCompressorBzip2::envelope(ZLFile::ArchiveType &type, shared_ptr<ZLInputStream> base) {
	ZLFile::ArchiveType lastType = ZLFSPluginManager::getLastArchiver(type);
	if (lastType == signature()) {
        type = ZLFSPluginManager::stripLastArchiver(type);
		return new ZLBzip2InputStream(base);
	}
	return base;
}

ZLFSCompressorBzip2::~ZLFSCompressorBzip2() {
}
