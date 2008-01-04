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

#include <string.h>

#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLInputStream.h>

#include "CHMFile.h"
#include "CHMReferenceCollection.h"

#include "LZXDecompressor.h"

static std::string readString(ZLInputStream &stream, size_t length) {
	std::string string(length, ' ');
	stream.read(const_cast<char*>(string.data()), length);
	return string;
}

static unsigned short readUnsignedWord(ZLInputStream &stream) {
	unsigned char buffer[2];
	stream.read((char*)buffer, 2);
	unsigned short result = buffer[1];
	result = result << 8;
	result += buffer[0];
	return result;
}

static unsigned long readUnsignedDWord(ZLInputStream &stream) {
	unsigned long lowPart = readUnsignedWord(stream);
	unsigned long highPart = readUnsignedWord(stream);
	return (highPart << 16) + lowPart;
}

static unsigned long long readUnsignedQWord(ZLInputStream &stream) {
	unsigned long long lowPart = readUnsignedDWord(stream);
	unsigned long long highPart = readUnsignedDWord(stream);
	return (highPart << 32) + lowPart;
}

static unsigned long long readEncodedInteger(ZLInputStream &stream) {
	unsigned long long result = 0;
	char part;
	do {
		result = result << 7;
		stream.read(&part, 1);
		result += part & 0x7F;
	} while (part & -0x80);
	return result;
}

CHMInputStream::CHMInputStream(shared_ptr<ZLInputStream> base, const CHMFileInfo::SectionInfo &sectionInfo, size_t offset, size_t size) : myBase(base), mySectionInfo(sectionInfo), mySize(size) {
	myBaseStartIndex = offset / 0x8000;
	myBaseStartIndex -= myBaseStartIndex % sectionInfo.ResetInterval;
	myBytesToSkip = offset - myBaseStartIndex * 0x8000;
	myOutData = new unsigned char[0x8000];
}

CHMInputStream::~CHMInputStream() {
	close();
	delete[] myOutData;
}

bool CHMInputStream::open() {
	myOffset = 0;
	myDoSkip = true;
	myBaseIndex = myBaseStartIndex;
	if (myDecompressor.isNull()) {
		myDecompressor = new LZXDecompressor(mySectionInfo.WindowSizeIndex);
	} else {
		myDecompressor->reset();
	}
	myOutDataOffset = 0;
	myOutDataLength = 0;
	return true;
}

size_t CHMInputStream::read(char *buffer, size_t maxSize) {
	if (myDoSkip) {
		do_read(0, myBytesToSkip);
		myDoSkip = false;
	}
	size_t realSize = do_read(buffer, std::min(maxSize, mySize - myOffset));
	myOffset += realSize;
	return realSize;
}

size_t CHMInputStream::do_read(char *buffer, size_t maxSize) {
	size_t realSize = 0;
	do {
		if (myOutDataLength == 0) {
			if (myBaseIndex >= mySectionInfo.ResetTable.size()) {
				break;
			}
			const bool isTail = myBaseIndex + 1 == mySectionInfo.ResetTable.size();
			const size_t start = mySectionInfo.ResetTable[myBaseIndex];
			const size_t end = isTail ? mySectionInfo.CompressedSize : mySectionInfo.ResetTable[myBaseIndex + 1];
			myOutDataLength = isTail ? mySectionInfo.UncompressedSize % 0x8000 : 0x8000;
			myOutDataOffset = 0;

			myInData.erase();
			myInData.append(end - start, '\0');
			myBase->seek(mySectionInfo.Offset + start, true);
			myBase->read((char*)myInData.data(), myInData.length());
			if (myBaseIndex % mySectionInfo.ResetInterval == 0) {
				myDecompressor->reset();
			}
			++myBaseIndex;

			if (!myDecompressor->decompress(myInData, myOutData, myOutDataLength)) {
				break;
			}
		}
		const size_t partSize = std::min(myOutDataLength, maxSize);
		if (buffer != 0) {
			memcpy(buffer + realSize, myOutData + myOutDataOffset, partSize);
		}
		maxSize -= partSize;
		realSize += partSize;
		myOutDataLength -= partSize;
		myOutDataOffset += partSize;
	} while (maxSize != 0);
	return realSize;
}

void CHMInputStream::close() {
	myDecompressor = 0;
}

void CHMInputStream::seek(int offset, bool absoluteOffset) {
	if (absoluteOffset) {
		offset -= myOffset;
	}
	if (offset > 0) {
		read(0, offset);
	} else if (offset < 0) {
		open();
		read(0, std::max(offset + (int)myOffset, 0));
	}
}

size_t CHMInputStream::offset() const {
	return myOffset;
}

size_t CHMInputStream::sizeOfOpened() {
	return mySize;
}

