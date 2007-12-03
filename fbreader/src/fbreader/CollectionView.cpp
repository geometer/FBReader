/*
 * Copyright (C) 2004-2007 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLFileImage.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLStringUtil.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>
#include <ZLTextParagraphCursor.h>

#include "CollectionView.h"
#include "FBReader.h"
#include "BookInfoDialog.h"

#include "../bookmodel/FBTextKind.h"
#include "../collection/BookCollection.h"
#include "../collection/BookList.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";
static const std::string DELETE_IMAGE_ID = "delete";
static const std::string BOOK_INFO_IMAGE_ID = "bookInfo";
static const std::string AUTHOR_INFO_IMAGE_ID = "authorInfo";
static const std::string SERIES_ORDER_IMAGE_ID = "seriesOrder";

class CollectionModel : public ZLTextTreeModel {

public:
	CollectionModel(BookCollection &collection);
	~CollectionModel();

	BookDescriptionPtr bookByParagraphNumber(int num);
	int paragraphNumberByBook(BookDescriptionPtr book);

	void update();

private:
	void build();

	void insertText(FBTextKind kind, const std::string &text);
	void insertImage(const std::string &id);

private:
	BookCollection &myCollection;

	ZLImageMap myImageMap;
	std::map<ZLTextParagraph*,BookDescriptionPtr> myParagraphToBook;
	std::map<BookDescriptionPtr,int> myBookToParagraph;
};

CollectionModel::CollectionModel(BookCollection &collection) : ZLTextTreeModel(), myCollection(collection) {
	const std::string prefix = ZLApplication::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
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
	std::map<ZLTextParagraph*,BookDescriptionPtr>::iterator it = myParagraphToBook.find((*this)[num]);
	return (it != myParagraphToBook.end()) ? it->second : 0;
}

int CollectionModel::paragraphNumberByBook(BookDescriptionPtr book) {
	std::map<BookDescriptionPtr,int>::iterator it = myBookToParagraph.find(book);
	return (it != myBookToParagraph.end()) ? it->second : -1;
}

void CollectionModel::build() {
	const std::vector<AuthorPtr> &authors = myCollection.authors();
	std::string currentSequenceName;
	ZLTextTreeParagraph *sequenceParagraph;
	for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		const Books &books = myCollection.books(*it);
		if (!books.empty()) {
			currentSequenceName.erase();
			sequenceParagraph = 0;

			ZLTextTreeParagraph *authorParagraph = createParagraph();
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
				ZLTextTreeParagraph *bookParagraph = createParagraph(
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

void CollectionModel::insertText(FBTextKind kind, const std::string &text) {
	addControl(kind, true);
	addText(text);
}

void CollectionModel::insertImage(const std::string &id) {
	addFixedHSpace(1);
	addImage(id, myImageMap, 0);
}

CollectionView::CollectionView(FBReader &reader, shared_ptr<ZLPaintContext> context) : FBView(reader, context), myUpdateModel(true) {
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
		shared_ptr<ZLTextModel> oldModel = model();
		setModel(0, LIBRARY);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel, LIBRARY);
		myUpdateModel = false;
	}
	int toSelect = collectionModel().paragraphNumberByBook(book);
	if (toSelect >= 0) {
		highlightParagraph(toSelect);
		gotoParagraph(toSelect);
		scrollPage(false, ZLTextView::SCROLL_PERCENTAGE, 40);
	}
}

void CollectionView::paint() {
	if (myUpdateModel) {
		shared_ptr<ZLTextModel> oldModel = model();
		setModel(0, LIBRARY);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel, LIBRARY);
		myUpdateModel = false;
	}
	FBView::paint();
}

bool CollectionView::_onStylusPress(int x, int y) {
	const ZLTextElementArea *imageArea = elementByCoordinates(x, y);
	if ((imageArea != 0) && (imageArea->Kind == ZLTextElement::IMAGE_ELEMENT)) {
		ZLTextWordCursor cursor = startCursor();
		cursor.moveToParagraph(imageArea->ParagraphNumber);
		cursor.moveTo(imageArea->TextElementNumber, 0);
		const ZLTextElement &element = cursor.element();
		if (element.kind() != ZLTextElement::IMAGE_ELEMENT) {
			return false;
		}
		const ZLTextImageElement &imageElement = (ZLTextImageElement&)element;

		BookDescriptionPtr book = collectionModel().bookByParagraphNumber(imageArea->ParagraphNumber);
		if (book.isNull()) {
			return false;
		}

		if (imageElement.id() == BOOK_INFO_IMAGE_ID) {
			if (BookInfoDialog(myCollection, book->fileName()).dialog().run()) {
				myCollection.rebuild(false);
				myUpdateModel = true;
				selectBook(book);
				application().refreshWindow();
			}
			return true;
		} else if (imageElement.id() == DELETE_IMAGE_ID) {
			ZLResourceKey boxKey("removeBookBox");
			const std::string message =
				ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book->title());
			if (ZLDialogManager::instance().questionBox(boxKey, message,
				ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) == 0) {
				collectionModel().removeAllMarks();
				BookList().removeFileName(book->fileName());
				ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)collectionModel()[imageArea->ParagraphNumber];
				ZLTextTreeParagraph *parent = paragraph->parent();
				if (parent->children().size() == 1) {
					collectionModel().removeParagraph(imageArea->ParagraphNumber);
					collectionModel().removeParagraph(imageArea->ParagraphNumber - 1);
				} else {
					collectionModel().removeParagraph(imageArea->ParagraphNumber);
				}
				if (collectionModel().paragraphsNumber() == 0) {
					setStartCursor(0);
				}
				rebuildPaintInfo(true);
				application().refreshWindow();
			}
			return true;
		}
		return false;
	}

	int index = paragraphIndexByCoordinate(y);
	if (index == -1) {
		return false;
	}

	BookDescriptionPtr book = collectionModel().bookByParagraphNumber(index);
	if (!book.isNull()) {
		fbreader().openBook(book);
		fbreader().showBookTextView();
		return true;
	}

	return false;
}

CollectionModel &CollectionView::collectionModel() {
	return (CollectionModel&)*model();
}
