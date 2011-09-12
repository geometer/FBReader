#include "RootTree.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId RootTree::TYPE_ID(ZLTreeNode::TYPE_ID);
const ZLTypeId AuthorTreeNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);
const ZLTypeId TagTreeNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

RootTree::RootTree() {
	myChildren.push_back(new AuthorTreeNode(this));
	myChildren.push_back(new TagTreeNode(this));
}

const ZLTypeId &RootTree::typeId() const {
	return TYPE_ID;
}

const ZLTypeId &AuthorTreeNode::typeId() const {
	return TYPE_ID;
}

const ZLTypeId &TagTreeNode::typeId() const {
	return TYPE_ID;
}


AuthorTreeNode::AuthorTreeNode(ZLTreeNode* parent) : ZLTreeTitledNode(parent) {
	const AuthorList &authors = Library::Instance().authors();
	for (AuthorList::const_iterator it=authors.begin(); it!=authors.end(); ++it) {
		myChildren.push_back(new AuthorNode(this, *it));
	}
}

TagTreeNode::TagTreeNode(ZLTreeNode* parent) : ZLTreeTitledNode(parent) {

}
