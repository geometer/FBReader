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

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "RecentBooksView.h"
#include "FBReader.h"

#include "../bookmodel/FBTextKind.h"
#include "../description/Author.h"

RecentBooksView::RecentBooksView(FBReader &reader, shared_ptr<ZLPaintContext> context) : FBView(reader, context) {
}

RecentBooksView::~RecentBooksView() {
	rebuild();
}

const std::string &RecentBooksView::caption() const {
	static const std::string LIBRARY = "Recent Books";
	return LIBRARY;
}

void RecentBooksView::paint() {
	if (model().isNull()) {
		ZLTextPlainModel *recentBooksModel = new ZLTextPlainModel(8192);
		const Books &books = myLastBooks.books();
		for (Books::const_iterator it = books.begin(); it != books.end(); ++it) {
			recentBooksModel->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
			recentBooksModel->addControl(RECENT_BOOK_LIST, true);
			recentBooksModel->addControl(LIBRARY_AUTHOR_ENTRY, true);
			recentBooksModel->addText((*it)->author()->displayName() + ". ");
			recentBooksModel->addControl(LIBRARY_AUTHOR_ENTRY, false);
			recentBooksModel->addControl(LIBRARY_BOOK_ENTRY, true);
			recentBooksModel->addText((*it)->title());
		}
		setModel(recentBooksModel);
	}
	FBView::paint();
}

void RecentBooksView::rebuild() {
	setModel(0);
}

bool RecentBooksView::_onStylusPress(int x, int y) {
	const Books &books = myLastBooks.books();

	int index = paragraphIndexByCoordinate(y);
	if ((index == -1) || (index >= (int)books.size())) {
		return false;
	}

	fbreader().openBook(books[index]);
	fbreader().showBookTextView();
	return true;
}
