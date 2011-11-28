#include "LibraryTreeNodes.h"

#include "../library/Library.h"
#include "LibraryNodes.h"

const ZLTypeId TitleTreeNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &TitleTreeNode::typeId() const {
        return TYPE_ID;
}

TitleTreeNode::TitleTreeNode() {
        //TODO may be add lazy initialization here
        const FirstLetterList &titles = Library::Instance().firstLetterTitles();
        for (FirstLetterList::const_iterator it=titles.begin(); it!=titles.end(); ++it) {
                append(new TitleNode(*it));
        }
}

std::string TitleTreeNode::title() const {
        return ZLResource::resource("library")["byTitle"].value();
}

std::string TitleTreeNode::subtitle() const {
        return ZLResource::resource("library")["byTitle"]["summary"].value();
}

std::string TitleTreeNode::imageUrl() const {
        return FBNode::defaultImageUrl("booktree-books.png");
}
