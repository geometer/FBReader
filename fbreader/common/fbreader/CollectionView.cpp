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

#include <abstract/ZLFileImage.h>
#include <abstract/ZLDialogManager.h>

#include "CollectionView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

#include "../collection/BookCollection.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";
static const std::string deleteImageId = "deleteImage";

CollectionView::CollectionView(FBReader &reader, ZLPaintContext &context) : TextView(context), myReader(reader) {
	myCollection = 0;
	myTreeModel = 0;
	myTreeStateIsFrozen = false;
	myImageMap[deleteImageId] = new ZLFileImage("image/png", DeleteBookImageFile, 0);
}

CollectionView::~CollectionView() {
	rebuild();
}

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
				if (myCollection->isBookExternal(*jt)) {
					bookParagraph->addText(" ");
					bookParagraph->addImage(deleteImageId, myImageMap);
				}
				myBooksMap[bookParagraph] = *jt;
			}
		}
		setModel(myTreeModel, LIBRARY);
	}
	TextView::paint();
}

void CollectionView::rebuild() {
	setModel(0, LIBRARY);
	myBooksMap.clear();
	if (myCollection != 0) {
		delete myCollection;
		delete myTreeModel;
		myCollection = 0;
		myTreeModel = 0;
	}
}

bool CollectionView::onStylusPress(int x, int y) {
	myTreeStateIsFrozen = true;
	bool processedByParent = TextView::onStylusPress(x, y);
	myTreeStateIsFrozen = false;
	if (processedByParent) {
		return true;
	}

	const TextElementPosition *imagePosition = elementByCoordinates(x, y);
	if ((imagePosition != 0) && (imagePosition->Kind == TextElement::IMAGE_ELEMENT)) {
		int paragraphNumber = imagePosition->ParagraphNumber;
		if ((paragraphNumber < 0) || ((int)model()->paragraphs().size() <= paragraphNumber)) {
			return false;
		}
		TreeParagraph *paragraph = (TreeParagraph*)model()->paragraphs()[paragraphNumber];
		std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
		if (it != myBooksMap.end()) {
			BookDescription &description = *it->second;
			const std::string question = "Remove Book\n\"" + description.title() + "\"\nfrom library?";
			ZLDialogManager::instance().informationBox("Remove Book", question.c_str(), "Yes", "No");
		}
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

	TreeParagraph *paragraph = (TreeParagraph*)model()->paragraphs()[paragraphNumber];
	if (!paragraph->children().empty()) {
		const TextElementPosition *elementPosition = elementByCoordinates(x, y);
		if ((elementPosition == 0) || (elementPosition->Kind != TextElement::TREE_ELEMENT)) {
			return false;
		}

		paragraph->open(!paragraph->isOpen());
		rebuildPaintInfo(true);
		preparePaintInfo();
		if (paragraph->isOpen()) {
			// TODO: correct next paragraph number calculation for multi-level trees
			int nextParagraphNumber = paragraphNumber + paragraph->children().size() + 1;
			int lastParagraphNumber = endCursor().paragraphCursor().paragraphNumber();
			if (endCursor().isEndOfParagraph()) {
				lastParagraphNumber++;
			}
			if (lastParagraphNumber < nextParagraphNumber) {
				gotoParagraph(nextParagraphNumber, true);
				preparePaintInfo();
			}
		}
		int firstParagraphNumber = startCursor().paragraphCursor().paragraphNumber();
		if (startCursor().isStartOfParagraph()) {
			firstParagraphNumber--;
		}
		if (firstParagraphNumber >= paragraphNumber) {
			gotoParagraph(paragraphNumber);
			preparePaintInfo();
		}
		repaintView();
	} else {
		std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
		if (it != myBooksMap.end()) {
			myReader.openBook(it->second);
			myReader.showBookTextView();
		}
	}

	return true;
}
