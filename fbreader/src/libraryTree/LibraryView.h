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

#ifndef __LIBRARYVIEW_H__
#define __LIBRARYVIEW_H__

#include <map>
#include <set>

#include <ZLBlockTreeView.h>

#include "../library/Lists.h"

class Book;
class Author;
class Tag;
class TagComparator;

class AuthorNode;
class TagNode;

class LibraryView : public ZLBlockTreeView {

protected:
	LibraryView(ZLPaintContext &context);

public:
	void showBook(shared_ptr<Book>);

private:
	ZLColor backgroundColor() const;
	const std::string &caption() const;

private:
	void paint();

protected:
	virtual void makeUpToDate() = 0;

private:
	size_t myCollectionRevision;
};

class LibraryByAuthorView : public LibraryView {

public:
	LibraryByAuthorView(ZLPaintContext &context);

private:
	void makeUpToDate();

	void addAuthorSubtree(shared_ptr<Author> author, size_t atPosition);
	void fillAuthorSubtree(AuthorNode *node, const std::set<shared_ptr<Book> > &visibleBooks);
	bool isSubtreeUpToDate(AuthorNode *node);
	void updateAuthorSubtree(AuthorNode *node);
};

class LibraryByTagView : public LibraryView {

public:
	LibraryByTagView(ZLPaintContext &context);

private:
	void makeUpToDate();

	void collectTagNodes(const ZLBlockTreeNode &root, std::map<shared_ptr<Tag>,TagNode*,TagComparator> &nodeMap);
	void updateBookList(TagNode *tagNode);
};

#endif /* __LIBRARYVIEW_H__ */
