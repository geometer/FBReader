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

#ifndef __COLLECTIONMODEL_H__
#define __COLLECTIONMODEL_H__

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "../bookmodel/FBTextKind.h"
#include "../collection/BookCollection.h"
#include "../description/BookDescription.h"

class CollectionView;

class CollectionModel : public ZLTextTreeModel {

public:
	static const std::string RemoveBookImageId;
	static const std::string BookInfoImageId;
	static const std::string AuthorInfoImageId;
	static const std::string SeriesOrderImageId;
	static const std::string TagInfoImageId;
	static const std::string RemoveTagImageId;

public:
	CollectionModel(CollectionView &view, BookCollection &collection);
	~CollectionModel();

	BookDescriptionPtr bookByParagraphNumber(int num);
	const std::vector<int> &paragraphNumbersByBook(BookDescriptionPtr book);
	const std::string &tagByParagraphNumber(int num);

	void update();

	void removeBook(BookDescriptionPtr book);

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
	std::map<ZLTextParagraph*,std::string> myParagraphToTag;
	std::map<BookDescriptionPtr,std::vector<int> > myBookToParagraph;
};

#endif /* __COLLECTIONMODEL_H__ */
