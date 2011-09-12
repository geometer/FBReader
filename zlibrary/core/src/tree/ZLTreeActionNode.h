#ifndef ZLTREEACTIONNODE_H
#define ZLTREEACTIONNODE_H

#include "ZLTreeTitledNode.h"

class ZLTreeActionNode : public ZLTreeTitledNode {
public:
	// Some action if there is no children
	virtual void activate() {}
};

#endif // ZLTREEACTIONNODE_H
