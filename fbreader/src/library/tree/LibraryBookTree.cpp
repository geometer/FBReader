#include "LibraryTreeNodes.h"
#include <ZLFileImage.h>
#include <ZLibrary.h>
#include "../../formats/FormatPlugin.h"

const ZLTypeId LibraryBookTree::TYPE_ID(ZLTreePageNode::TYPE_ID);

const ZLTypeId &LibraryBookTree::typeId() const {
    return TYPE_ID;
}

LibraryBookTree::LibraryBookTree(LibraryTree *parent, shared_ptr<Book> book) : ZLTreePageNode(parent), myBook(book){
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
    //return myTree.actionText(action);
    return "actionText";
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
