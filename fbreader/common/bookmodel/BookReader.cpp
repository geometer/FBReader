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

#include "BookReader.h"
#include "BookModel.h"
#include "../model/Paragraph.h"
#include "../model/Image.h"

BookReader::BookReader(BookModel &model) : myModel(model) {
	myCurrentTextModel = 0;
	myCurrentParagraph = 0;
	myCurrentContentsParagraph = 0;
	myCurrentImage = 0;

	myInsideTitle = false;
	mySectionContainsRegularContents = false;
}

void BookReader::setMainTextModel() {
	myCurrentTextModel = &myModel.myBookTextModel;
}

void BookReader::setFootnoteTextModel(const std::string &id) {
	std::map<std::string,PlainTextModel*>::iterator it = myModel.myFootnotes.find(id);
	if (it != myModel.myFootnotes.end()) {
		myCurrentTextModel = it->second;
	} else {
		myCurrentTextModel = new PlainTextModel();
		myModel.myFootnotes[id] = myCurrentTextModel;
	}
}

void BookReader::unsetTextModel() {
	myCurrentTextModel = 0;
}

bool BookReader::currentTextModelIsMain() const {
	return myCurrentTextModel == &myModel.myBookTextModel;
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
	}
}

void BookReader::endParagraph() {
	if (myCurrentParagraph != 0) {
		flushTextBuffer();
		myCurrentParagraph = 0;
	}
}

void BookReader::addControl(TextKind kind, bool start) {
	if (myCurrentParagraph != 0) {
		flushTextBuffer();
		myCurrentParagraph->addControl(kind, start);
	}
}

void BookReader::addHyperlinkControl(TextKind kind, const std::string &label) {
	if (myCurrentParagraph != 0) {
		flushTextBuffer();
		myCurrentParagraph->addHyperlinkControl(kind, label);
	}
}

void BookReader::flushTextBuffer() {
	if (!myBuffer.empty()) {
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
		myBuffer.clear();
	}
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

void BookReader::addImageToParagraph(const std::string &id) {
	bool createSeparateParagraph = myCurrentParagraph == 0;
	if (createSeparateParagraph) {
		beginParagraph();
	} else {
		flushTextBuffer();
	}
	myCurrentParagraph->addControl(IMAGE, true);
	myCurrentParagraph->addImage(id, myModel);
	myCurrentParagraph->addControl(IMAGE, false);
	if (createSeparateParagraph) {
		endParagraph();
	}
}

void BookReader::addImageToModel(const std::string &id, Image *image) {
	myModel.myImages[id] = image;
}

void BookReader::addContentsParagraphToModel() {
	if (myCurrentContentsParagraph != 0) {
		if (myCurrentContentsParagraph->reference() == -1) {
			delete myCurrentContentsParagraph;
		} else {
			if (myCurrentContentsParagraph->entries().size() == 1) {
				myCurrentContentsParagraph->addText("...");
			}
			myModel.myContentsModel.addParagraph(myCurrentContentsParagraph);
		}
	}
}
