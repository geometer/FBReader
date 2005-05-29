/*
 * FBReader -- electronic book reader
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

#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringInputStream.h>

#include <abstract/ZLStringUtil.h>

#include "../docbook/DocBookReader.h"
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
	do {
		length = stream.read(buffer, BUFSIZE);
		char *ptr = buffer;
		for (size_t i = 0; i < length; i++) {
			if (buffer[i] == '\n') {
				if (ptr != buffer + i) {
					str.erase();
					myConverter.convert(str, ptr, buffer + i);
					characterDataHandler(str);
				}
				ptr = buffer + i;
				newLineHandler();
			}
		}
		if (ptr != buffer + length) {
			str.erase();
			myConverter.convert(str, ptr, buffer + length);
			characterDataHandler(str);
		}
  } while (length == BUFSIZE);
	delete[] buffer;

	endDocumentHandler();

	stream.close();
}
