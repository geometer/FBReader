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

#include "RecentBooksView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../collection/BookCollection.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";

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
		const Books &books = lastBooks.books();
		for (Books::const_iterator it = books.begin(); it != books.end(); it++) {
			Paragraph *p = new Paragraph(Paragraph::TEXT_PARAGRAPH);
			p->addControl(RECENT_BOOK_LIST, true);
			p->addControl(LIBRARY_AUTHOR_ENTRY, true);
			p->addText((*it)->author()->displayName());
			p->addText(". ");
			p->addControl(LIBRARY_AUTHOR_ENTRY, false);
			p->addControl(LIBRARY_BOOK_ENTRY, true);
			p->addText((*it)->title());
			myLastBooksModel->addParagraph(p);
			myBooksMap[p] = *it;
		}
		setModel(myLastBooksModel, LIBRARY);
	}
	TextView::paint();
}

void RecentBooksView::rebuild() {
	setModel(0, LIBRARY);
	myBooksMap.clear();
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
	if (position == 0) {
		return false;
	}

	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)model()->paragraphs().size() <= paragraphNumber)) {
		return false;
	}

	Paragraph *paragraph = (Paragraph*)model()->paragraphs()[paragraphNumber];
	std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
	if (it != myBooksMap.end()) {
		myReader.openBook(it->second);
		myReader.showBookTextView();
	}
	return true;
}
