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

#include <cctype>

#include "TxtBookReader.h"
#include "../../bookmodel/BookModel.h"

TxtBookReader::TxtBookReader(BookModel &model, const PlainTextFormat &format) : BookReader(model), myFormat(format) {
}

void TxtBookReader::flushTextBufferToParagraph() {
	if (!myBufferIsEmpty) {
		BookReader::flushTextBufferToParagraph();
		myLineFeedCounter = 0;
	}
	myBufferIsEmpty = true;
}

bool TxtBookReader::characterDataHandler(const std::string &str) {
	const char *ptr = str.data();
	const char *end = ptr + str.length();
	for (; ptr != end; ptr++) {
		if (isspace(*ptr)) {
			mySpaceCounter++;
		} else {
			myBufferIsEmpty = false;
			break;
		}
	}
	if (ptr != end) {
		if ((myFormat.breakType() & PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT) &&
				myNewLine && (mySpaceCounter > myFormat.ignoredIndent() + 1)) {
			endParagraph();
			beginParagraph();
		}
		addDataToBuffer(str);
		myNewLine = false;
	}
	return true;
}

bool TxtBookReader::newLineHandler() {
	flushTextBufferToParagraph();
	myLineFeedCounter++;
	myNewLine = true;
	mySpaceCounter = 0;
	bool paragraphBreak =
		(myFormat.breakType() & PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) ||
		((myFormat.breakType() & PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE) && (myLineFeedCounter > 1));

	if (myFormat.createContentsTable()) {
		if (!myInsideContentsParagraph && (myLineFeedCounter == myFormat.emptyLinesBeforeNewSection() + 1)) {
			myInsideContentsParagraph = true;
			endParagraph();
			insertEndOfSectionParagraph();
			beginContentsParagraph();
			enterTitle();
			pushKind(SECTION_TITLE);
			beginParagraph();
			paragraphBreak = false;
		}
		if (myInsideContentsParagraph && (myLineFeedCounter == 1)) {
			exitTitle();
			endContentsParagraph();
			popKind();
			myInsideContentsParagraph = false;
			paragraphBreak = true;
		}
	}

	if (paragraphBreak) {
		endParagraph();
		beginParagraph();
	}
	return true;
}

void TxtBookReader::startDocumentHandler() {
	setMainTextModel();
	pushKind(REGULAR);
	beginParagraph();
	myLineFeedCounter = 1;
	myInsideContentsParagraph = false;
	enterTitle();
	myBufferIsEmpty = true;
	myNewLine = true;
	mySpaceCounter = 0;
}

void TxtBookReader::endDocumentHandler() {
	endParagraph();
}
