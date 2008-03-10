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

static const std::string REMOVE_BOOK_IMAGE_ID = "removeBook";
static const std::string BOOK_INFO_IMAGE_ID = "bookInfo";
static const std::string AUTHOR_INFO_IMAGE_ID = "authorInfo";
static const std::string SERIES_ORDER_IMAGE_ID = "seriesOrder";
static const std::string TAG_INFO_IMAGE_ID = "tagInfo";
static const std::string REMOVE_TAG_IMAGE_ID = "removeTag";

class CollectionModel : public ZLTextTreeModel {

public:
	CollectionModel(CollectionView &view, BookCollection &collection);
	~CollectionModel();

	BookDescriptionPtr bookByParagraphNumber(int num);
	const std::vector<int> &paragraphNumbersByBook(BookDescriptionPtr book);

	void update();

private:
	void build();
	void buildWithTags();
	void buildWithoutTags();

	void addBooks(const Books &books, ZLTextTreeParagraph *root);

	void insertText(FBTextKind kind, const std::string &text);
	void insertImage(const std::string &id);

private:
	CollectionView &myView;
	BookCollection &myCollection;

	ZLImageMap myImageMap;
	std::map<ZLTextParagraph*,BookDescriptionPtr> myParagraphToBook;
	std::map<BookDescriptionPtr,std::vector<int> > myBookToParagraph;
};

CollectionModel::CollectionModel(CollectionView &view, BookCollection &collection) : ZLTextTreeModel(), myView(view), myCollection(collection) {
	const std::string prefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	myImageMap[REMOVE_BOOK_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-remove.png", 0);
	myImageMap[BOOK_INFO_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-bookinfo.png", 0);
	myImageMap[AUTHOR_INFO_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-authorinfo.png", 0);
	myImageMap[SERIES_ORDER_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-order.png", 0);
	myImageMap[TAG_INFO_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-taginfo.png", 0);
	myImageMap[REMOVE_TAG_IMAGE_ID] = new ZLFileImage("image/png", prefix + "tree-removetag.png", 0);
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

const std::vector<int> &CollectionModel::paragraphNumbersByBook(BookDescriptionPtr book) {
	return myBookToParagraph[book];
}

void CollectionModel::build() {
	if (myView.ShowTagsOption.value()) {
		buildWithTags();
	} else {
		buildWithoutTags();
	}
}

void CollectionModel::buildWithTags() {
	const ZLResource &resource = ZLResource::resource("library");

	if (myView.ShowAllBooksTagOption.value()) {
		ZLTextTreeParagraph *allBooksParagraph = createParagraph();
		insertText(LIBRARY_AUTHOR_ENTRY, resource["allBooks"].value());
		insertImage(TAG_INFO_IMAGE_ID);
		addBooks(myCollection.books(), allBooksParagraph);
	}

	std::map<std::string,Books> tagMap;
	Books booksWithoutTags;

	const Books &books = myCollection.books();
	for (Books::const_iterator it = books.begin(); it != books.end(); ++it) {
		const std::vector<std::string> &bookTags = (*it)->tags();
		if (bookTags.empty()) {
			booksWithoutTags.push_back(*it);
		} else {
			for (std::vector<std::string>::const_iterator jt = bookTags.begin(); jt != bookTags.end(); ++jt) {
				tagMap[*jt].push_back(*it);
			}
		}
	}

	if (!booksWithoutTags.empty()) {
		ZLTextTreeParagraph *booksWithoutTagsParagraph = createParagraph();
		insertText(LIBRARY_AUTHOR_ENTRY, resource["booksWithoutTags"].value());
		insertImage(TAG_INFO_IMAGE_ID);
		addBooks(booksWithoutTags, booksWithoutTagsParagraph);
	}

	std::vector<std::string> tagStack;
	ZLTextTreeParagraph *tagParagraph = 0;
	for (std::map<std::string,Books>::const_iterator it = tagMap.begin(); it != tagMap.end(); ++it) {
		const std::string &fullTagName = it->first;
		bool useExistingTagStack = true;
		for (int index = 0, depth = 0; index != -1; ++depth) {
			int newIndex = fullTagName.find('/', index);
			const std::string subTag = fullTagName.substr(index, newIndex - index);
			index = (newIndex == -1) ? newIndex : newIndex + 1;

			if (useExistingTagStack) {
				if (tagStack.size() == (size_t)depth) {
					useExistingTagStack = false;
				} else if (tagStack[depth] != subTag) {
					for (int i = tagStack.size() - depth; i > 0; --i) {
						tagParagraph = tagParagraph->parent();
					}
					tagStack.resize(depth);
					useExistingTagStack = false;
				}
			}
			if (!useExistingTagStack) {
				tagStack.push_back(subTag);
				tagParagraph = createParagraph(tagParagraph);
				insertText(LIBRARY_AUTHOR_ENTRY, subTag);
				insertImage(TAG_INFO_IMAGE_ID);
				insertImage(REMOVE_TAG_IMAGE_ID);
			}
		}
		addBooks(it->second, tagParagraph);
	}
}

void CollectionModel::buildWithoutTags() {
	addBooks(myCollection.books(), 0);
}

void CollectionModel::addBooks(const Books &books, ZLTextTreeParagraph *root) {
	AuthorPtr author;
	AuthorComparator comparator;
	ZLTextTreeParagraph *authorParagraph = 0;
	std::string currentSequenceName;
	ZLTextTreeParagraph *sequenceParagraph = 0;

	for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
		BookDescriptionPtr description = *jt;

		if (author.isNull() || comparator(author, description->author())) {
			author = description->author();
			authorParagraph = createParagraph(root);
			insertText(LIBRARY_AUTHOR_ENTRY, author->displayName());
			//insertImage(AUTHOR_INFO_IMAGE_ID);
			currentSequenceName.erase();
			sequenceParagraph = 0;
		}

		const std::string &sequenceName = description->sequenceName();
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
		insertText(LIBRARY_BOOK_ENTRY, description->title());
		insertImage(BOOK_INFO_IMAGE_ID);
		if (myCollection.isBookExternal(description)) {
			insertImage(REMOVE_BOOK_IMAGE_ID);
		}
		myParagraphToBook[bookParagraph] = description;
		myBookToParagraph[description].push_back(paragraphsNumber() - 1);
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
	removeHighlightings();
	const std::vector<int> &toSelect = collectionModel().paragraphNumbersByBook(book);
	for (std::vector<int>::const_iterator it = toSelect.begin(); it != toSelect.end(); ++it) {
		highlightParagraph(*it);
	}
	if (!toSelect.empty()) {
		gotoParagraph(toSelect[0]);
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
		} else if (imageElement.id() == REMOVE_BOOK_IMAGE_ID) {
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
