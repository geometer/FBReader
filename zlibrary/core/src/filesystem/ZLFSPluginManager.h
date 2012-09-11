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

#ifndef __ZLFSPLUGINMANAGER_H__
#define __ZLFSPLUGINMANAGER_H__

#include <map>
#include <shared_ptr.h>

#include <ZLibrary.h>
#include <ZLDir.h>

#include "ZLFSPlugin.h"

class ZLFile;

class ZLFSPluginManager {

public:
  ZLFSPluginManager();
  ~ZLFSPluginManager();

public:
  ZLFile::ArchiveType prepareFile(ZLFile &file, std::string &nameWithoutExt);
  shared_ptr<ZLInputStream> envelope(const ZLFile &file, shared_ptr<ZLInputStream> &base);
  shared_ptr<ZLDir> createDirectory(const ZLFile &file, const std::string &path);
  shared_ptr<ZLInputStream> archiveInputStream(const ZLFile &file, shared_ptr<ZLInputStream> base, const std::string &subpath);

public:
  static std::string getLastArchiver(const std::string &archiversDesc);
  static std::string stripLastArchiver(const std::string &archiversDesc);
  static ZLFile::ArchiveType combineArchiveTypes(ZLFile::ArchiveType type1, ZLFile::ArchiveType type2);

protected:
  typedef std::vector< shared_ptr<ZLFSCompressor> > Compressors;
  typedef std::vector< shared_ptr<ZLFSArchiver> > Archivers;
  Compressors myCompressors;
  Archivers myArchivers;

protected:
  void registerPlugin(shared_ptr<ZLFSCompressor> plugin);
  void registerPlugin(shared_ptr<ZLFSArchiver> plugin);

};

#endif /* __ZLFSPLUGINMANAGER_H__ */
