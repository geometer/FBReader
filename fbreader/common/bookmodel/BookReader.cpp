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

#include "BookReader.h"
#include "BookModel.h"
#include "../model/Paragraph.h"

BookReader::BookReader(BookModel &model) : myModel(model) {
	myCurrentTextModel = 0;

	myTextParagraphExists = false;
	myContentsParagraphStatus = DONT_ADD;

	myInsideTitle = false;
	mySectionContainsRegularContents = false;
}

BookReader::~BookReader() {
}

void BookReader::setMainTextModel() {
	myCurrentTextModel = &myModel.myBookTextModel;
}

void BookReader::setFootnoteTextModel(const std::string &id) {
	std::map<std::string,PlainTextModel*>::iterator it = myModel.myFootnotes.find(id);
	if (it != myModel.myFootnotes.end()) {
		myCurrentTextModel = (*it).second;
	} else {
		myCurrentTextModel = new PlainTextModel();
		myModel.myFootnotes.insert(std::pair<std::string,PlainTextModel*>(id, myCurrentTextModel));
	}
}

void BookReader::unsetTextModel() {
	myCurrentTextModel = 0;
}

bool BookReader::currentTextModelIsNull() const {
	return myCurrentTextModel == 0;
}

void BookReader::pushKind(TextKind kind) {
	myKindStack.push_back(kind);
}

bool BookReader::popKind() {
	if (!myKindStack.empty()) {
		myKindStack.pop_back();
		return true;
	}
	return false;
}

void BookReader::beginParagraph(Paragraph::Kind kind) {
	if (!currentTextModelIsNull()) {
		myCurrentTextModel->createParagraph(kind);
		for (std::vector<TextKind>::const_iterator it = myKindStack.begin(); it != myKindStack.end(); it++) {
			myCurrentTextModel->addControl(*it, true);
		}
		myTextParagraphExists = true;
	}
}

void BookReader::endParagraph() {
	if (myTextParagraphExists) {
		flushTextBufferToParagraph();
		myTextParagraphExists = false;
	}
}

void BookReader::addControl(TextKind kind, bool start) {
	if (myTextParagraphExists) {
		flushTextBufferToParagraph();
		myCurrentTextModel->addControl(kind, start);
	}
}

void BookReader::addControl(const ForcedControlEntry &entry) {
	if (myTextParagraphExists) {
		flushTextBufferToParagraph();
		myCurrentTextModel->addControl(entry);
	}
}

void BookReader::addHyperlinkControl(TextKind kind, const std::string &label) {
	if (myTextParagraphExists) {
		flushTextBufferToParagraph();
		myCurrentTextModel->addHyperlinkControl(kind, label);
	}
}

void BookReader::addHyperlinkLabel(const std::string &label) {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		int paragraphNumber = myModel.bookTextModel().paragraphsNumber();
		if (myTextParagraphExists) {
			paragraphNumber--;
		}
		addHyperlinkLabel(label, paragraphNumber);
	}
}

void BookReader::addHyperlinkLabel(const std::string &label, int paragraphNumber) {
	myModel.myInternalHyperlinks.insert(std::pair<std::string,int>(label, paragraphNumber));
}

void BookReader::addDataToBuffer(const char *data, int len) {
	if ((len > 0) && myTextParagraphExists) {
		myBuffer.push_back(std::string());
		myBuffer.back().append(data, len);
	}
}

void BookReader::addDataToBuffer(std::string &data) {
	if (myTextParagraphExists) {
		myBuffer.push_back(std::string());
		myBuffer.back().swap(data);
	}
}

void BookReader::flushTextBufferToParagraph() {
	if (myBuffer.empty() || !myTextParagraphExists) {
		return;
	}

	if (!myInsideTitle) {
		mySectionContainsRegularContents = true;
	}

	myCurrentTextModel->addText(myBuffer);
	if (myContentsParagraphStatus != DONT_ADD) {
		if (myContentsParagraphStatus == TO_ADD) {
			myModel.myContentsModel.createParagraphWithReference(myModel.bookTextModel().paragraphsNumber() - 1);
			myModel.myContentsModel.addControl(CONTENTS_TABLE_ENTRY, true);
			myContentsParagraphStatus = ADDED;
		} else if (myInsideTitle) {
			myModel.myContentsModel.addText(" ");
		}
		if (myInsideTitle) {
			myModel.myContentsModel.addText(myBuffer);
		}
	}
	myBuffer.clear();
}

void BookReader::addImage(const std::string &id, ZLImage *image) {
	myModel.myImages.insert(std::pair<std::string,ZLImage*>(id, image));
}

void BookReader::insertEndOfSectionParagraph() {
	if (!currentTextModelIsNull() && mySectionContainsRegularContents) {
		size_t size = myCurrentTextModel->paragraphsNumber();
		if ((size > 0) && (((*myCurrentTextModel)[(size_t)-1])->kind() != Paragraph::END_OF_SECTION_PARAGRAPH)) {
			myCurrentTextModel->createParagraph(Paragraph::END_OF_SECTION_PARAGRAPH);
			mySectionContainsRegularContents = false;
		}
	}
}

void BookReader::insertEndOfTextParagraph() {
	if (!currentTextModelIsNull() && mySectionContainsRegularContents) {
		size_t size = myCurrentTextModel->paragraphsNumber();
		if ((size > 0) && (((*myCurrentTextModel)[(size_t)-1])->kind() != Paragraph::END_OF_TEXT_PARAGRAPH)) {
			myCurrentTextModel->createParagraph(Paragraph::END_OF_TEXT_PARAGRAPH);
			mySectionContainsRegularContents = false;
		}
	}
}

void BookReader::addImageReference(const std::string &id) {
	mySectionContainsRegularContents = true;
	if (myTextParagraphExists) {
		flushTextBufferToParagraph();
		myCurrentTextModel->addImage(id, myModel.imageMap());
	} else {
		beginParagraph();
		myCurrentTextModel->addControl(IMAGE, true);
		myCurrentTextModel->addImage(id, myModel.imageMap());
		myCurrentTextModel->addControl(IMAGE, false);
		endParagraph();
	}
}

void BookReader::beginContentsParagraph() {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		myContentsParagraphStatus = TO_ADD;
	}
}

void BookReader::endContentsParagraph() {
	if (myContentsParagraphStatus == ADDED) {
		if (myModel.myContentsModel[(size_t)-1]->entryNumber() == 1) {
			myModel.myContentsModel.addText("...");
		}
	}
	myContentsParagraphStatus = DONT_ADD;
}
