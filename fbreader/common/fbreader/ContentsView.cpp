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

#include "ContentsView.h"
#include "BookTextView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

ContentsView::ContentsView(FBReader &reader, ZLPaintContext &context) : TextView(context), myReader(reader) {
}

bool ContentsView::onStylusPress(int x, int y) {
	if (TextView::onStylusPress(x, y)) {
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}
	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)myModel->paragraphs().size() < paragraphNumber)) {
		return false;
	}
	Paragraph *paragraph = myModel->paragraphs()[paragraphNumber];
	
	myReader.textView().gotoParagraph(((ParagraphWithReference*)paragraph)->reference());
	myReader.showBookTextView();
	return true;
}

bool ContentsView::isEmpty() const {
	return (myModel == 0) || myModel->paragraphs().empty();
}

static const std::string PARAGRAPH_OPTION_NAME = "ContentsParagraph";
static const std::string WORD_OPTION_NAME = "ContentsWord";
static const std::string CHAR_OPTION_NAME = "ContentsChar";

void ContentsView::saveState() {
	if ((myModel == 0) || myStartCursor.isNull()) {
		return;
	}

	ZLIntegerOption(myName, PARAGRAPH_OPTION_NAME, 0).setValue(myStartCursor.paragraphCursor().paragraphNumber());
	ZLIntegerOption(myName, WORD_OPTION_NAME, 0).setValue(myStartCursor.wordNumber());
	ZLIntegerOption(myName, CHAR_OPTION_NAME, 0).setValue(myStartCursor.charNumber());
}

void ContentsView::setModel(const TextModel *model, const std::string &name) {
	TextView::setModel(model, name);

	if ((myModel != 0) && !myModel->paragraphs().empty()) {
		setStartCursor(
			ZLIntegerOption(myName, PARAGRAPH_OPTION_NAME, 0).value(),
			ZLIntegerOption(myName, WORD_OPTION_NAME, 0).value(),
			ZLIntegerOption(myName, CHAR_OPTION_NAME, 0).value()
		);
	}
}
