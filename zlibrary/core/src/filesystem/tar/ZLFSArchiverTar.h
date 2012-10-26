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

#ifndef __ZLFSARCHIVERTAR_H__
#define __ZLFSARCHIVERTAR_H__

#include "../ZLFSPlugin.h"

class ZLFSArchiverTar : public ZLFSArchiver {

public:
	ZLFSArchiverTar();
	virtual const std::string signature() const;
	virtual ZLFile::ArchiveType prepareFile(ZLFile &file, std::string &nameWithoutExt);
	virtual shared_ptr<ZLDir> createDirectory(const ZLFile &file, const std::string &path);
	virtual shared_ptr<ZLInputStream> archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath);
	virtual ~ZLFSArchiverTar();
};

#endif /* __ZLFSARCHIVERTAR_H__ */
