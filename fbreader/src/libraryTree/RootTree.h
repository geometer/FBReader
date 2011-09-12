#ifndef __ROOTTREE_H__
#define __ROOTTREE_H__

#include <ZLTreeNode.h>
#include <ZLTreeTitledNode.h>

class RootTree : ZLTreeNode {

public:
	const ZLTypeId &typeId() const;

public:
    RootTree();

};

class AuthorTreeNode : ZLTreeTitledNode {
public:
	const ZLTypeId &typeId() const;
public:
	AuthorTreeNode(ZLTreeNode* parent);
};

class TagTreeNode : ZLTreeTitledNode {
public:
	const ZLTypeId &typeId() const;
public:
	TagTreeNode(ZLTreeNode* parent);
};

#endif /* __ROOTTREE_H__ */
