/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLXMLReader.h>

#include "XMLTextStream.h"

class XMLTextReader : public ZLXMLReader {

public:
	XMLTextReader(std::string &buffer);

private:
	void characterDataHandler(const char *text, size_t len);

private:
	std::string &myBuffer;
};

XMLTextReader::XMLTextReader(std::string &buffer) : myBuffer(buffer) {
}

void XMLTextReader::characterDataHandler(const char *text, size_t len) {
	myBuffer.append(text, len);
}

XMLTextStream::XMLTextStream(shared_ptr<ZLInputStream> base) : myBase(base), myStreamBuffer(2048, '\0') {
	myReader = new XMLTextReader(myDataBuffer);
}

XMLTextStream::~XMLTextStream() {
}

bool XMLTextStream::open() {
	close();
	if (myBase.isNull() || !myBase->open()) {
		return false;
	}
	myReader->initialize();
	myOffset = 0;
	return true;
}

size_t XMLTextStream::read(char *buffer, size_t maxSize) {
	while (myDataBuffer.size() < maxSize) {
		size_t len = myBase->read((char*)myStreamBuffer.data(), 2048);
		if ((len == 0) || !myReader->readFromBuffer(myStreamBuffer.data(), len)) {
			break;
		}
	}
	size_t realSize = std::min(myDataBuffer.size(), maxSize);
	if (buffer != 0) {
		memcpy(buffer, myDataBuffer.data(), realSize);
	}
	myDataBuffer.erase(0, realSize);
	myOffset += realSize;
	return realSize;
}

void XMLTextStream::close() {
	myReader->shutdown();
	myBase->close();
	myDataBuffer.clear();
}

void XMLTextStream::seek(int offset, bool absoluteOffset) {
	// works for nonnegative offsets only
	if (absoluteOffset) {
		offset -= myOffset;
	}
	read(0, offset);
}

size_t XMLTextStream::offset() const {
	return myOffset;
}

size_t XMLTextStream::sizeOfOpened() {
	// couldn't be implemented
	return 0;
}
