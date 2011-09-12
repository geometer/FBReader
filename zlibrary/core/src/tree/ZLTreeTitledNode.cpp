#include "ZLTreeTitledNode.h"

const ZLTypeId ZLTreeTitledNode::TYPE_ID(ZLTreeNode::TYPE_ID);

const ZLTypeId &ZLTreeTitledNode::typeId() const {
	return TYPE_ID;
}

