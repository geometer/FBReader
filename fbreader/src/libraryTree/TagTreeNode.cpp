#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId TagTreeNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &TagTreeNode::typeId() const {
        return TYPE_ID;
}

TagTreeNode::TagTreeNode() {
        //TODO may be add lazy initialization here
        const TagList &tags = Library::Instance().tags();
        size_t index = 0;
        for (TagList::const_iterator it=tags.begin(); it!=tags.end(); ++it) {
                insert(new TagNode(*it), index++);
        }
}

std::string TagTreeNode::title() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["byTag"]["label"].value();
}

std::string TagTreeNode::subtitle() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["byTag"]["tooltip"].value();
}

std::string TagTreeNode::imageUrl() const {
	return FBNode::defaultImageUrl("booktree-tags.png");
}
