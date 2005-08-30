/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringUtil.h>

#include "TxtReader.h"

TxtReader::TxtReader() : myConverter(0) {
}

TxtReader::~TxtReader() {
}

void TxtReader::readDocument(ZLInputStream &stream, const std::string &encoding) {
	if (!stream.open()) {
		return;
	}

	myConverter.setEncoding(encoding.c_str());

	startDocumentHandler();

	const size_t BUFSIZE = 2048;
	char *buffer = new char[BUFSIZE];
	std::string str;
	size_t length;
	bool crlfFlag = false;
	do {
		length = stream.read(buffer, BUFSIZE);
		char *start = buffer;
		const char *end = buffer + length;
		for (char *ptr = start; ptr != end; ptr++) {
			if ((*ptr == '\n') || (*ptr == '\r')) {
				if (crlfFlag && (*ptr == '\n')) {
					crlfFlag = false;
					continue;
				}
				crlfFlag = *ptr == '\r';
				if (start != ptr) {
					str.erase();
					myConverter.convert(str, start, ptr);
					characterDataHandler(str);
				}
				start = ptr;
				newLineHandler();
			} else if (isspace(*ptr)) {
				*ptr = ' ';
				crlfFlag = false;
			}
		}
		if (start != end) {
			str.erase();
			myConverter.convert(str, start, end);
			characterDataHandler(str);
		}
  } while (length == BUFSIZE);
	delete[] buffer;

	endDocumentHandler();

	stream.close();
}
