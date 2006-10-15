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

#include <abstract/ZLFileImage.h>
#include <abstract/ZLDialogManager.h>

#include "CollectionView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

#include "../collection/BookCollection.h"
#include "../collection/BookList.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";
static const std::string deleteImageId = "deleteImage";
//static const std::string orderImageId = "orderImage";

CollectionView::CollectionView(FBReader &reader, ZLPaintContext &context) : TreeView(reader, context) {
	myTreeModel = 0;
	//myImageMap[orderImageId] = new ZLFileImage("image/png", DeleteBookImageFile, 0);
	myImageMap[deleteImageId] = new ZLFileImage("image/png", DeleteBookImageFile, 0);
}

CollectionView::~CollectionView() {
	if (myTreeModel != 0) {
		delete myTreeModel;
	}

	for (ImageMap::iterator image = myImageMap.begin(); image != myImageMap.end(); ++image) {
		if (image->second != 0) {
			delete image->second;
		}
	}
}

const std::string &CollectionView::caption() const {
	return LIBRARY;
}

void CollectionView::paint() {
	if (!myCollection.isSynchronized()) {
		rebuild();
	}
	if (myTreeModel == 0) {
		myTreeModel = new TreeModel();
		const std::vector<AuthorPtr> &authors = myCollection.authors();
		std::string currentSequenceName;
		TreeParagraph *sequenceParagraph;
		for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
			const Books &books = myCollection.books(*it);
			if (!books.empty()) {
				currentSequenceName.erase();
				sequenceParagraph = 0;

				TreeParagraph *authorParagraph = myTreeModel->createParagraph();
				myTreeModel->addControl(LIBRARY_AUTHOR_ENTRY, true);
				myTreeModel->addText((*it)->displayName());
				for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
					const std::string &sequenceName = (*jt)->sequenceName();
					if (sequenceName.empty()) {
						currentSequenceName.erase();
						sequenceParagraph = 0;
					} else if (sequenceName != currentSequenceName) {
						currentSequenceName = sequenceName;
						sequenceParagraph = myTreeModel->createParagraph(authorParagraph);
						myTreeModel->addControl(LIBRARY_BOOK_ENTRY, true);
						myTreeModel->addText(sequenceName);
						//myTreeModel->addText(" ");
						//myTreeModel->addImage(orderImageId, myImageMap);
					}
					TreeParagraph *bookParagraph = myTreeModel->createParagraph(
						(sequenceParagraph == 0) ? authorParagraph : sequenceParagraph
					);
					myTreeModel->addControl(LIBRARY_BOOK_ENTRY, true);
					myTreeModel->addText((*jt)->title());
					if (myCollection.isBookExternal(*jt)) {
						myTreeModel->addText(" ");
						myTreeModel->addImage(deleteImageId, myImageMap);
					}
					myBooksMap[bookParagraph] = *jt;
				}
			}
		}
		setModel(myTreeModel, LIBRARY);
	}
	TextView::paint();
}

void CollectionView::rebuild() {
	setModel(0, LIBRARY);
	myBooksMap.clear();
	if (myTreeModel != 0) {
		delete myTreeModel;
		myTreeModel = 0;
	}
	myCollection.rebuild();
}

bool CollectionView::onStylusPress(int x, int y) {
	if (TreeView::onStylusPress(x, y)) {
		return true;
	}

	const TextElementPosition *imagePosition = elementByCoordinates(x, y);
	if ((imagePosition != 0) && (imagePosition->Kind == TextElement::IMAGE_ELEMENT)) {
		int paragraphNumber = imagePosition->ParagraphNumber;
		if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() <= paragraphNumber)) {
			return false;
		}
		/*
		WordCursor cursor;
		cursor.moveToParagraph(imagePosition->ParagraphNumber);
		cursor.moveTo(imagePosition->TextElementNumber, 0);
		TextElement &element = cursor.element();
		*/
		TreeParagraph *paragraph = (TreeParagraph*)(*myTreeModel)[paragraphNumber];
		std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
		if (it != myBooksMap.end()) {
			BookDescription &description = *it->second;
			const std::string question = "Remove Book\n\"" + description.title() + "\"\nfrom library?";
			if (ZLDialogManager::instance().infoBox(ZLDialogManager::INFORMATION_TYPE, "Remove Book", question, "Yes", "No") == 0) {
				BookList().removeFileName(description.fileName());
				myTreeModel->removeParagraph(paragraphNumber);
				rebuildPaintInfo(true);
				repaintView();
			}
		}
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}

	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() <= paragraphNumber)) {
		return false;
	}

	std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find((*myTreeModel)[paragraphNumber]);
	if (it != myBooksMap.end()) {
		fbreader().openBook(it->second);
		fbreader().showBookTextView();
	}

	return true;
}
