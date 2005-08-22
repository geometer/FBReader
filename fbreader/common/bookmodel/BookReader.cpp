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

void BookReader::popKind() {
	if (!myKindStack.empty()) {
		myKindStack.pop_back();
	}
}

void BookReader::beginParagraph(Paragraph::Kind kind) {
	if (!currentTextModelIsNull()) {
		myCurrentParagraph = new Paragraph(kind);
		for (std::vector<TextKind>::const_iterator it = myKindStack.begin(); it != myKindStack.end(); it++) {
			myCurrentParagraph->addControl(*it, true);
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
		myCurrentParagraph->addControl(kind, start);
	}
}

void BookReader::addHyperlinkControl(TextKind kind, const std::string &label) {
	if (myCurrentParagraph != 0) {
		flushTextBufferToParagraph();
		myCurrentParagraph->addHyperlinkControl(kind, label);
	}
}

void BookReader::addHyperlinkLabel(const std::string &label) {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		int paragraphNumber = myModel.bookTextModel().paragraphs().size();
		if (myCurrentParagraph != 0) {
			paragraphNumber--;
		}
		myModel.myInternalHyperlinks.insert(std::pair<std::string,int>(label, paragraphNumber));
	}
}

void BookReader::addDataToBuffer(const char *data, int len) {
	if ((len > 0) && myProcessData) {
		myBuffer.push_back(ZLString());
		myBuffer.back().append(data, len);
	}
}

void BookReader::flushTextBufferToParagraph() {
	if (myBuffer.empty() || (myCurrentParagraph == 0)) {
		return;
	}

	if (!myInsideTitle) {
		mySectionContainsRegularContents = true;
	}

	// optimization: addNonConstText can change parameter if needs
	if ((myBuffer.size() == 1) && myCurrentContentsParagraph == 0) {
		myCurrentParagraph->addNonConstText(myBuffer[0]);
	} else {
		myCurrentParagraph->addText(myBuffer);
		if (myCurrentContentsParagraph != 0) {
			if (myInsideTitle) {
				if (myCurrentContentsParagraph->reference() != -1) {
					myCurrentContentsParagraph->addText(" ");
				}
				myCurrentContentsParagraph->addText(myBuffer);
			}
			if (myCurrentContentsParagraph->reference() == -1) {
				myCurrentContentsParagraph->setReference(myModel.bookTextModel().paragraphs().size() - 1);
			}
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
			(myCurrentTextModel->paragraphs().back()->kind() != Paragraph::EOS_PARAGRAPH)) {
		myCurrentTextModel->addParagraph(new Paragraph(Paragraph::EOS_PARAGRAPH));
		mySectionContainsRegularContents = false;
	}
}

void BookReader::addImageReference(const std::string &id) {
	mySectionContainsRegularContents = true;
	bool createSeparateParagraph = myCurrentParagraph == 0;
	if (createSeparateParagraph) {
		beginParagraph();
		myCurrentParagraph->addControl(IMAGE, true);
		myCurrentParagraph->addImage(id, myModel.imageMap());
		myCurrentParagraph->addControl(IMAGE, false);
		endParagraph();
	} else {
		flushTextBufferToParagraph();
		myCurrentParagraph->addImage(id, myModel.imageMap());
	}
}

void BookReader::beginContentsParagraph() {
	if (myCurrentTextModel == &myModel.myBookTextModel) {
		myCurrentContentsParagraph = new ParagraphWithReference();
		myCurrentContentsParagraph->addControl(CONTENTS_TABLE_ENTRY, true);
	}
}

void BookReader::endContentsParagraph() {
	if (myCurrentContentsParagraph != 0) {
		if (myCurrentContentsParagraph->reference() == -1) {
			delete myCurrentContentsParagraph;
		} else {
			if (myCurrentContentsParagraph->entries().size() == 1) {
				myCurrentContentsParagraph->addText("...");
			}
			myModel.myContentsModel.addParagraph(myCurrentContentsParagraph);
		}
		myCurrentContentsParagraph = 0;
	}
}
