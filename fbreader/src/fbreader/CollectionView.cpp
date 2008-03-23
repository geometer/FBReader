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

#include <ZLOptionsDialog.h>
#include <ZLStringUtil.h>

#include "CollectionView.h"
#include "CollectionModel.h"
#include "FBReader.h"
#include "BookInfoDialog.h"

#include "../collection/BookList.h"

static const std::string LIBRARY = "Library";

CollectionView::CollectionView(FBReader &reader, shared_ptr<ZLPaintContext> context) : FBView(reader, context),
	ShowTagsOption(ZLCategoryKey::LOOK_AND_FEEL, LIBRARY, "ShowTags", true),
	ShowAllBooksTagOption(ZLCategoryKey::LOOK_AND_FEEL, LIBRARY, "ShowAllBooksTag", true),
	myUpdateModel(true) {
	setModel(new CollectionModel(*this, myCollection));
	myShowTags = ShowTagsOption.value();
	myShowAllBooksList = ShowAllBooksTagOption.value();
}

CollectionView::~CollectionView() {
	setModel(0);
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
	return ZLResource::resource("library")["caption"].value();
}

void CollectionView::selectBook(BookDescriptionPtr book) {
	if (myUpdateModel) {
		shared_ptr<ZLTextModel> oldModel = model();
		setModel(0);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel);
		myUpdateModel = false;
	}
	collectionModel().removeAllMarks();
	const std::vector<int> &toSelect = collectionModel().paragraphNumbersByBook(book);
	for (std::vector<int>::const_iterator it = toSelect.begin(); it != toSelect.end(); ++it) {
		highlightParagraph(*it);
	}
	if (!toSelect.empty()) {
		gotoParagraph(toSelect[toSelect.size() - 1]);
		scrollPage(false, ZLTextView::SCROLL_PERCENTAGE, 40);
	}
}

void CollectionView::paint() {
	if ((myShowTags != ShowTagsOption.value()) ||
			(myShowAllBooksList != ShowAllBooksTagOption.value())) {
		myShowTags = ShowTagsOption.value();
		myShowAllBooksList = ShowAllBooksTagOption.value();
		myUpdateModel = true;
	}
	if (myUpdateModel) {
		shared_ptr<ZLTextModel> oldModel = model();
		setModel(0);
		((CollectionModel&)*oldModel).update();
		setModel(oldModel);
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

		const std::string &id = imageElement.id();

		if (id == CollectionModel::BookInfoImageId) {
			editBookInfo(collectionModel().bookByParagraphNumber(imageArea->ParagraphNumber));
			return true;
		} else if (id == CollectionModel::RemoveBookImageId) {
			removeBook(collectionModel().bookByParagraphNumber(imageArea->ParagraphNumber));
			return true;
		} else if (id == CollectionModel::RemoveTagImageId) {
			removeTag(collectionModel().tagByParagraphNumber(imageArea->ParagraphNumber));
			return true;
		} else if (id == CollectionModel::TagInfoImageId) {
			editTagInfo(collectionModel().tagByParagraphNumber(imageArea->ParagraphNumber));
			return true;
		} else {
			return false;
		}
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

void CollectionView::editBookInfo(BookDescriptionPtr book) {
	if (!book.isNull() && BookInfoDialog(myCollection, book->fileName()).dialog().run()) {
		myCollection.rebuild(false);
		myUpdateModel = true;
		selectBook(book);
		application().refreshWindow();
	}
}

void CollectionView::removeBook(BookDescriptionPtr book) {
	if (book.isNull()) {
		return;
	}

	CollectionModel &cModel = collectionModel();

	ZLResourceKey boxKey("removeBookBox");
	const std::string message =
		ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book->title());
	if (ZLDialogManager::instance().questionBox(boxKey, message,
		ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) == 0) {
		cModel.removeAllMarks();
		BookList().removeFileName(book->fileName());
		
		cModel.removeBook(book);
		if (cModel.paragraphsNumber() == 0) {
			setStartCursor(0);
		} else {
			size_t index = startCursor().paragraphCursor().index();
			if (index >= cModel.paragraphsNumber()) {
				index = cModel.paragraphsNumber() - 1;
			}
			while (!((ZLTextTreeParagraph*)cModel[index])->parent()->isOpen()) {
				--index;
			}
			gotoParagraph(index);
		}
		rebuildPaintInfo(true);
		application().refreshWindow();
	}
}

void CollectionView::removeTag(const std::string &tag) {
	if (tag.empty()) {
		return;
	}

	ZLResourceKey boxKey("removeTagBox");
	const std::string message =
		ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), tag);
	enum { REMOVE_TAG, REMOVE_SUBTREE, DONT_REMOVE } code = DONT_REMOVE;
	if (myCollection.hasSubtags(tag)) {
		if (myCollection.hasBooks(tag)) {
			switch (ZLDialogManager::instance().questionBox(boxKey, message,
								ZLResourceKey("thisOnly"),
								ZLResourceKey("withSubtags"),
								ZLDialogManager::CANCEL_BUTTON
							)) {
				case 0:
					code = REMOVE_TAG;
					break;
				case 1:
					code = REMOVE_SUBTREE;
					break;
			}
		} else {
			if (ZLDialogManager::instance().questionBox(boxKey, message,
				ZLResourceKey("withSubtags"), ZLDialogManager::CANCEL_BUTTON) == 0) {
				code = REMOVE_SUBTREE;
			}
		}
	} else {
		if (ZLDialogManager::instance().questionBox(boxKey, message,
			ZLDialogManager::YES_BUTTON, ZLDialogManager::CANCEL_BUTTON) == 0) {
			code = REMOVE_TAG;
		}
	}
	if (code != DONT_REMOVE) {
		collectionModel().removeAllMarks();
		myCollection.removeTag(tag, code == REMOVE_SUBTREE);
		updateModel();
		application().refreshWindow();
	}
}

CollectionModel &CollectionView::collectionModel() {
	return (CollectionModel&)*model();
}
