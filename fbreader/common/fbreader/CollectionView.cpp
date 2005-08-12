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

#include "CollectionView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

#include "../collection/BookCollection.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

CollectionView::CollectionView(FBReader &reader, ZLPaintContext &context) : TextView(context), myReader(reader) {
	myCollection = 0;
	myTreeModel = 0;
	myLastBooksModel = 0;
	myLastBooksAreShown = false;
	myTreeStateIsFrozen = false;
}

CollectionView::~CollectionView() {
	rebuild();
}

void CollectionView::showLastBooks(bool show) {
	myLastBooksAreShown = show;
	setModel(myLastBooksAreShown ? (TreeModel*)myLastBooksModel : myTreeModel, "Library");
}

static const std::string LIBRARY = "Library";

const std::string &CollectionView::caption() const {
	return LIBRARY;
}

void CollectionView::gotoParagraph(int num, bool last) {
	if ((num >= 0) && (num < (int)myTreeModel->paragraphs().size())) {
		TreeParagraph *tp = (TreeParagraph*)myTreeModel->paragraphs()[num];
		if (myTreeStateIsFrozen) {
			TreeParagraph *parent = tp->parent();
			while ((num > 0) && (parent != 0) && !parent->isOpen()) {
				for (num--; ((num > 0) && parent != myTreeModel->paragraphs()[num]); num--);
				parent = parent->parent();
			}
		} else {
			tp->openTree();
		}
	}
	
	TextView::gotoParagraph(num, last);
}

void CollectionView::paint() {
	if (myLastBooksAreShown) {
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
			setModel(myLastBooksModel, "Library");
		}
	} else {
		if ((myCollection == 0) || (!myCollection->isActual())) {
			myCollection = new BookCollection();
			myTreeModel = new TreeModel();
			const std::vector<const Author*> &authors = myCollection->authors();
			for (std::vector<const Author*>::const_iterator it = authors.begin(); it != authors.end(); it++) {
				TreeParagraph *authorParagraph = myTreeModel->createParagraph();
				authorParagraph->addControl(LIBRARY_AUTHOR_ENTRY, true);
				authorParagraph->addText((*it)->displayName());
				const Books &books = myCollection->books(*it);
				for (Books::const_iterator jt = books.begin(); jt != books.end(); jt++) {
					TreeParagraph *bookParagraph = myTreeModel->createParagraph(authorParagraph);
					bookParagraph->addControl(LIBRARY_BOOK_ENTRY, true);
					bookParagraph->addText((*jt)->title());
					myBooksMap[bookParagraph] = *jt;
				}
			}
			setModel(myTreeModel, "Library");
		}
	}
	TextView::paint();
}

void CollectionView::rebuild() {
	setModel(0, "Library");
	myBooksMap.clear();
	if (myCollection != 0) {
		delete myCollection;
		delete myTreeModel;
		myCollection = 0;
		myTreeModel = 0;
	}
	if (myLastBooksModel != 0) {
		delete myLastBooksModel;
		myLastBooksModel = 0;
	}
}

bool CollectionView::onStylusPress(int x, int y) {
	myTreeStateIsFrozen = true;
	bool processedByParent = TextView::onStylusPress(x, y);
	myTreeStateIsFrozen = false;
	if (processedByParent) {
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}

	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)myModel->paragraphs().size() <= paragraphNumber)) {
		return false;
	}

	if (myModel == myTreeModel) {
		TreeParagraph *paragraph = (TreeParagraph*)myModel->paragraphs()[paragraphNumber];
		if (!paragraph->children().empty()) {
			const TextElementPosition *elementPosition = elementByCoordinates(x, y);
			if ((elementPosition == 0) || (elementPosition->Kind != TextElement::TREE_ELEMENT)) {
				return false;
			}

			paragraph->open(!paragraph->isOpen());
			if (myStartCursor.isNull()) {
				myStartCursor = myEndCursor;
			}
			if (!myStartCursor.isNull()) {
				myLineInfos.clear();
				myEndCursor = 0;
			}
			repaintView();
			if (paragraph->isOpen()) {
				// TODO: correct next paragraph number calculation for multi-level trees
				int nextParagraphNumber = paragraphNumber + paragraph->children().size() + 1;
				int lastParagraphNumber = myEndCursor.paragraphCursor().paragraphNumber();
				if (myEndCursor.isEndOfParagraph()) {
					lastParagraphNumber++;
				}
				if (lastParagraphNumber < nextParagraphNumber) {
					gotoParagraph(nextParagraphNumber, true);
					repaintView();
				}
			}
			int firstParagraphNumber = myStartCursor.paragraphCursor().paragraphNumber();
			if (myStartCursor.isStartOfParagraph()) {
				firstParagraphNumber--;
			}
			if (firstParagraphNumber >= paragraphNumber) {
				gotoParagraph(paragraphNumber);
				repaintView();
			}
		} else {
			std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
			if (it != myBooksMap.end()) {
				myReader.openBook(it->second);
				myReader.showBookTextView();
			}
		}
	} else /* if (myModel == myLastBooksModel) */ {
		Paragraph *paragraph = (Paragraph*)myModel->paragraphs()[paragraphNumber];
		std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
		if (it != myBooksMap.end()) {
			myReader.openBook(it->second);
			myReader.showBookTextView();
		}
	}
	return true;
}
