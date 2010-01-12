/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLTextParagraph.h>

#include "ContentsView.h"
#include "BookTextView.h"
#include "FBReader.h"

#include "../bookmodel/BookModel.h"

ContentsView::ContentsView(ZLPaintContext &context) : FBView(context) {
}

ContentsView::~ContentsView() {
}

bool ContentsView::_onStylusMove(int x, int y) {
	FBReader &fbreader = FBReader::Instance();

	int index = textArea().paragraphIndexByCoordinates(x, y);
	if ((index < 0) || ((int)textArea().model()->paragraphsNumber() <= index)) {
		fbreader.setHyperlinkCursor(false);
		return true;
	}

	const ContentsModel &contentsModel = (const ContentsModel&)*textArea().model();
	const ZLTextTreeParagraph *paragraph = (const ZLTextTreeParagraph*)contentsModel[index];
	
	fbreader.setHyperlinkCursor(contentsModel.reference(paragraph) >= 0);
	return true;
}

bool ContentsView::_onStylusPress(int x, int y) {
	FBReader &fbreader = FBReader::Instance();

	const ContentsModel &contentsModel = (const ContentsModel&)*textArea().model();
	int index = textArea().paragraphIndexByCoordinates(x, y);
	if ((index < 0) || ((int)contentsModel.paragraphsNumber() <= index)) {
		return false;
	}

	const ZLTextTreeParagraph *paragraph = (const ZLTextTreeParagraph*)contentsModel[index];
	
	int reference = contentsModel.reference(paragraph);

	if (reference >= 0) {
		fbreader.bookTextView().gotoParagraph(reference);
		fbreader.showBookTextView();
	}

	return true;
}

bool ContentsView::isEmpty() const {
	shared_ptr<ZLTextModel> model = textArea().model();
	return model.isNull() || model->paragraphsNumber() == 0;
}

size_t ContentsView::currentTextViewParagraph(bool includeStart) const {
	const ZLTextWordCursor &cursor = FBReader::Instance().bookTextView().textArea().startCursor();
	if (!cursor.isNull()) {
		long reference = cursor.paragraphCursor().index();
		bool startOfParagraph = cursor.elementIndex() == 0;
		if (cursor.isEndOfParagraph()) {
			++reference;
			startOfParagraph = true;
		}
		shared_ptr<ZLTextModel> model = textArea().model();
		size_t length = model->paragraphsNumber();
		const ContentsModel &contentsModel = (const ContentsModel&)*model;
		for (size_t i = 1; i < length; ++i) {
			long contentsReference =
				contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[i]));
			if ((contentsReference > reference) ||
					(!includeStart && startOfParagraph && (contentsReference == reference))) {
				return i - 1;
			}
		}
		return length - 1;
	}
	return (size_t)-1;
}

void ContentsView::gotoReference() {
	textArea().model()->removeAllMarks();
	const size_t selected = currentTextViewParagraph();
	highlightParagraph(selected);
	preparePaintInfo();
	gotoParagraph(selected);
	scrollPage(false, ZLTextAreaController::SCROLL_PERCENTAGE, 40);
}
