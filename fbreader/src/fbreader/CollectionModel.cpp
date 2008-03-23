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

#include "CollectionView.h"
#include "CollectionModel.h"

const std::string CollectionModel::RemoveBookImageId = "removeBook";
const std::string CollectionModel::BookInfoImageId = "bookInfo";
const std::string CollectionModel::AuthorInfoImageId = "authorInfo";
const std::string CollectionModel::SeriesOrderImageId = "seriesOrder";
const std::string CollectionModel::TagInfoImageId = "tagInfo";
const std::string CollectionModel::RemoveTagImageId = "removeTag";

CollectionModel::CollectionModel(CollectionView &view, BookCollection &collection) : ZLTextTreeModel(), myView(view), myCollection(collection) {
	const std::string prefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	myImageMap[RemoveBookImageId] = new ZLFileImage("image/png", prefix + "tree-remove.png", 0);
	myImageMap[BookInfoImageId] = new ZLFileImage("image/png", prefix + "tree-bookinfo.png", 0);
	myImageMap[AuthorInfoImageId] = new ZLFileImage("image/png", prefix + "tree-authorinfo.png", 0);
	myImageMap[SeriesOrderImageId] = new ZLFileImage("image/png", prefix + "tree-order.png", 0);
	myImageMap[TagInfoImageId] = new ZLFileImage("image/png", prefix + "tree-taginfo.png", 0);
	myImageMap[RemoveTagImageId] = new ZLFileImage("image/png", prefix + "tree-removetag.png", 0);
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

const std::string &CollectionModel::tagByParagraphNumber(int num) {
	static const std::string EMPTY;
	std::map<ZLTextParagraph*,std::string>::iterator it = myParagraphToTag.find((*this)[num]);
	return (it != myParagraphToTag.end()) ? it->second : EMPTY;
}

const std::vector<int> &CollectionModel::paragraphNumbersByBook(BookDescriptionPtr book) {
	return myBookToParagraph[book];
}

void CollectionModel::build() {
	if (myCollection.books().empty()) {
		createParagraph();
		insertText(LIBRARY_AUTHOR_ENTRY, ZLResource::resource("library")["noBooks"].value());
	} else {
		if (myView.ShowTagsOption.value()) {
			buildWithTags();
		} else {
			buildWithoutTags();
		}
	}
}

void CollectionModel::buildWithTags() {
	const ZLResource &resource = ZLResource::resource("library");

	if (myView.ShowAllBooksTagOption.value()) {
		ZLTextTreeParagraph *allBooksParagraph = createParagraph();
		insertText(LIBRARY_AUTHOR_ENTRY, resource["allBooks"].value());
		insertImage(TagInfoImageId);
		myParagraphToTag[allBooksParagraph] = CollectionView::SpecialTagAllBooks;
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
		insertImage(TagInfoImageId);
		myParagraphToTag[booksWithoutTagsParagraph] = CollectionView::SpecialTagNoTagsBooks;
		addBooks(booksWithoutTags, booksWithoutTagsParagraph);
	}

	std::vector<std::string> tagStack;
	ZLTextTreeParagraph *tagParagraph = 0;
	std::map<ZLTextTreeParagraph*,std::string> paragraphToTagMap;
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
						std::map<ZLTextTreeParagraph*,std::string>::iterator jt =
							paragraphToTagMap.find(tagParagraph);
						if (jt != paragraphToTagMap.end()) {
							addBooks(tagMap[jt->second], tagParagraph);
						}
						tagParagraph = tagParagraph->parent();
					}
					tagStack.resize(depth);
					useExistingTagStack = false;
				}
			}
			if (!useExistingTagStack) {
				tagStack.push_back(subTag);
				tagParagraph = createParagraph(tagParagraph);
				myParagraphToTag[tagParagraph] = fullTagName.substr(0, newIndex);
				insertText(LIBRARY_AUTHOR_ENTRY, subTag);
				insertImage(TagInfoImageId);
				insertImage(RemoveTagImageId);
			}
		}
		paragraphToTagMap[tagParagraph] = fullTagName;
	}
	while (tagParagraph != 0) {
		std::map<ZLTextTreeParagraph*,std::string>::iterator jt = paragraphToTagMap.find(tagParagraph);
		if (jt != paragraphToTagMap.end()) {
			addBooks(tagMap[jt->second], tagParagraph);
		}
		tagParagraph = tagParagraph->parent();
	}
}

void CollectionModel::buildWithoutTags() {
	addBooks(myCollection.books(), 0);
}

void CollectionModel::addBooks(const Books &books, ZLTextTreeParagraph *root) {
	AuthorPtr author;
	AuthorComparator comparator;
	ZLTextTreeParagraph *authorParagraph = 0;
	std::string currentSeriesName;
	ZLTextTreeParagraph *seriesParagraph = 0;

	for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
		BookDescriptionPtr description = *jt;

		if (author.isNull() || comparator(author, description->author())) {
			author = description->author();
			authorParagraph = createParagraph(root);
			insertText(LIBRARY_AUTHOR_ENTRY, author->displayName());
			//insertImage(AuthorInfoImageId);
			currentSeriesName.erase();
			seriesParagraph = 0;
		}

		const std::string &seriesName = description->seriesName();
		if (seriesName.empty()) {
			currentSeriesName.erase();
			seriesParagraph = 0;
		} else if (seriesName != currentSeriesName) {
			currentSeriesName = seriesName;
			seriesParagraph = createParagraph(authorParagraph);
			insertText(LIBRARY_BOOK_ENTRY, seriesName);
			//insertImage(SeriesOrderImageId);
		}
		ZLTextTreeParagraph *bookParagraph = createParagraph(
			(seriesParagraph == 0) ? authorParagraph : seriesParagraph
		);
		insertText(LIBRARY_BOOK_ENTRY, description->title());
		insertImage(BookInfoImageId);
		if (myCollection.isBookExternal(description)) {
			insertImage(RemoveBookImageId);
		}
		myParagraphToBook[bookParagraph] = description;
		myBookToParagraph[description].push_back(paragraphsNumber() - 1);
	}
}

void CollectionModel::update() {
	myParagraphToBook.clear();
	myBookToParagraph.clear();
	myParagraphToTag.clear();
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

void CollectionModel::removeBook(BookDescriptionPtr book) {
	std::map<BookDescriptionPtr,std::vector<int> >::iterator it = myBookToParagraph.find(book);
	if (it == myBookToParagraph.end()) {
		return;
	}
	const std::vector<int> paragraphIndices = it->second;
	myBookToParagraph.erase(it);
	for (int i = paragraphIndices.size() - 1; i >= 0; --i) {
		int index = paragraphIndices[i];
		ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)(*this)[index];
		int count = 1;
		for (ZLTextTreeParagraph *parent = paragraph->parent(); (parent != 0) && (parent->children().size() == 1); parent = parent->parent()) {
			++count;
		}

		if (count > index) {
			count = index;
		}

		for (std::map<BookDescriptionPtr,std::vector<int> >::iterator jt = myBookToParagraph.begin(); jt != myBookToParagraph.end(); ++jt) {
			std::vector<int> &indices = jt->second;
			for (std::vector<int>::iterator kt = indices.begin(); kt != indices.end(); ++kt) {
				if (*kt > index) {
					*kt -= count;
				}
			}
		}

		for (; count > 0; --count) {
			removeParagraph(index--);
		}
	}
}
