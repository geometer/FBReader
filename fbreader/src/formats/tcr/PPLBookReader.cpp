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

#include <cctype>

#include "PPLBookReader.h"
#include <ZLInputStream.h>

static const size_t BUFFER_SIZE = 2048;

PPLBookReader::PPLBookReader(BookModel &model, const std::string &encoding) : EncodedTextReader(encoding), myModelReader(model) {
	myBuffer = new char[BUFFER_SIZE + 1];
}

PPLBookReader::~PPLBookReader() {
	delete[] myBuffer;
}

bool PPLBookReader::currentParagraphIsEmpty() const {
	const char *ptr = myCurrentParagraph.data();
	const char *end = ptr + myCurrentParagraph.length();
	for (; ptr < end; ++ptr) {
		if (!isspace((unsigned char)*ptr)) {
			return false;
		}
	}
	return true;
}

void PPLBookReader::addParagraph() {
	static const std::string END_OF_TEXT = "<* >";
	if (!myCurrentParagraph.empty()) {
		if (currentParagraphIsEmpty()) {
			++myEmptyLineCounter;
			if (myEmptyLineCounter >= 2) {
				myModelReader.beginParagraph(ZLTextParagraph::EMPTY_LINE_PARAGRAPH);
				myModelReader.endParagraph();
			}
		} else if (myEmptyLineCounter < 2) {
			myModelReader.beginParagraph();
			myModelReader.addControl(TITLE, true);
			myModelReader.addData(myCurrentParagraph);
			myModelReader.endParagraph();
		} else if (myCurrentParagraph[0] == 9) {
			myModelReader.beginParagraph();
			myModelReader.addData(myCurrentParagraph);
			myModelReader.endParagraph();
		} else if ((myCurrentParagraph.length() >= 2) &&
				(myCurrentParagraph[0] == '*') &&
				(myCurrentParagraph[1] == ' ')) {
			myCurrentParagraph.erase(0, 2);
			myModelReader.insertEndOfSectionParagraph();
			myModelReader.beginContentsParagraph();
			myModelReader.addContentsData(myCurrentParagraph);
			myModelReader.endContentsParagraph();
			myModelReader.beginParagraph();
			myModelReader.addControl(SECTION_TITLE, true);
			myModelReader.addData(myCurrentParagraph);
			myModelReader.endParagraph();
		} else if (myCurrentParagraph.substr(0, 4) != END_OF_TEXT) {
			myModelReader.beginParagraph();
			myModelReader.addControl(SUBTITLE, true);
			myModelReader.addData(myCurrentParagraph);
			myModelReader.endParagraph();
		}
		myCurrentParagraph.erase();
	}
}

bool PPLBookReader::readDocument(ZLInputStream &stream) {
	if (!stream.open()) {
		return false;
	}

	myModelReader.setMainTextModel();
	myModelReader.pushKind(REGULAR);
	myCurrentParagraph.erase();
	myEmptyLineCounter = 0;

	// "PPL\r\n"
	stream.seek(5, false);

	size_t size;
	do {
		size = stream.read(myBuffer, BUFFER_SIZE);
		myBuffer[size] = '\0';

		const char *start = myBuffer;
		const char *end = myBuffer + size;
		const char *eol;
		do {
			eol = strchr(start, '\n');
			if (eol != 0) {
				if (start < eol) {
					myConverter->convert(myCurrentParagraph, start, eol);
				}
				addParagraph();
				start = eol + 1;
			} else {
				if (start < end) {
					myConverter->convert(myCurrentParagraph, start, end);
				}
			}
		} while (eol != 0);
	} while (size == BUFFER_SIZE);

	addParagraph();

	stream.close();

	return true;
}
