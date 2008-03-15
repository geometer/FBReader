/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

ContentsView::ContentsView(FBReader &reader, shared_ptr<ZLPaintContext> context) : FBView(reader, context) {
}

ContentsView::~ContentsView() {
}

bool ContentsView::_onStylusPress(int x, int y) {
	int index = paragraphIndexByCoordinate(y);
	if ((index < 0) || ((int)model()->paragraphsNumber() <= index)) {
		return false;
	}

	const ContentsModel &contentsModel = (const ContentsModel&)*model();
	const ZLTextTreeParagraph *paragraph = (const ZLTextTreeParagraph*)contentsModel[index];
	
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

size_t ContentsView::currentTextViewParagraph(bool includeStart) const {
	const ZLTextWordCursor &cursor = fbreader().bookTextView().startCursor();
	if (!cursor.isNull()) {
		long reference = cursor.paragraphCursor().index();
		bool startOfParagraph = cursor.wordNumber() == 0;
		if (cursor.isEndOfParagraph()) {
			++reference;
			startOfParagraph = true;
		}
		size_t length = model()->paragraphsNumber();
		const ContentsModel &contentsModel = (const ContentsModel&)*model();
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
	model()->removeAllMarks();
	const size_t selected = currentTextViewParagraph();
	highlightParagraph(selected);
	gotoParagraph(selected);
	scrollPage(false, ZLTextView::SCROLL_PERCENTAGE, 40);
}
