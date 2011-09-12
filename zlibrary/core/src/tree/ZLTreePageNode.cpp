#include "ZLTreePageNode.h"

const ZLTypeId ZLTreePageNode::TYPE_ID(ZLTreeNode::TYPE_ID);

const ZLTypeId &ZLTreePageNode::typeId() const {
	return TYPE_ID;
}
