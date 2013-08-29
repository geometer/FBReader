/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef LIBRARYNODES_H
#define LIBRARYNODES_H

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

#include "../Library.h"
#include "../Book.h"
#include "../Author.h"
#include "../Tag.h"
#include "../Lists.h"
#include "LibraryTreeNodes.h"

class LibraryCatalogTree;

class LibraryNode{
public:

    typedef std::vector<shared_ptr<LibraryNode> > List;

    LibraryNode();
    virtual void getChildren(LibraryCatalogTree *tree) = 0;
    virtual std::string getTitle();
    virtual std::string getSubTitle();
    virtual std::string getImageName();
};

class FavoritesBooksNode : public LibraryNode{
public:
    FavoritesBooksNode();
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
};

class RecentBooksNode : public LibraryNode{
public:
    RecentBooksNode();
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
};

class AuthorsCatalogNode : public LibraryNode{
public:
    AuthorsCatalogNode();
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
    std::string getImageName();
};

class BooksByAuthorNode : public LibraryNode{
public:
    BooksByAuthorNode(shared_ptr<Author> author);
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
    std::string getImageName();
private:
    shared_ptr<Author> myAuthor;
};

class TagsCatalogNode : public LibraryNode{
public:
    TagsCatalogNode();
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
    std::string getImageName();
};

class BooksByTagNode : public LibraryNode{
public:
    BooksByTagNode(shared_ptr<Tag> tag);
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
    std::string getImageName();
private:
    shared_ptr<Tag> myTag;
};

class BooksByTitleNode : public LibraryNode{
public:
    BooksByTitleNode(std::string title = "");
    void getChildren(LibraryCatalogTree *tree);
    std::string getTitle();
    std::string getSubTitle();
private:
    std::string myTitle;
};


#endif // LIBRARYNODES_H
