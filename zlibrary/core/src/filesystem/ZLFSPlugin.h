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

#ifndef __ZLFSPLUGIN_H__
#define __ZLFSPLUGIN_H__

#include <string>

#include <ZLFile.h>
#include <ZLUnicodeUtil.h>

class ZLFSCompressor {

public:
	ZLFSCompressor();
	virtual ~ZLFSCompressor();

public:
	virtual ZLFile::ArchiveType prepareFile(ZLFile &file, std::string &nameWithoutExt) = 0;
	virtual const std::string signature() const = 0;
	virtual shared_ptr<ZLInputStream> envelope(ZLFile::ArchiveType &type, shared_ptr<ZLInputStream> base) = 0;
};

inline ZLFSCompressor::ZLFSCompressor() {}
inline ZLFSCompressor::~ZLFSCompressor() {}

class ZLFSArchiver {

public:
	ZLFSArchiver();
	virtual ~ZLFSArchiver();

public:
	virtual ZLFile::ArchiveType prepareFile(ZLFile &file,	std::string &nameWithoutExt) = 0;
	virtual shared_ptr<ZLDir> createDirectory(const ZLFile &file, const std::string &path ) = 0;
	virtual shared_ptr<ZLInputStream> archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath) = 0;
	virtual const std::string signature() const = 0;

protected:
	void setCompressed(ZLFile &file, bool compressed);
};

inline ZLFSArchiver::ZLFSArchiver() {}
inline ZLFSArchiver::~ZLFSArchiver() {}
inline void ZLFSArchiver::setCompressed(ZLFile &file, bool compressed) { file.setCompressed(compressed); }

#endif /* __ZLFSPLUGIN_H__ */
