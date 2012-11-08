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

#ifndef __ZLFILE_H__
#define __ZLFILE_H__

#include <string>
#include <map>

#include <shared_ptr.h>
#include <ZLFileInfo.h>
#include <ZLMimeType.h>

class ZLDir;
class ZLInputStream;
class ZLOutputStream;
class ZLFSCompressor;
class ZLFSArchiver;
class ZLFSPluginManager;

class ZLFile {
  friend class ZLFSPluginManager;
  friend class ZLFSArchiver;

public:
	static const ZLFile NO_FILE;

private:
	static std::map<std::string,weak_ptr<ZLInputStream> > ourPlainStreamCache;

public:
	static std::string fileNameToUtf8(const std::string &fileName);
	static std::string replaceIllegalCharacters(const std::string &fileName, char replaceWith);

public:
	static const std::string ZIP; // = "zip"
	 typedef std::string ArchiveType; //ArchiveType is a string like "tag.gz", that describes sequence of compressors/archivers need to be used
	ArchiveType archiveType() const;
private:
	void setArchiveType(const ArchiveType type);
	void setCompressed(bool compressed);
	void setArchived(bool archived);
private:
	ZLFile();

public:
	explicit ZLFile(const std::string &path, shared_ptr<ZLMimeType> mimeType = ZLMimeType::EMPTY);
	~ZLFile();

	bool exists() const;
	std::size_t size() const;

	void forceArchiveType(const std::string & type) const;

	bool isCompressed() const;
	bool isDirectory() const;
	bool isArchive() const;

	bool remove() const;
	bool canRemove() const;

	const std::string &path() const;
	const std::string &name(bool hideExtension) const;
	const std::string &extension() const;

	shared_ptr<ZLMimeType> mimeType() const;

	std::string physicalFilePath() const;
	std::string resolvedPath() const;

	shared_ptr<ZLInputStream> inputStream() const;
	shared_ptr<ZLOutputStream> outputStream(bool writeThrough = false) const;
	shared_ptr<ZLDir> directory(bool createUnexisting = false) const;

	bool operator == (const ZLFile &other) const;
	bool operator != (const ZLFile &other) const;
	bool operator < (const ZLFile &other) const;

private:
	void fillInfo() const;
	shared_ptr<ZLInputStream> envelopeCompressedStream(shared_ptr<ZLInputStream> &base) const;

private:
	std::string myPath;
	std::string myNameWithExtension;
	std::string myNameWithoutExtension;
	std::string myExtension;
	mutable shared_ptr<ZLMimeType> myMimeType;
	mutable bool myMimeTypeIsUpToDate;
	mutable ArchiveType myArchiveType;
	bool myIsCompressed;
	bool myIsArchive;
	mutable ZLFileInfo myInfo;
	mutable bool myInfoIsFilled;
};

inline ZLFile::~ZLFile() {}

inline bool ZLFile::isCompressed() const { return myIsCompressed; }
inline bool ZLFile::isArchive() const { return myIsArchive; }
inline ZLFile::ArchiveType ZLFile::archiveType() const { return myArchiveType; }
inline void ZLFile::setCompressed(bool compressed) { myIsCompressed = compressed; }
inline void ZLFile::setArchived(bool archived) { myIsArchive = archived; }

inline const std::string &ZLFile::path() const { return myPath; }
inline const std::string &ZLFile::name(bool hideExtension) const { return hideExtension ? myNameWithoutExtension : myNameWithExtension; }
inline const std::string &ZLFile::extension() const { return myExtension; }

inline bool ZLFile::operator == (const ZLFile &other) const { return myPath == other.myPath; }
inline bool ZLFile::operator != (const ZLFile &other) const { return myPath != other.myPath; }
inline bool ZLFile::operator < (const ZLFile &other) const { return myPath < other.myPath; }

#endif /* __ZLFILE_H__ */
