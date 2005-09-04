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
	myCurrentParagraph = 0;
	myCurrentContentsParagraph = 0;

	myProcessData = false;

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
		myCurrentParagraph = (kind == Paragraph::TEXT_PARAGRAPH) ? new Paragraph() : new SpecialParagraph(kind);
		for (std::vector<TextKind>::const_iterator it = myKindStack.begin(); it != myKindStack.end(); it++) {
			myCurrentParagraph->addControl(*it, true, myModel.bookTextModel().allocator());
		}
		myCurrentTextModel->addParagraph(myCurrentParagraph);
		myProcessData = true;
	}
}

void BookReader::endParagraph() {
	if (myCurrentParagraph != 0) {
		flushTextBufferToParagraph();
		myCurrentParagraph = 0;
		myProcessData = false;
	}
}

void BookReader::addControl(TextKind kind, bool start) {
	if (myCurrentParagraph != 0) {
		flushTextBufferToParagraph();
		myCurrentParagraph->addControl(kind, start, myModel.bookTextModel().allocator());
	}
}

void BookReader::addControl(const ForcedControlEntry &entry) {
	if (myCurrentParagraph != 0) {
		flushTextBufferToParagraph();
		myCurrentParagraph->addControl(entry, myModel.bookTextModel().allocator());
	}
}

void BookReader::addHyperlinkControl(TextKind kind, const std::string &label) {
	if (myCurrentParagraph != 0) {
		flushTextBufferToParagraph();
		myCurrentParagraph->addHyperlinkControl(kind, label, myModel.bookTextModel().allocator());
	}
}

void BookReader::addHyperlinkLabel(const std::string &label) {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		int paragraphNumber = myModel.bookTextModel().paragraphs().size();
		if (myCurrentParagraph != 0) {
			paragraphNumber--;
		}
		addHyperlinkLabel(label, paragraphNumber);
	}
}

void BookReader::addHyperlinkLabel(const std::string &label, int paragraphNumber) {
	myModel.myInternalHyperlinks.insert(std::pair<std::string,int>(label, paragraphNumber));
}

void BookReader::addDataToBuffer(const char *data, int len) {
	if ((len > 0) && myProcessData) {
		myBuffer.push_back(std::string());
		myBuffer.back().append(data, len);
	}
}

void BookReader::addDataToBuffer(std::string &data) {
	if (myProcessData) {
		myBuffer.push_back(std::string());
		myBuffer.back().swap(data);
	}
}

void BookReader::flushTextBufferToParagraph() {
	if (myBuffer.empty() || (myCurrentParagraph == 0)) {
		return;
	}

	if (!myInsideTitle) {
		mySectionContainsRegularContents = true;
	}

	myCurrentParagraph->addText(myBuffer, myCurrentTextModel->allocator());
	if (myCurrentContentsParagraph != 0) {
		if (myInsideTitle) {
			if (myCurrentContentsParagraph->reference() != -1) {
				myCurrentContentsParagraph->addText(" ", myModel.contentsModel().allocator());
			}
			myCurrentContentsParagraph->addText(myBuffer, myModel.contentsModel().allocator());
		}
		if (myCurrentContentsParagraph->reference() == -1) {
			myCurrentContentsParagraph->setReference(myModel.bookTextModel().paragraphs().size() - 1);
		}
	}
	myBuffer.clear();
}

void BookReader::addImage(const std::string &id, ZLImage *image) {
	myModel.myImages.insert(std::pair<std::string,ZLImage*>(id, image));
}

void BookReader::beginImageData() {
	myProcessData = true;
}

void BookReader::endImageData() {
	myProcessData = false;
}

void BookReader::insertEndOfSectionParagraph() {
	if (!currentTextModelIsNull() &&
			mySectionContainsRegularContents &&
			!myCurrentTextModel->paragraphs().empty() &&
			(myCurrentTextModel->paragraphs().back()->kind() != Paragraph::END_OF_SECTION_PARAGRAPH)) {
		myCurrentTextModel->addParagraph(new SpecialParagraph(Paragraph::END_OF_SECTION_PARAGRAPH));
		mySectionContainsRegularContents = false;
	}
}

void BookReader::insertEndOfTextParagraph() {
	if (!currentTextModelIsNull() &&
			mySectionContainsRegularContents &&
			!myCurrentTextModel->paragraphs().empty() &&
			(myCurrentTextModel->paragraphs().back()->kind() != Paragraph::END_OF_TEXT_PARAGRAPH)) {
		myCurrentTextModel->addParagraph(new SpecialParagraph(Paragraph::END_OF_TEXT_PARAGRAPH));
		mySectionContainsRegularContents = false;
	}
}

void BookReader::addImageReference(const std::string &id) {
	mySectionContainsRegularContents = true;
	bool createSeparateParagraph = myCurrentParagraph == 0;
	if (createSeparateParagraph) {
		beginParagraph();
		myCurrentParagraph->addControl(IMAGE, true, myModel.bookTextModel().allocator());
		myCurrentParagraph->addImage(id, myModel.imageMap(), myModel.bookTextModel().allocator());
		myCurrentParagraph->addControl(IMAGE, false, myModel.bookTextModel().allocator());
		endParagraph();
	} else {
		flushTextBufferToParagraph();
		myCurrentParagraph->addImage(id, myModel.imageMap(), myModel.bookTextModel().allocator());
	}
}

void BookReader::beginContentsParagraph() {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		myCurrentContentsParagraph = new ParagraphWithReference();
		myCurrentContentsParagraph->addControl(CONTENTS_TABLE_ENTRY, true, myModel.contentsModel().allocator());
	}
}

void BookReader::endContentsParagraph() {
	if (myCurrentContentsParagraph != 0) {
		if (myCurrentContentsParagraph->reference() == -1) {
			delete myCurrentContentsParagraph;
		} else {
			if (myCurrentContentsParagraph->entryNumber() == 1) {
				myCurrentContentsParagraph->addText("...", myModel.contentsModel().allocator());
			}
			myModel.myContentsModel.addParagraph(myCurrentContentsParagraph);
		}
		myCurrentContentsParagraph = 0;
	}
}
