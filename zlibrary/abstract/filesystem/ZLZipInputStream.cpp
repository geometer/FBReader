/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ZLZipInputStream.h"
#include "ZLFileInputStream.h"

const unsigned int IN_BUFFER_SIZE = 4096;
const unsigned int OUT_BUFFER_SIZE = 65536;

struct ZipHeader {
	unsigned long Signature;
	unsigned short Version;
	unsigned short Flags;
	unsigned short CompressionMethod;
	unsigned short ModificationTime;
	unsigned short ModificationDate;
	unsigned long CRC32;
	unsigned long CompressedSize;
	unsigned long UncompressedSize;
	unsigned short NameLength;
	unsigned short ExtraLength;

	bool readFrom(ZLFileInputStream &stream);
};

bool ZipHeader::readFrom(ZLFileInputStream &stream) {
	int startOffset = stream.offset();
	stream.read((char*)&Signature, 4);
	stream.read((char*)&Version, 2);
	stream.read((char*)&Flags, 2);
	stream.read((char*)&CompressionMethod, 2);
	stream.read((char*)&ModificationTime, 2);
	stream.read((char*)&ModificationDate, 2);
	stream.read((char*)&CRC32, 4);
	stream.read((char*)&CompressedSize, 4);
	stream.read((char*)&UncompressedSize, 4);
	stream.read((char*)&NameLength, 2);
	stream.read((char*)&ExtraLength, 2);
	return (Signature == 0x04034B50) && (stream.offset() == startOffset + 30) && (NameLength != 0);
}

std::list<ZLZipEntry> ZLZipEntry::entriesList(const std::string &zipName) {
	std::list<ZLZipEntry> entries;

	ZLFileInputStream stream(zipName);
	if (stream.open()) {
		ZipHeader header;
		while (header.readFrom(stream)) {
			char *buffer = new char[header.NameLength];
			if (stream.read(buffer, header.NameLength) == header.NameLength) {
				std::string str;
				str.append(buffer, header.NameLength);
				entries.push_back(ZLZipEntry(zipName, str));
			}
			delete[] buffer;
			stream.seek(header.ExtraLength + header.CompressedSize);
			if (header.Flags & 0x04) {
				stream.seek(12);
			}
		}
		stream.close();
	}
	return entries;
}

ZLZipInputStream::ZLZipInputStream(const std::string &name) {
	int index = name.find(':');
	myFileStream = new ZLFileInputStream(name.substr(0, index));
	myCompressedFileName = name.substr(index + 1);
	myZStream = 0;
	myInBuffer = new char[IN_BUFFER_SIZE];
	myOutBuffer = new char[OUT_BUFFER_SIZE];
}

ZLZipInputStream::~ZLZipInputStream() {
	close();
	delete myFileStream;
	delete[] myInBuffer;
	delete[] myOutBuffer;
}

bool ZLZipInputStream::open() {
	close();

	if (!myFileStream->open()) {
		return false;
	}

	ZipHeader header;
	while (true) {
		if (!header.readFrom(*myFileStream)) {
			close();
			return false;
		}
		if (header.NameLength == myCompressedFileName.length()) {
			char *buffer = new char[header.NameLength];
			myFileStream->read(buffer, header.NameLength);
			std::string str;
			str.append(buffer, header.NameLength);
			delete[] buffer;
			if (str == myCompressedFileName) {
				myFileStream->seek(header.ExtraLength);
				break;
			}
		} else {
			myFileStream->seek(header.NameLength);
		}
		myFileStream->seek(header.ExtraLength + header.CompressedSize);
		if (header.Flags & 0x04) {
			myFileStream->seek(12);
		}
	}
	if (header.CompressionMethod == 0) {
		myIsDeflated = false;
	} else if (header.CompressionMethod == 8) {
		myIsDeflated = true;
	} else {
		close();
		return false;
	}
	myAvailableSize = header.CompressedSize;

	if (myIsDeflated) {
		myZStream = new z_stream;
		memset(myZStream, 0, sizeof(z_stream));
		inflateInit2(myZStream, -MAX_WBITS);
	}

	return true;
}

int ZLZipInputStream::read(char *buffer, int maxSize) {
	if (myIsDeflated) {
		while (((int)myBuffer.length() < maxSize) && (myAvailableSize > 0)) {
			unsigned int size = (myAvailableSize < IN_BUFFER_SIZE) ? myAvailableSize : IN_BUFFER_SIZE;

			myZStream->next_in = (Bytef*)myInBuffer;
			myZStream->avail_in = myFileStream->read(myInBuffer, size);
			if (myZStream->avail_in == size) {
				myAvailableSize -= size;
			} else {
				myAvailableSize = 0;
			}
			while (myZStream->avail_in > 0) {
				myZStream->avail_out = OUT_BUFFER_SIZE;
				myZStream->next_out = (Bytef*)myOutBuffer;
				int code = inflate(myZStream, Z_SYNC_FLUSH);
				if ((code != Z_OK) && (code != Z_STREAM_END)) {
					break;
				}
				myBuffer.append(myOutBuffer, OUT_BUFFER_SIZE - myZStream->avail_out);
			}
		}

		if (maxSize > (int)myBuffer.length()) {
			maxSize = myBuffer.length();
		}
		strncpy(buffer, myBuffer.data(), maxSize);
		myBuffer.erase(0, maxSize);
		return maxSize;
	} else {
		if ((int)myAvailableSize < maxSize) {
			maxSize = myAvailableSize;
		}
		myAvailableSize -= maxSize;
		return myFileStream->read(buffer, maxSize);
	}
}

void ZLZipInputStream::close() {
	if (myZStream != 0) {
		inflateEnd(myZStream);
		delete myZStream;
		myZStream = 0;
	}

	myFileStream->close();
}
