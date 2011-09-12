#ifndef ZLTREEPAGENODE_H
#define ZLTREEPAGENODE_H

#include "ZLTreeNode.h"

class ZLTreePageNode : public ZLTreeNode {

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;
};

#endif // ZLTREEPAGENODE_H