shared_ptr<ZLInputStream> CHMFileInfo::entryStream(shared_ptr<ZLInputStream> base, const std::string &name) const {
	RecordMap::const_iterator it = myRecords.find(ZLUnicodeUtil::toLower(name));
	if (it == myRecords.end()) {
		return 0;
	}
	const RecordInfo &recordInfo = it->second;
	if (recordInfo.Length == 0) {
		return 0;
	}
	if (recordInfo.Section == 0) {
		// TODO: implement
		return 0;
	}
	if (recordInfo.Section > mySectionInfos.size()) {
		return 0;
	}
	const SectionInfo &sectionInfo = mySectionInfos[recordInfo.Section - 1];
	if (recordInfo.Offset + recordInfo.Length > sectionInfo.UncompressedSize) {
		return 0;
	}

	return new CHMInputStream(base, sectionInfo, recordInfo.Offset, recordInfo.Length);
}

CHMFileInfo::CHMFileInfo(const std::string &fileName) : myFileName(fileName) {
}

bool CHMFileInfo::moveToEntry(ZLInputStream &stream, const std::string &entryName) {
	RecordMap::const_iterator it = myRecords.find(entryName);
	if (it == myRecords.end()) {
		return false;
	}
	RecordInfo recordInfo = it->second;
	if (recordInfo.Section > mySectionInfos.size()) {
		return false;
	}
	if (recordInfo.Section != 0) {
		// TODO: ???
		return false;
	}

	stream.seek(mySection0Offset + recordInfo.Offset, true);
	return true;
}

bool CHMFileInfo::init(ZLInputStream &stream) {
	{
		// header start
		if (readString(stream, 4) != "ITSF") {
			return false;
		}

		unsigned long version = readUnsignedDWord(stream);

		// DWORD total length
		// DWORD unknown
		// DWORD timestamp
		// DWORD language id
		// 0x10 bytes 1st GUID
		// 0x10 bytes 2nd GUID
		// QWORD section 0 offset
		// QWORD section 0 length
		stream.seek(4 * 4 + 2 * 0x10 + 2 * 8, false);
		
		unsigned long long sectionOffset1 = readUnsignedQWord(stream);
		unsigned long long sectionLength1 = readUnsignedQWord(stream);
		mySection0Offset = sectionOffset1 + sectionLength1;
		// header end

		// additional header data start
		if (version > 2) {
			mySection0Offset = readUnsignedQWord(stream);
		}
		// additional header data end

		stream.seek(sectionOffset1, true);
		// header section 1 start
		// directory header start
		if (readString(stream, 4) != "ITSP") {
			return false;
		}

		// DWORD version
		// DWORD length
		// DWORD 0x000A
		// DWORD chunk size
		// DWORD density
		// DWORD depth
		// DWORD root chunk number
		// DWORD first chunk number
		// DWORD last chunk number
		// DWORD -1
		stream.seek(10 * 4, false);
		unsigned long dirChunkNumber = readUnsignedDWord(stream);
		// ...
		stream.seek(36, false);
		// header section 1 end

		size_t nextOffset = stream.offset();
		for (unsigned long i = 0; i < dirChunkNumber; ++i) {
			nextOffset += 4096;
			std::string header = readString(stream, 4);
			if (header == "PMGL") {
				unsigned long quickRefAreaSize = readUnsignedDWord(stream) % 4096;
				stream.seek(12, false);
				size_t startOffset = stream.offset();
				size_t oldOffset = startOffset;
				while (startOffset < nextOffset - quickRefAreaSize) {
					int nameLength = readEncodedInteger(stream);
					std::string name = readString(stream, nameLength);
					int contentSection = readEncodedInteger(stream);
					int offset = readEncodedInteger(stream);
					int length = readEncodedInteger(stream);
					if (name.substr(0, 2) != "::") {
						name = ZLUnicodeUtil::toLower(name);
					}
					myRecords.insert(
						std::pair<std::string,CHMFileInfo::RecordInfo>(
							name,
							CHMFileInfo::RecordInfo(contentSection, offset, length)
						)
					);
					startOffset = stream.offset();
					if (oldOffset == startOffset) {
						break;
					}
					oldOffset = startOffset;
				}
			} else if (header == "PMGI") {
				unsigned long quickRefAreaSize = readUnsignedDWord(stream);
				size_t startOffset = stream.offset();
				size_t oldOffset = startOffset;
				while (startOffset < nextOffset - quickRefAreaSize) {
					int nameLength = readEncodedInteger(stream);
					std::string name = readString(stream, nameLength);
					// chunk number
					readEncodedInteger(stream);
					startOffset = stream.offset();
					if (oldOffset == startOffset) {
						break;
					}
					oldOffset = startOffset;
				}
			}
			stream.seek(nextOffset, true);
			if (stream.offset() != nextOffset) {
				break;
			}
		}
	}

	{
		if (!moveToEntry(stream, "::DataSpace/NameList")) {
			return false;
		}
		stream.seek(2, false);
		const int sectionNumber = readUnsignedWord(stream);
		for (int i = 0; i < sectionNumber; ++i) {
			const int length = readUnsignedWord(stream);
			std::string sectionName;
			sectionName.reserve(length);
			for (int j = 0; j < length; ++j) {
				sectionName += (char)readUnsignedWord(stream);
			}
			stream.seek(2, false);
			mySectionNames.push_back(sectionName);
		}
	}

	{
		for (unsigned int i = 1; i < mySectionNames.size(); ++i) {
			RecordMap::const_iterator it =
				myRecords.find("::DataSpace/Storage/" + mySectionNames[i] + "/Content");
			if (it == myRecords.end()) {
				return false;
			}
			RecordInfo recordInfo = it->second;
			if (recordInfo.Section != 0) {
				return false;
			}
			mySectionInfos.push_back(SectionInfo());
			SectionInfo &info = mySectionInfos.back();
			info.Offset = mySection0Offset + recordInfo.Offset;
			info.Length = recordInfo.Length;

			if (!moveToEntry(stream, "::DataSpace/Storage/" + mySectionNames[i] + "/ControlData")) {
				return false;
			}
			stream.seek(4, false);
			std::string lzxc = readString(stream, 4);
			if (lzxc != "LZXC") {
				return false;
			}
			const int version = readUnsignedDWord(stream);
			if ((version <= 0) || (version > 2)) {
				return false;
			}
			info.ResetInterval = readUnsignedDWord(stream);
			if (version == 1) {
				info.ResetInterval /= 0x8000;
			}
			info.WindowSizeIndex = (version == 1) ? 0 : 15;
			{
				int ws = readUnsignedDWord(stream);
				if (ws > 0) {
					while ((ws & 1) == 0) {
						ws >>= 1;
						info.WindowSizeIndex++;
					}
				}
			}

			if (!moveToEntry(stream, "::DataSpace/Storage/" + mySectionNames[i] + "/Transform/{7FC28940-9D31-11D0-9B27-00A0C91E9C7C}/InstanceData/ResetTable")) {
				return false;
			}
			stream.seek(4, false);
			const size_t entriesNumber = readUnsignedDWord(stream);
			if (entriesNumber == 0) {
				return false;
			}
			if (entriesNumber > 2048) {
				// file size is greater than 60 Mb
				return false;
			}
			info.ResetTable.reserve(entriesNumber);
			stream.seek(8, false);
			info.UncompressedSize = readUnsignedQWord(stream);
			if ((info.UncompressedSize - 1) / 0x8000 != entriesNumber - 1) {
				return false;
			}
			info.CompressedSize = readUnsignedQWord(stream);
			stream.seek(8, false);
			size_t previous = 0;
			for (size_t j = 0; j < entriesNumber; ++j) {
				size_t value = readUnsignedQWord(stream);
				if ((j > 0) == (value <= previous)) {
					return false;
				}
				info.ResetTable.push_back(value);
				previous = value;
			}
		}
	}

	return true;
}

