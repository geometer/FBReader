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

#ifndef __CHMFILE_H__
#define __CHMFILE_H__

#include <string>
#include <map>
#include <vector>

#include <shared_ptr.h>
#include <ZLInputStream.h>

class ZLFile;

class LZXDecompressor;

class CHMFileInfo {

public:
	struct FileNames {
		std::string TOC;
		std::string Index;
		std::string Start;
		std::string Home;

		bool empty() const;
	};

public:
	CHMFileInfo(const ZLFile &file);
	bool init(ZLInputStream &stream);
	// We assume that base exists and is already open
	shared_ptr<ZLInputStream> entryStream(shared_ptr<ZLInputStream> base, const std::string &name) const;
	// We assume that base exists and is already open
	FileNames sectionNames(shared_ptr<ZLInputStream> base) const;
	const std::string filePath() const;

private:
	bool moveToEntry(ZLInputStream &stream, const std::string &entryName);

private:
	unsigned long long mySection0Offset;

	struct RecordInfo {
		RecordInfo(int section, int offset, int length) : Section(section), Offset(offset), Length(length) {}
		std::size_t Section;
		std::size_t Offset;
		std::size_t Length;
	};

	typedef std::map<std::string,RecordInfo> RecordMap;
	RecordMap myRecords;
	std::vector<std::string> mySectionNames;

	struct SectionInfo {
		std::size_t WindowSizeIndex;
		std::size_t ResetInterval;
		std::size_t Offset;
		std::size_t Length;
		std::size_t CompressedSize;
		std::size_t UncompressedSize;
		std::vector<std::size_t> ResetTable;
	};
	std::vector<SectionInfo> mySectionInfos;

	const std::string myFilePath;

private:
	CHMFileInfo(const CHMFileInfo&);
	const CHMFileInfo &operator = (const CHMFileInfo&);

friend class CHMInputStream;
};

class CHMInputStream : public ZLInputStream {

public:
	CHMInputStream(shared_ptr<ZLInputStream> base, const CHMFileInfo::SectionInfo &sectionInfo, std::size_t offset, std::size_t size);
	~CHMInputStream();

	bool open();
	std::size_t read(char *buffer, std::size_t maxSize);
	void close();

	void seek(int offset, bool absoluteOffset);
	std::size_t offset() const;
	std::size_t sizeOfOpened();

private:
	std::size_t do_read(char *buffer, std::size_t maxSize);

private:
	shared_ptr<ZLInputStream> myBase;
	const CHMFileInfo::SectionInfo mySectionInfo;
	std::size_t myBaseStartIndex;
	std::size_t myBaseIndex;
	std::size_t myBytesToSkip;
	const std::size_t mySize;

	std::size_t myOffset;
	bool myDoSkip;

	shared_ptr<LZXDecompressor> myDecompressor;
	std::string myInData;
	
	unsigned char *myOutData;
	std::size_t myOutDataOffset;
	std::size_t myOutDataLength;
};

#endif /* __CHMFILE_H__ */
