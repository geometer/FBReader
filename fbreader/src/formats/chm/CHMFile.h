/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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
	CHMFileInfo(const std::string &fileName);
	bool init(ZLInputStream &stream);
	// We assume that base exists and is already open
	shared_ptr<ZLInputStream> entryStream(shared_ptr<ZLInputStream> base, const std::string &name) const;
	// We assume that base exists and is already open
	FileNames sectionNames(shared_ptr<ZLInputStream> base) const;
	const std::string fileName() const;

private:
	bool moveToEntry(ZLInputStream &stream, const std::string &entryName);

private:
	unsigned long long mySection0Offset;

	struct RecordInfo {
		RecordInfo(int section, int offset, int length) : Section(section), Offset(offset), Length(length) {}
		size_t Section;
		size_t Offset;
		size_t Length;
	};

	typedef std::map<std::string,RecordInfo> RecordMap;
	RecordMap myRecords;
	std::vector<std::string> mySectionNames;

	struct SectionInfo {
		size_t WindowSizeIndex;
		size_t ResetInterval;
		size_t Offset;
		size_t Length;
		size_t CompressedSize;
		size_t UncompressedSize;
		std::vector<size_t> ResetTable;
	};
	std::vector<SectionInfo> mySectionInfos;

	const std::string myFileName;

private:
	CHMFileInfo(const CHMFileInfo&);
	const CHMFileInfo &operator= (const CHMFileInfo&);

friend class CHMInputStream;
};

class CHMInputStream : public ZLInputStream {

public:
	CHMInputStream(shared_ptr<ZLInputStream> base, const CHMFileInfo::SectionInfo &sectionInfo, size_t offset, size_t size);
	~CHMInputStream();

	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(int offset, bool absoluteOffset);
	size_t offset() const;
	size_t sizeOfOpened();

private:
	size_t do_read(char *buffer, size_t maxSize);

private:
	shared_ptr<ZLInputStream> myBase;
	const CHMFileInfo::SectionInfo mySectionInfo;
	size_t myBaseStartIndex;
	size_t myBaseIndex;
	size_t myBytesToSkip;
	const size_t mySize;

	size_t myOffset;
	bool myDoSkip;

	shared_ptr<LZXDecompressor> myDecompressor;
	std::string myInData;
	
	unsigned char *myOutData;
	size_t myOutDataOffset;
	size_t myOutDataLength;
};

#endif /* __CHMFILE_H__ */
