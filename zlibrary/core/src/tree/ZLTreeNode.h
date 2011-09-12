#ifndef __ZLTREENODE_H__
#define __ZLTREENODE_H__

#include <map>
#include <vector>

#include <shared_ptr.h>

#include <ZLTypeId.h>
#include <ZLRunnable.h>
#include <ZLPaintContext.h>

// Nowadays I see hierarchy in this way:
//
// ZLTreeNode
// |
// +-ZLTreeTitledNode -- simple node, as shown below
// | |
// | +- ZLTreeActionNode -- node with some action on click, i.e. BookNode at local library
// |
// +-ZLTreePageNode -- usually the leaf of the tree, smth like page with ZLDialogContent
//                     and some actions, i.e. BookInfo page at network library

class ZLTreeListener;

class ZLTreeNode : public ZLObjectWithRTTI {

public:
	typedef std::vector<ZLTreeNode*> List;

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLTreeNode(ZLTreeListener &view);
	ZLTreeNode(ZLTreeNode *parent, size_t atPosition = -1);

public:
	virtual ~ZLTreeNode();
	void clear();

public:
	ZLTreeNode *parent() const;
	ZLTreeNode *previous() const;
	ZLTreeNode *next() const;
	const List &children() const;

	void open(bool openNotClose);
	bool isOpen() const;

protected:
	size_t level() const;

	ZLTreeListener &view();

private:
	// I don't see any reasons to store myListener in every node, 
	// maximum level is not so big to suffer from getting it from top parent
	ZLTreeListener &myListener;
	ZLTreeNode *myParent;
	size_t myChildIndex;

	List myChildren;
	// Should this be removed?
	bool myIsOpen;

private:
	ZLTreeNode(const ZLTreeNode&);
	const ZLTreeNode &operator = (const ZLTreeNode&);
};

#endif /* __ZLTREENODE_H__ */
