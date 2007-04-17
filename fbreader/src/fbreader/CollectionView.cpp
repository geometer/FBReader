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

#include <ZLFileImage.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>

#include "CollectionView.h"
#include "FBReader.h"
#include "BookInfoDialog.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

#include "../collection/BookCollection.h"
#include "../collection/BookList.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";
static const std::string DELETE_IMAGE_ID = "delete";
static const std::string BOOK_INFO_IMAGE_ID = "bookInfo";
static const std::string AUTHOR_INFO_IMAGE_ID = "authorInfo";
static const std::string SERIES_ORDER_IMAGE_ID = "seriesOrder";

class CollectionModel : public TreeModel {

public:
	CollectionModel(BookCollection &collection);
	~CollectionModel();

	BookDescriptionPtr bookByParagraphNumber(int num);
	int paragraphNumberByBook(BookDescriptionPtr book);

	void update();

private:
	void build();

	void insertText(TextKind kind, const std::string &text);
	void insertImage(const std::string &id);

private:
	BookCollection &myCollection;

	ImageMap myImageMap;
	std::map<Paragraph*,BookDescriptionPtr> myParagraphToBook;
	std::map<BookDescriptionPtr,int> myBookToParagraph;
};

CollectionModel::CollectionModel(BookCollection &collection) : TreeModel(), myCollection(collection) {
	const std::string prefix = ZLApplication::ApplicationImageDirectory() + ZLApplication::FileNameDelimiter;
	myImageMap[DELETE_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-remove.png", 0);
	myImageMap[BOOK_INFO_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-bookinfo.png", 0);
	myImageMap[AUTHOR_INFO_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-authorinfo.png", 0);
	myImageMap[SERIES_ORDER_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-order.png", 0);
}

CollectionModel::~CollectionModel() {
}

BookDescriptionPtr CollectionModel::bookByParagraphNumber(int num) {
	if ((num < 0) || ((int)paragraphsNumber() <= num)) {
		return 0;
	}
	std::map<Paragraph*,BookDescriptionPtr>::iterator it = myParagraphToBook.find((*this)[num]);
	return (it != myParagraphToBook.end()) ? it->second : 0;
}

int CollectionModel::paragraphNumberByBook(BookDescriptionPtr book) {
	std::map<BookDescriptionPtr,int>::iterator it = myBookToParagraph.find(book);
	return (it != myBookToParagraph.end()) ? it->second : -1;
}

void CollectionModel::build() {
	const std::vector<AuthorPtr> &authors = myCollection.authors();
	std::string currentSequenceName;
	TreeParagraph *sequenceParagraph;
	for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		const Books &books = myCollection.books(*it);
		if (!books.empty()) {
			currentSequenceName.erase();
			sequenceParagraph = 0;

			TreeParagraph *authorParagraph = createParagraph();
			insertText(LIBRARY_AUTHOR_ENTRY, (*it)->displayName());
			//insertImage(AUTHOR_INFO_IMAGE_ID);
			for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
				const std::string &sequenceName = (*jt)->sequenceName();
				if (sequenceName.empty()) {
					currentSequenceName.erase();
					sequenceParagraph = 0;
				} else if (sequenceName != currentSequenceName) {
					currentSequenceName = sequenceName;
					sequenceParagraph = createParagraph(authorParagraph);
					insertText(LIBRARY_BOOK_ENTRY, sequenceName);
					//insertImage(SERIES_ORDER_IMAGE_ID);
				}
				TreeParagraph *bookParagraph = createParagraph(
					(sequenceParagraph == 0) ? authorParagraph : sequenceParagraph
				);
				insertText(LIBRARY_BOOK_ENTRY, (*jt)->title());
				insertImage(BOOK_INFO_IMAGE_ID);
				if (myCollection.isBookExternal(*jt)) {
					insertImage(DELETE_IMAGE_ID);
				}
				myParagraphToBook[bookParagraph] = *jt;
				myBookToParagraph[*jt] = paragraphsNumber() - 1;
			}
		}
	}
}

void CollectionModel::update() {
	myParagraphToBook.clear();
	myBookToParagraph.clear();
	for (int i = paragraphsNumber() - 1; i >= 0; --i) {
		removeParagraph(i);
	}
	build();
}

void CollectionModel::insertText(TextKind kind, const std::string &text) {
	addControl(kind, true);
	addText(text);
}

void CollectionModel::insertImage(const std::string &id) {
	addFixedHSpace(1);
	addImage(id, myImageMap, 0);
}

CollectionView::CollectionView(FBReader &reader, ZLPaintContext &context) : FBView(reader, context), myUpdateModel(true) {
	setModel(new CollectionModel(myCollection), LIBRARY);
}

CollectionView::~CollectionView() {
	setModel(0, LIBRARY);
}

void CollectionView::updateModel() {
	myCollection.rebuild(true);
	myUpdateModel = true;
}

void CollectionView::synchronizeModel() {
	if (myCollection.synchronize()) {
		updateModel();
	}
}

const std::string &CollectionView::caption() const {
	return LIBRARY;
}

void CollectionView::selectBook(BookDescriptionPtr book) {
	if (myUpdateModel) {
		shared_ptr<TextModel> oldModel = model();
		setModel(0, LIBRARY);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel, LIBRARY);
		myUpdateModel = false;
	}
	int toSelect = collectionModel().paragraphNumberByBook(book);
	if (toSelect >= 0) {
		selectParagraph(toSelect);
		gotoParagraph(toSelect);
		scrollPage(false, TextView::SCROLL_PERCENTAGE, 40);
	}
}

void CollectionView::paint() {
	if (myUpdateModel) {
		shared_ptr<TextModel> oldModel = model();
		setModel(0, LIBRARY);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel, LIBRARY);
		myUpdateModel = false;
	}
	TextView::paint();
}

