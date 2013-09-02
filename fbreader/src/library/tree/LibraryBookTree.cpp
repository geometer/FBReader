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

#include "LibraryTreeNodes.h"
#include <ZLFileImage.h>
#include <ZLibrary.h>
#include "../../formats/FormatPlugin.h"
#include "../../libraryActions/LibraryBookActions.h"

const ZLTypeId LibraryBookTree::TYPE_ID(ZLTreePageNode::TYPE_ID);

const ZLTypeId &LibraryBookTree::typeId() const {
    return TYPE_ID;
}

LibraryBookTree::LibraryBookTree(LibraryTree *parent, shared_ptr<Book> book) : ZLTreePageNode(parent), myBook(book){
    init();
}

static std::vector<shared_ptr<ZLTreeAction> > getBookActions(LibraryBookTree &tree) {
    std::vector<shared_ptr<ZLTreeAction> > actions;
    const shared_ptr<Book> book = tree.book ();
    if(!book.isNull ()){
        actions.push_back(new BookReadAction(book));
        //actions.push_back(new BookRemoveAction(book));
    }

    /*const NetworkBookItem &book = tree.book();
    if (!book.reference(BookReference::DOWNLOAD_FULL).isNull() ||
            !book.reference(BookReference::DOWNLOAD_FULL_CONDITIONAL).isNull()) {
        actions.push_back(new NetworkTreeBookReadAction(tree, book, false));
        actions.push_back(new NetworkBookDownloadAction(tree, book, false));
        actions.push_back(new NetworkBookDeleteAction(book));
    }
    if (!book.reference(BookReference::DOWNLOAD_DEMO).isNull()) {
        actions.push_back(new NetworkTreeBookReadAction(tree, book, true));
        actions.push_back(new NetworkBookDownloadAction(tree, book, true, tree.resource()["demo"].value()));
    }
    if (!book.reference(BookReference::BUY).isNull()) {
        actions.push_back(new NetworkBookBuyDirectlyAction(tree, book));
    } else if (!book.reference(BookReference::BUY_IN_BROWSER).isNull()) {
        actions.push_back(new NetworkBookBuyInBrowserAction(book));
    }*/
    return actions;
}

void LibraryBookTree::init() {
    std::vector<shared_ptr<ZLTreeAction> > actions = getBookActions(*this);
    for (std::size_t i = 0; i < actions.size(); ++i) {
        registerAction(actions.at(i));
    }
}

shared_ptr<ZLTreePageInfo> LibraryBookTree::getPageInfo(){
    if (myPageInfo.isNull()) {
        myPageInfo = new BookItemWrapper(*this, myBook);
    }
    return myPageInfo;
}

const ZLResource &LibraryBookTree::resource() const{
    return ZLResource::resource("networkView")["bookNode"];
}
std::string LibraryBookTree::title() const{
    return myBook->title();
}
std::string LibraryBookTree::subtitle() const{
    return myBook->seriesTitle();
}

shared_ptr<Book> LibraryBookTree::book() const{
    return myBook;
}

shared_ptr<const ZLImage> LibraryBookTree::image() const {
    shared_ptr<FormatPlugin> plugin = PluginCollection::Instance().plugin(*myBook);
    if (!plugin.isNull()) {
        shared_ptr<const ZLImage> cover = plugin->coverImage(myBook->file());
        if (!cover.isNull()) {
            return cover;
        }
    }
    return FBTree::defaultCoverImage("booktree-book.png");
}

//-------------------------------

LibraryBookTree::BookItemWrapper::BookItemWrapper(LibraryBookTree &tree, shared_ptr<Book> book) :
    myTree(tree), myBook(book), myIsInitialized(false) {
}

bool LibraryBookTree::BookItemWrapper::isPageInfoLoaded() {
    return myIsInitialized;
}

class BookItemWrapperScope : public ZLUserData {
public:
    shared_ptr<ZLNetworkRequest::Listener> listener;
};

void LibraryBookTree::BookItemWrapper::loadAll(shared_ptr<ZLNetworkRequest::Listener> listener) {
   myIsInitialized = true;
   listener->finished("");
}

std::string LibraryBookTree::BookItemWrapper::title() const {
    return myBook->title();
}

std::vector<std::string> LibraryBookTree::BookItemWrapper::authors() const {
    std::vector<std::string> authors;
    const AuthorList &authorsList = myBook->authors();
    if (!authorsList.empty()) {
        for (AuthorList::const_iterator it = authorsList.begin(); it != authorsList.end(); ++it) {
            authors.push_back((*it)->name());
        }
    }
    return authors;
}

std::vector<std::string> LibraryBookTree::BookItemWrapper::tags() const {
    std::vector<std::string> tags;
    return tags;
}

std::string LibraryBookTree::BookItemWrapper::summary() const {
    return "";
}

shared_ptr<const ZLImage> LibraryBookTree::BookItemWrapper::image() const {
    return myTree.image();
}

const std::vector<shared_ptr<ZLTreeAction> > &LibraryBookTree::BookItemWrapper::actions() const {
    return myTree.actions();
}

std::string LibraryBookTree::BookItemWrapper::actionText(const shared_ptr<ZLTreeAction> &action) const {
    return myTree.actionText(action);
}

//class RelatedAction : public ZLTreeAction {
//public:
//    RelatedAction(shared_ptr<Book> item) : myTitle(item->title()) {
        //myNode = new LibraryCatalogTree(&NetworkLibrary::Instance().getFakeCatalogTree(), item);
        //myNode = NetworkTreeFactory::createNetworkTree(0, item);
//    }
//    ZLResourceKey key() const { return ZLResourceKey(""); }
//    std::string text(const ZLResource &/*resource*/) const {
//        return myTitle;
//    }
//    void run() {
        //if (NetworkCatalogTree *tree = zlobject_cast<NetworkCatalogTree*>(&*myNode)){
        //    tree->expand();
        //}
//    }

//private:
//    shared_ptr<ZLTreeTitledNode> myNode;
//    std::string myTitle;
//};

const std::vector<shared_ptr<ZLTreeAction> > LibraryBookTree::BookItemWrapper::relatedActions() const {
    if (!myRelatedActions.empty()) {
        return myRelatedActions;
    }
    /*std::vector<shared_ptr<NetworkItem> > catalogItems;// =  static_cast<NetworkBookItem&>(*myBookItem).getRelatedCatalogsItems();
    for (std::size_t i = 0; i < catalogItems.size(); ++i) {
        shared_ptr<Book> item = catalogItems.at(i);
        myRelatedActions.push_back(new RelatedAction(item));
    }*/
    return myRelatedActions;
}