static std::string readNTString(ZLInputStream &stream) {
	std::string s;
	char c;
	while (stream.read(&c, 1) == 1) {
		if (c == '\0') {
			break;
		} else {
			s += c;
		}
	}
	return CHMReferenceCollection::fullReference("/", s);
}

bool CHMFileInfo::FileNames::empty() const {
	return Start.empty() && TOC.empty() && Home.empty() && Index.empty();
}

CHMFileInfo::FileNames CHMFileInfo::sectionNames(shared_ptr<ZLInputStream> base) const {
	FileNames names;
	shared_ptr<ZLInputStream> stringsStream = entryStream(base, "/#STRINGS");
	if (!stringsStream.isNull() && stringsStream->open()) {
		std::vector<std::string> fileNames;
		int tocIndex = -1;
		int indexIndex = -1;
		for (int i = 0; i < 12; ++i) {
			std::string argument = readNTString(*stringsStream);
			if (argument.empty() || (argument[argument.length() - 1] == '/')) {
				continue;
			}
			if (myRecords.find(argument) == myRecords.end()) {
				continue;
			}
			if ((tocIndex == -1) && ZLStringUtil::stringEndsWith(argument, ".hhc")) {
				tocIndex = fileNames.size();
				names.TOC = argument;
			} else if ((indexIndex == -1) && ZLStringUtil::stringEndsWith(argument, ".hhk")) {
				indexIndex = fileNames.size();
				names.Index = argument;
			}
			fileNames.push_back(argument);
		}
		size_t startIndex = std::max(3, std::max(tocIndex, indexIndex) + 1);
		if (startIndex < 11) {
			if (startIndex < fileNames.size()) {
				names.Start = fileNames[startIndex];
			}
			if (startIndex + 1 < fileNames.size()) {
				names.Home = fileNames[startIndex + 1];
			}
		}
		stringsStream->close();
	}
	if (names.TOC.empty()) {
		for (RecordMap::const_iterator it = myRecords.begin(); it != myRecords.end(); ++it) {
			if (ZLStringUtil::stringEndsWith(it->first, ".hhc")) {
				names.TOC = it->first;
				break;
			}
		}
	}
	if (names.empty()) {
		for (RecordMap::const_iterator it = myRecords.begin(); it != myRecords.end(); ++it) {
			if ((ZLStringUtil::stringEndsWith(it->first, ".htm")) ||
			    (ZLStringUtil::stringEndsWith(it->first, ".html"))) {
				names.Start = it->first;
				break;
			}
		}
	}

	return names;
}

const std::string CHMFileInfo::fileName() const {
	return myFileName;
}
