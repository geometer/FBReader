#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId RecentBooksTreeNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &RecentBooksTreeNode::typeId() const {
	return TYPE_ID;
}

RecentBooksTreeNode::RecentBooksTreeNode() {
        //TODO may be add lazy initialization here
	const BookList &books = Library::Instance().recentBooks();
	size_t index = 0;
	for (BookList::const_iterator it=books.begin(); it!=books.end(); ++it) {
                insert(new BookNode(*it), index++);
	}
}

std::string RecentBooksTreeNode::title() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["showRecent"]["label"].value();
}

std::string RecentBooksTreeNode::subtitle() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["showRecent"]["tooltip"].value();
}
