/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "RecentBooksView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../description/Author.h"

static const std::string LIBRARY = "Recent Books";

RecentBooksView::RecentBooksView(FBReader &reader, ZLPaintContext &context) : TextView(context), myReader(reader) {
	myLastBooksModel = 0;
}

RecentBooksView::~RecentBooksView() {
	rebuild();
}

const std::string &RecentBooksView::caption() const {
	return LIBRARY;
}

void RecentBooksView::paint() {
	if (myLastBooksModel == 0) {
		myLastBooksModel = new PlainTextModel();
		const LastOpenedBooks lastBooks;
		myBooks = lastBooks.books();
		for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); it++) {
			myLastBooksModel->createParagraph(Paragraph::TEXT_PARAGRAPH);
			myLastBooksModel->addControl(RECENT_BOOK_LIST, true);
			myLastBooksModel->addControl(LIBRARY_AUTHOR_ENTRY, true);
			myLastBooksModel->addText((*it)->author()->displayName() + ". ");
			myLastBooksModel->addControl(LIBRARY_AUTHOR_ENTRY, false);
			myLastBooksModel->addControl(LIBRARY_BOOK_ENTRY, true);
			myLastBooksModel->addText((*it)->title());
		}
		setModel(myLastBooksModel, LIBRARY);
	}
	TextView::paint();
}

void RecentBooksView::rebuild() {
	setModel(0, LIBRARY);
	myBooks.clear();
	if (myLastBooksModel != 0) {
		delete myLastBooksModel;
		myLastBooksModel = 0;
	}
}

bool RecentBooksView::onStylusPress(int x, int y) {
	if (TextView::onStylusPress(x, y)) {
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if ((position == 0) || (position->ParagraphNumber >= (int)myBooks.size())) {
		return false;
	}

	myReader.openBook(myBooks[position->ParagraphNumber]);
	myReader.showBookTextView();
	return true;
}
