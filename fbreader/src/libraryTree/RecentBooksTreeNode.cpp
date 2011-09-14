#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId RecentBooksTreeNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &RecentBooksTreeNode::typeId() const {
	return TYPE_ID;
}

RecentBooksTreeNode::RecentBooksTreeNode() {
	const BookList &books = Library::Instance().recentBooks();
	size_t index = 0;
	for (BookList::const_iterator it=books.begin(); it!=books.end(); ++it) {
		insert(new BookNode(*it), index++);
	}
}

std::string RecentBooksTreeNode::title() const {
	//TODO add resource for this node
	return ZLResource::resource("menu")["showRecent"].value();
}

std::string RecentBooksTreeNode::subtitle() const {
	//TODO add another subtitle here
	return ZLResource::resource("menu")["showRecent"].value();
}
