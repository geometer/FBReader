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
	if ((paragraphNumber < 0) || ((int)model()->paragraphs().size() < paragraphNumber)) {
		return false;
	}
	Paragraph *paragraph = model()->paragraphs()[paragraphNumber];
	
	myReader.textView().gotoParagraph(((ParagraphWithReference*)paragraph)->reference());
	myReader.showBookTextView();
	return true;
}

bool ContentsView::isEmpty() const {
	return (model() == 0) || model()->paragraphs().empty();
}

static const std::string PARAGRAPH_OPTION_NAME = "ContentsParagraph";
static const std::string WORD_OPTION_NAME = "ContentsWord";
static const std::string CHAR_OPTION_NAME = "ContentsChar";

void ContentsView::saveState() {
	const WordCursor &cursor = startCursor();

	if (!cursor.isNull()) {
		const std::string &group = fileName();
		ZLIntegerOption(group, PARAGRAPH_OPTION_NAME, 0).setValue(cursor.paragraphCursor().index());
		ZLIntegerOption(group, WORD_OPTION_NAME, 0).setValue(cursor.wordNumber());
		ZLIntegerOption(group, CHAR_OPTION_NAME, 0).setValue(cursor.charNumber());
	}
}

void ContentsView::setModel(const TextModel *model, const std::string &name) {
	TextView::setModel(model, name);

	const std::string &group = fileName();
	moveStartCursor(
		ZLIntegerOption(group, PARAGRAPH_OPTION_NAME, 0).value(),
		ZLIntegerOption(group, WORD_OPTION_NAME, 0).value(),
		ZLIntegerOption(group, CHAR_OPTION_NAME, 0).value()
	);
}

void ContentsView::gotoReference() {
	const WordCursor &cursor = myReader.textView().endCursor();
	if (!cursor.isNull()) {
		long reference = cursor.paragraphCursor().index();
		const std::vector<Paragraph*> &paragraphs = model()->paragraphs();
		unsigned int selected = paragraphs.size() - 1;
		for (std::vector<Paragraph*>::const_iterator it = paragraphs.begin() + 1; it != paragraphs.end(); it++) {
			if (((ParagraphWithReference*)*it)->reference() >= reference) {
				selected = it - paragraphs.begin() - 1;
				break;
			}
		}
		selectParagraph(selected);
		gotoParagraph(selected);
		if (selected != paragraphs.size() - 1) {
			scrollPage(false, TextView::SCROLL_PERCENTAGE, 40);
		}
	}
}
