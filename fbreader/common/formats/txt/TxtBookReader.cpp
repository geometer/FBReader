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

#include "TxtBookReader.h"
#include "../../bookmodel/BookModel.h"

TxtBookReader::TxtBookReader(BookModel &model) : BookReader(model) {
}

void TxtBookReader::flushTextBufferToParagraph() {
	if (myIsPreformatted) {
		std::string fullText;
		ZLStringUtil::append(fullText, myBuffer);
		myBuffer.clear();
		int index = -1;
		do {
			//TODO: process '\t' and ' ' symbols
			int oldIndex = index + 1;
			index = fullText.find('\n', oldIndex);
			int len = ((index == -1) ? fullText.length() : index) - oldIndex;
			if (len > 0) {
				addDataToBuffer(fullText.data() + oldIndex, len);
			} else {
				addDataToBuffer(" ");
			}
			myIsPreformatted = false;
			endParagraph();
			beginParagraph();
			myIsPreformatted = true;
		} while (index != -1);
	} else {
		if (!myIsStarted) {
			for (std::vector<std::string>::const_iterator it = myBuffer.begin(); !myIsStarted && (it != myBuffer.end()); it++) {
				for (std::string::const_iterator jt = it->begin(); jt != it->end(); jt++) {
					if (!isspace(*jt)) {
						myIsStarted = true;
						break;
					}
				}
			}
		}
		if (myIsStarted) {
			myConverter->convertBuffer(myBuffer);
			BookReader::flushTextBufferToParagraph();
		}
	}
}

bool TxtBookReader::characterDataHandler(const char *text, int len) {
	if (myIgnoreDataCounter == 0) {
		addDataToBuffer(text, len);
	}
	return true;
}

void TxtBookReader::startDocumentHandler() {
	setMainTextModel();
	pushKind(REGULAR);
	beginParagraph();
	myIgnoreDataCounter = 0;
	myIsPreformatted = false;
	myIsHyperlink = false;
	myIsStarted = false;
}

void TxtBookReader::endDocumentHandler() {
	endParagraph();
}
