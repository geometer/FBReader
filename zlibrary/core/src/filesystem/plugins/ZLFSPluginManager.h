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

#ifndef __ZLFSPLUGINMANAGER_H__
#define __ZLFSPLUGINMANAGER_H__

#include <map>
#include <shared_ptr.h>

#include <ZLibrary.h>
#include <ZLDir.h>

class ZLFile;
class ZLFSPlugin;

class ZLFSPluginManager{
protected:
  typedef std::vector< shared_ptr<ZLFSCompressor> > myCompressors_t;
  typedef std::vector< shared_ptr<ZLFSArchiver> > myArchivers_t;
  
  myCompressors_t myCompressors;
  myArchivers_t myArchivers;

protected:
  void registerPlugin( ZLFSCompressor * plugin);
  void registerPlugin( ZLFSArchiver * plugin);
  
public:
  static std::string getLastArchiver( const std::string & archivers_desc );
  static std::string stripLastArchiver( const std::string & archivers_desc );
  
  ZLFile::ArchiveType combineArchiveTypes( ZLFile::ArchiveType, ZLFile::ArchiveType );
public:
  /**
   * Separates archiver signatures in full description of archiver
   * examples: "tar.gz", "zip.7z.7z" 
   *
   */
  static const char ArchiversSeparator = '.';
public:
  ZLFSPluginManager();
  
  ZLFile::ArchiveType PrepareFile( ZLFile * file, 
                    std::string & nameWithoutExt, 
                    std::string lowerCaseName );
  /**
   * Envelopes stream into simple file-to-file decompression stream,
   * if any
   * 
   * @param file File description
   * @param base Stream to be enveloped
   * @return Detected archive type
   */
  shared_ptr<ZLInputStream> 
          envelope( const ZLFile * file, shared_ptr<ZLInputStream> & base );
  /**
   * Creates first-level directory reader from archive description
   * 
   * @param file File description
   * @param path Path to file ( can we fetch this from ZLFile* ? )
   * @return Directory reader
   */
  shared_ptr<ZLDir> createDirectory( const ZLFile * file, const std::string & path );
  
  /**
   * Creates input stream for subentry in archive
   * @param file
   * @param base
   * @param subpath
   * @return 
   */
  shared_ptr<ZLInputStream> archiveInputStream( const ZLFile * file, shared_ptr<ZLInputStream> base, const std::string & subpath);
  /**
   * Destructor
   */
  ~ZLFSPluginManager();
};

#endif /* __ZLFSPLUGINMANAGER_H__ */
