#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId LibraryTreeNode::TYPE_ID(ZLTreeListener::RootNode::TYPE_ID);

const ZLTypeId &LibraryTreeNode::typeId() const {
	return TYPE_ID;
}

LibraryTreeNode::LibraryTreeNode(ZLTreeListener &listener) : RootNode(listener) {
	size_t index = 0;
	insert(new AuthorTreeNode, index++);
	insert(new TagTreeNode, index++);
	insert(new RecentBooksTreeNode, index++);
}

