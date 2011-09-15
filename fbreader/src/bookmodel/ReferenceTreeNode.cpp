#include "../fbreader/FBReader.h"
#include "../fbreader/BookTextView.h"

#include "TOCTreeNodes.h"

const ZLTypeId ReferenceTreeNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

const ZLTypeId &ReferenceTreeNode::typeId() const {
	return TYPE_ID;
}

ReferenceTreeNode::ReferenceTreeNode(const ZLTextTreeParagraph& paragraph): myParagraph(paragraph) {
	createChildren();
}

void ReferenceTreeNode::createChildren() {
	const std::vector<ZLTextTreeParagraph*>& children = myParagraph.children();
	for (size_t index=0; index < children.size(); ++index) {
		ZLTextTreeParagraph* paragraph = children.at(index);
		if (paragraph->children().size() == 0) {
			insert(new ReferenceNode(*paragraph), index);
		} else {
			insert(new ReferenceTreeNode(*paragraph), index);
		}
	}
}

std::string ReferenceTreeNode::title() const {
	return myParagraph.text();
}
