/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "../bookmodel/BookModel.h"
#include "../model/Paragraph.h"

ContentsView::ContentsView(FBReader &reader, ZLPaintContext &context) : FBView(reader, context) {
}

ContentsView::~ContentsView() {
	saveState();
}

bool ContentsView::onStylusPress(int x, int y) {
	if (FBView::onStylusPress(x, y)) {
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}
	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() < paragraphNumber)) {
		return false;
	}

	const ContentsModel &contentsModel = (const ContentsModel&)*model();
	const TreeParagraph *paragraph = (const TreeParagraph*)contentsModel[paragraphNumber];
	
	int reference = contentsModel.reference(paragraph);

	if (reference != -1) {
		fbreader().bookTextView().gotoParagraph(reference);
		fbreader().showBookTextView();
	}

	return true;
}

bool ContentsView::isEmpty() const {
	return (model() == 0) || (model()->paragraphsNumber() == 0);
}

static const std::string PARAGRAPH_OPTION_NAME = "ContentsParagraph";
static const std::string WORD_OPTION_NAME = "ContentsWord";
static const std::string CHAR_OPTION_NAME = "ContentsChar";

void ContentsView::saveState() {
	const WordCursor &cursor = startCursor();

	if (!cursor.isNull()) {
		const std::string &group = fileName();
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, PARAGRAPH_OPTION_NAME, 0).setValue(cursor.paragraphCursor().index());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, WORD_OPTION_NAME, 0).setValue(cursor.wordNumber());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, CHAR_OPTION_NAME, 0).setValue(cursor.charNumber());
	}
}

void ContentsView::setModel(shared_ptr<TextModel> model, const std::string &name) {
	TextView::setModel(model, name);

	const std::string &group = fileName();
	gotoPosition(
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, PARAGRAPH_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, WORD_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, CHAR_OPTION_NAME, 0).value()
	);
}

void ContentsView::gotoReference() {
	const WordCursor &cursor = fbreader().bookTextView().endCursor();
	if (!cursor.isNull()) {
		long reference = cursor.paragraphCursor().index();
		size_t length = model()->paragraphsNumber();
		const ContentsModel &contentsModel = (const ContentsModel&)*model();
		size_t selected =	length - 1;
		for (size_t i = 1; i < length; ++i) {
			if (contentsModel.reference(((const TreeParagraph*)contentsModel[i])) >= reference) {
				selected = i - 1;
				break;
			}
		}
		selectParagraph(selected);
		gotoParagraph(selected);
		scrollPage(false, TextView::SCROLL_PERCENTAGE, 40);
	}
}
