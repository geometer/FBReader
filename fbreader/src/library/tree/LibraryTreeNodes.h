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

#ifndef LIBRARYTREENODES_H
#define LIBRARYTREENODES_H

#include <ZLTreeNode.h>
#include <ZLTreePageNode.h>
#include <ZLResource.h>
#include "../../tree/FBTree.h"
#include <ZLUserData.h>

#include "LibraryNodes.h"

class ZLUserDataHolder;
class LibraryNode;
class AuthorNode;

class LibraryTree : public FBTree {

public:
    static const ZLTypeId TYPE_ID;

private:
    const ZLTypeId &typeId() const;

public:
    LibraryTree(RootTree *parent, std::size_t position);
    LibraryTree(LibraryTree *parent, std::size_t position = (std::size_t)-1);
};

class LibraryCatalogTree : public LibraryTree {

public:
    static const ZLTypeId TYPE_ID;

private:
    const ZLTypeId &typeId() const;
    shared_ptr<LibraryNode> myNode;

public:
    LibraryCatalogTree(RootTree *parent, shared_ptr<LibraryNode> node, std::size_t position = (std::size_t)-1);
    LibraryCatalogTree(LibraryTree *parent, shared_ptr<LibraryNode> node, std::size_t position = (std::size_t)-1);

    std::string title() const;
    std::string subtitle() const;
    shared_ptr<const ZLImage> image() const;

    void requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener);
    void requestMoreChildren(shared_ptr<ZLNetworkRequest::Listener> listener);
    virtual void onChildrenReceived(const BookList &childrens, const std::string &error);
    virtual void onChildrenReceived(const AuthorList &childrens, const std::string &error);
    virtual void onChildrenReceived(const TagList &childrens, const std::string &error);
    virtual void onChildrenReceived(std::vector<shared_ptr<LibraryNode> > &childrens, const std::string &error);

private:
    const ZLResource &resource() const;
    void notifyListeners(const std::string &error);

private:
    std::vector<shared_ptr<ZLNetworkRequest::Listener> > myListeners;
    mutable shared_ptr<const ZLImage> myImage;
};

class LibraryCatalogRootTree : public LibraryCatalogTree {

public:
    static const ZLTypeId TYPE_ID;

private:
    const ZLTypeId &typeId() const;

public:
    LibraryCatalogRootTree(RootTree *parent, shared_ptr<LibraryNode> node, std::size_t position);

private:
    const ZLResource &resource() const;
};

class LibraryBookTree : public ZLTreePageNode {

public:
    static const ZLTypeId TYPE_ID;

private:
    const ZLTypeId &typeId() const;

public:
    LibraryBookTree(LibraryTree *parent, shared_ptr<Book> book);

public:
    shared_ptr<ZLTreePageInfo> getPageInfo();
    const ZLResource &resource() const;
    std::string title() const;
    std::string subtitle() const;
    shared_ptr<const ZLImage> image() const;

private:
    class BookItemWrapper : public ZLTreePageInfo {

    public:
        BookItemWrapper(LibraryBookTree &tree, shared_ptr<Book> book);

        bool isPageInfoLoaded();
        void loadAll(shared_ptr<ZLNetworkRequest::Listener> listener);

    public:
        std::string title() const;
        std::vector<std::string> authors() const;
        std::vector<std::string> tags() const;
        std::string summary() const;
        shared_ptr<const ZLImage> image() const;

        //TODO maybe store actions in other place?
        const std::vector<shared_ptr<ZLTreeAction> > &actions() const;
        std::string actionText(const shared_ptr<ZLTreeAction> &action) const;
        const std::vector<shared_ptr<ZLTreeAction> > relatedActions() const;

    private:
        LibraryBookTree &myTree;
        shared_ptr<Book> myBook;
        mutable bool myIsInitialized;
        mutable std::vector<shared_ptr<ZLTreeAction> > myRelatedActions;
    };

private:
    shared_ptr<Book> myBook;
    shared_ptr<ZLTreePageInfo> myPageInfo;
};


#endif // LIBRARYTREENODES_H
