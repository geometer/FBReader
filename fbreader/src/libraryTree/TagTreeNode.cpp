#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId TagTreeNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &TagTreeNode::typeId() const {
	return TYPE_ID;
}

TagTreeNode::TagTreeNode() {
	const TagList &tags = Library::Instance().tags();
	size_t index = 0;
	for (TagList::const_iterator it=tags.begin(); it!=tags.end(); ++it) {
		insert(new TagNode(*it), index++);
	}
}

std::string TagTreeNode::title() const {
	//TODO add resource for this node
	return ZLResource::resource("toolbar")["byTag"]["label"].value();
}

std::string TagTreeNode::subtitle() const {
	//TODO add another subtitle here
	return ZLResource::resource("toolbar")["byTag"]["label"].value();
}