bool CollectionView::onStylusPress(int x, int y) {
	if (FBView::onStylusPress(x, y)) {
		return true;
	}

	const TextElementPosition *imagePosition = elementByCoordinates(x, y);
	if ((imagePosition != 0) && (imagePosition->Kind == TextElement::IMAGE_ELEMENT)) {
		WordCursor cursor = startCursor();
		cursor.moveToParagraph(imagePosition->ParagraphNumber);
		cursor.moveTo(imagePosition->TextElementNumber, 0);
		const TextElement &element = cursor.element();
		if (element.kind() != TextElement::IMAGE_ELEMENT) {
			return false;
		}
		const ImageElement &imageElement = (ImageElement&)element;

		if (imageElement.id() == BOOK_INFO_IMAGE_ID) {
			BookDescriptionPtr book = collectionModel().bookByParagraphNumber(imagePosition->ParagraphNumber);
			if (!book.isNull()) {
				if (BookInfoDialog(myCollection, book->fileName()).dialog().run("")) {
					myCollection.rebuild(false);
					myUpdateModel = true;
					selectBook(book);
					repaintView();
				}
			}
		} else if (imageElement.id() == DELETE_IMAGE_ID) {
			BookDescriptionPtr book = collectionModel().bookByParagraphNumber(imagePosition->ParagraphNumber);
			if (!book.isNull()) {
				const std::string question = "Remove Book\n\"" + book->title() + "\"\nfrom library?";
				if (ZLDialogManager::instance().questionBox("Remove Book", question, "Yes", "No") == 0) {
					collectionModel().removeAllMarks();
					BookList().removeFileName(book->fileName());
					TreeParagraph *paragraph = (TreeParagraph*)collectionModel()[imagePosition->ParagraphNumber];
					TreeParagraph *parent = paragraph->parent();
					if (parent->children().size() == 1) {
						collectionModel().removeParagraph(imagePosition->ParagraphNumber);
						collectionModel().removeParagraph(imagePosition->ParagraphNumber - 1);
					} else {
						collectionModel().removeParagraph(imagePosition->ParagraphNumber);
					}
					if (collectionModel().paragraphsNumber() == 0) {
						setStartCursor(0);
					}
					rebuildPaintInfo(true);
					repaintView();
				}
			}
		}
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}

	BookDescriptionPtr book = collectionModel().bookByParagraphNumber(position->ParagraphNumber);
	if (!book.isNull()) {
		fbreader().openBook(book);
		fbreader().showBookTextView();
	}

	return true;
}

CollectionModel &CollectionView::collectionModel() {
	return (CollectionModel&)*model();
}
