/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LIBRARYNODES_H__
#define __LIBRARYNODES_H__

#include "../blockTree/FBReaderNode.h"

class ZLImage;

class Author;
class Book;
class Tag;

class AuthorNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	AuthorNode(ZLBlockTreeView::RootNode *parent, size_t atPosition, shared_ptr<Author> author);
	void init();

	shared_ptr<Author> author() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;

private:
	shared_ptr<Author> myAuthor;
};

class SeriesNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	SeriesNode(AuthorNode *parent);
	void init();

	shared_ptr<Book> book() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
};

class TagNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	static size_t positionToInsert(ZLBlockTreeNode *parent, shared_ptr<Tag> tag);

public:
	TagNode(ZLBlockTreeView::RootNode *parent, shared_ptr<Tag> tag);
	TagNode(TagNode *parent, shared_ptr<Tag> tag);
	void init();

	shared_ptr<Tag> tag() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;

private:
	const shared_ptr<Tag> myTag;
};

class BookNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	BookNode(AuthorNode *parent, shared_ptr<Book> book);
	BookNode(SeriesNode *parent, shared_ptr<Book> book);
	BookNode(TagNode *parent, size_t atPosition, shared_ptr<Book> book);

	shared_ptr<Book> book() const;

private:
	void init();
	bool highlighted() const;
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string summary() const;

private:
	const shared_ptr<Book> myBook;
};

#endif /* __LIBRARYNODES_H__ */
