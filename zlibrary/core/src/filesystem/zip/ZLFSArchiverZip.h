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

#ifndef __ZLFSARCHIVERZIP_H__
#define __ZLFSARCHIVERZIP_H__

#include "../ZLFSPlugin.h"

class ZLFSArchiverZip : public ZLFSArchiver {

public:
	ZLFSArchiverZip();
	virtual ZLFile::ArchiveType prepareFile(ZLFile &file, std::string &nameWithoutExt);
	virtual const std::string signature() const;
	virtual shared_ptr<ZLDir> createDirectory(const ZLFile &file, const std::string &path);
	virtual shared_ptr<ZLInputStream> archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath);
	virtual ~ZLFSArchiverZip();
};

#endif /* __ZLFSARCHIVERZIP_H__ */
