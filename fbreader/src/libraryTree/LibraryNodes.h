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

#include "../tree/FBNode.h"

#include "../library/Author.h"
#include "../library/Tag.h"
#include "../library/Book.h"

#include <ZLTreeActionNode.h>
#include <ZLTreePageNode.h>

class ZLImage;

class Author;
class Book;
class Tag;

class AuthorNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

//public:
//	AuthorNode(ZLBlockTreeView::RootNode *parent, size_t atPosition, shared_ptr<Author> author);
//	void init();

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;

public:
	AuthorNode(shared_ptr<Author> author);

public:
	std::string title() const;
	std::string subtitle() const;
	std::string imageUrl() const;

protected:
//	shared_ptr<ZLImage> image() const;

private:
	shared_ptr<Author> myAuthor;
};


class SeriesNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	SeriesNode(AuthorNode *parent);
	//void init();

	//shared_ptr<Book> book() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string imageUrl() const;
};

class TagNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

//private:
//	static size_t positionToInsert(ZLBlockTreeNode *parent, shared_ptr<Tag> tag);

public:
	TagNode(shared_ptr<Tag> tag);
	//TagNode(ZLBlockTreeView::RootNode *parent, shared_ptr<Tag> tag);
	//TagNode(TagNode *parent, shared_ptr<Tag> tag);
	//void init();

	//shared_ptr<Tag> tag() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string imageUrl() const;
	std::string title() const;
	std::string subtitle() const;

private:
	const shared_ptr<Tag> myTag;
};

class TitleNode : public FBNode {

public:
        static const ZLTypeId TYPE_ID;

public:
        TitleNode(std::string firstLetter);

private:
        const ZLResource &resource() const;
        const ZLTypeId &typeId() const;
        std::string imageUrl() const;
        std::string title() const;
        std::string subtitle() const;

private:
        const std::string myFirstLetter;
};

class BookNode : public ZLTreePageNode  {

public:
	static const ZLTypeId TYPE_ID;

public:
        enum SubtitleMode {
            SHOW_AUTHORS,
            SHOW_TAGS
        };

public:
        BookNode(shared_ptr<Book> book, SubtitleMode subtitleMode = SHOW_AUTHORS);

public:
	std::string title() const;
	std::string subtitle() const;
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	void fillContent(ZLDialogContent &content) const;
	ZLResourceKey contentKey() const;
	shared_ptr<Book> book() const;

private:
	shared_ptr<ZLImage> originalImage() const;
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;

private:
	const shared_ptr<Book> myBook;
	const std::string mySubtitle;
	mutable bool myCoverImageIsStored;
	mutable shared_ptr<ZLImage> myStoredCoverImage;
};

class AuthorFunctor {
public:
    std::string operator()(shared_ptr<Author> author) const;
};

class TagFunctor {
public:
    std::string operator()(shared_ptr<Tag> tag) const;
};

class BookFunctor {
public:
    std::string operator()(shared_ptr<Book> book) const;
};

inline std::string AuthorFunctor::operator()(shared_ptr<Author> author) const {
    return author.isNull() ? std::string() : author->name();
}

inline std::string TagFunctor::operator()(shared_ptr<Tag> tag) const {
    return tag.isNull() ? std::string() : tag->name();
}

inline std::string BookFunctor::operator()(shared_ptr<Book> book) const {
    return book.isNull() ? std::string() : book->title();
}

#endif /* __LIBRARYNODES_H__ */
