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

#include <ctype.h>
#include <iostream>

#include <abstract/ZLInputStream.h>

#include "HtmlBookReader.h"
#include "../../bookmodel/BookModel.h"

HtmlBookReader::HtmlBookReader(BookModel &model) : BookReader(model) {
}

void HtmlBookReader::readBook(ZLInputStream &stream) {
	if (!stream.open()) {
	}

	setMainTextModel();
	pushKind(REGULAR);
	beginParagraph();

	bool insideTag = false;
	bool insideTagName = false;
	std::string tagName;
	
	const size_t BUFSIZE = 2048;
	char buffer[BUFSIZE];
	size_t length;
	do {
		length = stream.read(buffer, BUFSIZE);
		char *start = buffer;
		char *endOfBuffer = buffer + length;
		for (char *ptr = buffer; ptr < endOfBuffer; ptr++) {
			if (insideTag) {
				if (*ptr == '>') {
					insideTag = false;
					if (insideTagName) {
						tagName.append(start, ptr - start);
					}
					std::cerr << tagName << "\n";
					if ((tagName == "p") || (tagName == "P")) {
						endParagraph();
						beginParagraph();
					}
					tagName.clear();
					start = ptr + 1;
				} if (insideTagName && isspace(*ptr)) {
					tagName.append(start, ptr - start);
					insideTagName = false;
				}
			} else {
				if (*ptr == '<') {
					insideTag = true;
					insideTagName = true;
					if (start != ptr) {
						myBuffer.push_back(std::string());
						myBuffer.back().append(start, ptr - start);
					}
					start = ptr + 1;
				}
			}
		}
		if (start != endOfBuffer) {
			if (!insideTag) {
				myBuffer.push_back(std::string());
				myBuffer.back().append(start, endOfBuffer - start);
			} else if (insideTagName) {
				tagName.append(start, endOfBuffer - start);
			}
		}
  } while (length == BUFSIZE);

	endParagraph();
	stream.close();
}
