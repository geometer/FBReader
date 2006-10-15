/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <iostream>
#include <map>

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFile.h>
#include <abstract/ZLInputStream.h>

#include "CHMPlugin.h"
//#include "CHMDescriptionReader.h"
//#include "CHMBookReader.h"
#include "../txt/PlainTextFormat.h"
#include "../../description/BookDescription.h"

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

bool CHMPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return (extension == "CHM") || (extension == "chm");
}

bool CHMPlugin::readDescription(const std::string &/*path*/, BookDescription &/*description*/) const {
	return true;
}

static std::map<std::string,std::pair<int,int> > Section0;
static std::map<std::string,std::pair<int,int> > Section1;

bool CHMPlugin::readModel(const BookDescription &description, BookModel &/*model*/) const {
	shared_ptr<ZLInputStream> stream = ZLFile(description.fileName()).inputStream();
	if (!stream.isNull() && stream->open()) {
		// header start
		if (readString(*stream, 4) != "ITSF") {
			return 0;
		}

		unsigned long version = readUnsignedDWord(*stream);
		unsigned long totalLength = readUnsignedDWord(*stream);
		std::cerr << "version = " << version << "\n";
		std::cerr << "totalLength = " << totalLength << "\n";

		// unknown DWORD
		stream->seek(4, false);
		// timestamp DWORD
		stream->seek(4, false);
		// language id DWORD
		stream->seek(4, false);
		// 1st GUID 0x10 bytes
		stream->seek(16, false);
		// 2nd GUID 0x10 bytes
		stream->seek(16, false);

		
		unsigned long long sectionOffset0 = readUnsignedQWord(*stream);
		unsigned long long sectionLength0 = readUnsignedQWord(*stream);
		unsigned long long sectionOffset1 = readUnsignedQWord(*stream);
		unsigned long long sectionLength1 = readUnsignedQWord(*stream);
		unsigned long long contentOffset = sectionOffset1 + sectionLength1;
		std::cerr << "section : " << sectionOffset0 << " : " << sectionLength0 << "\n";
		std::cerr << "section : " << sectionOffset1 << " : " << sectionLength1 << "\n";
		// header end

		// additional header data start
		if (version > 2) {
			contentOffset = readUnsignedQWord(*stream);
		}
		std::cerr << "content offset = " << contentOffset << "\n\n";
		// additional header data end

		stream->seek(sectionOffset0, true);
		// header section 0 start
		// unknown 0x01FE0000
		stream->seek(8, false);
		unsigned long long fileSize = readUnsignedQWord(*stream);
		std::cerr << "fileSize = " << fileSize << "\n\n";
		// unknown 0x00000000
		stream->seek(8, false);
		// header section 0 end
		
		stream->seek(sectionOffset1, true);
		// header section 1 start
		// directory header start
		if (readString(*stream, 4) != "ITSP") {
			return 0;
		}

		version = readUnsignedDWord(*stream);
		unsigned long length = readUnsignedDWord(*stream);
		std::cerr << "version = " << version << "\n";
		std::cerr << "length = " << length << "\n";

		// unknown DWORD 0x000A
		stream->seek(4, false);
		unsigned long chunkSize = readUnsignedDWord(*stream);
		std::cerr << "chunk size = " << chunkSize << "\n";
		unsigned long density = readUnsignedDWord(*stream);
		std::cerr << "density = " << density << "\n";
		unsigned long depth = readUnsignedDWord(*stream);
		std::cerr << "depth = " << depth << "\n";
		long rootChunkNumber = readUnsignedDWord(*stream);
		std::cerr << "rootChunkNumber = " << rootChunkNumber << "\n";
		unsigned long firstChunkNumber = readUnsignedDWord(*stream);
		std::cerr << "firstChunkNumber = " << firstChunkNumber << "\n";
		unsigned long lastChunkNumber = readUnsignedDWord(*stream);
		std::cerr << "lastChunkNumber = " << lastChunkNumber << "\n";
		// unknown DWORD -1
		stream->seek(4, false);
		unsigned long dirChunkNumber = readUnsignedDWord(*stream);
		std::cerr << "dirChunkNumber = " << dirChunkNumber << "\n";
		// ...
		stream->seek(36, false);
		// header section 1 end

		size_t nextOffset = stream->offset();
		for (unsigned long i = 0; i < dirChunkNumber; ++i) {
			nextOffset += 4096;
			std::string header = readString(*stream, 4);
			std::cerr << i << ": " << header << "\n";
			if (header == "PMGL") {
				unsigned long quickRefAreaSize = readUnsignedDWord(*stream) % 4096;
				std::cerr << "quickRefAreaSize = " << quickRefAreaSize << "\n";
				stream->seek(12, false);
				size_t startOffset = stream->offset();
				size_t oldOffset = startOffset;
				while (startOffset < nextOffset - quickRefAreaSize) {
					int nameLength = readEncodedInteger(*stream);
					std::string name = readString(*stream, nameLength);
					int contentSection = readEncodedInteger(*stream);
					int offset = readEncodedInteger(*stream);
					int length = readEncodedInteger(*stream);
					if (contentSection == 0) {
						Section0[name] = std::pair<int,int>(offset, length);
					} else if (contentSection == 1) {
						Section1[name] = std::pair<int,int>(offset, length);
					}
					//std::cerr << name << " : " << contentSection << " : " << offset << " : " << length << "\n";
					startOffset = stream->offset();
					if (oldOffset == startOffset) {
						break;
					}
					oldOffset = startOffset;
				}
			} else if (header == "PMGI") {
				unsigned long quickRefAreaSize = readUnsignedDWord(*stream);
				size_t startOffset = stream->offset();
				size_t oldOffset = startOffset;
				while (startOffset < nextOffset - quickRefAreaSize) {
					int nameLength = readEncodedInteger(*stream);
					std::string name = readString(*stream, nameLength);
					int chunkNumber = readEncodedInteger(*stream);
					//std::cerr << name << " : " << chunkNumber << "\n";
					startOffset = stream->offset();
					if (oldOffset == startOffset) {
						break;
					}
					oldOffset = startOffset;
				}
			}
			stream->seek(nextOffset, true);
			if (stream->offset() != nextOffset) {
				break;
			}
		}

		stream->close();
	}
	return true;
}

const std::string &CHMPlugin::iconName() const {
	static const std::string ICON_NAME = "html";
	return ICON_NAME;
}
