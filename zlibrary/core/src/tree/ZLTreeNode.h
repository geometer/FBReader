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

public:
	// TODO: new API, there are listed methods of ZLTreeTitledNode
	// ** begin
	// Usually node looks like
	// +-------+---------------+
	// |       |  TITLE        |
	// | image |             > |
	// |       |  subtitle     |
	// +-------+---------------+
	// Name of book
	virtual std::string title() const { return std::string(); }
	// Short description
	virtual std::string subtitle() const { return std::string(); }
	// Url to image
	virtual std::string imageUrl() const { return std::string(); }
	// Or the image if there is no url
	virtual shared_ptr<ZLImage> image() const { return shared_ptr<ZLImage>(); }
	// Children should be requested from network only if we need them
	virtual void requestChildren() {}
	// Some action if there is no children, I suppose it should be at ZLTreeActionNode
	virtual void activate() {}
	// **end

private:
	// I don't see any reasons to store myListener in every node, 
	// maximum level is not so big to suffer from getting it from top parent
	ZLTreeListener &myListener;
	ZLTreeNode *myParent;
	size_t myChildIndex;

	List myChildren;
	// Should this be removed?
	bool myIsOpen;

	typedef std::map<Rectangle,shared_ptr<ZLRunnableWithKey> > LinkMap;
	LinkMap myHyperlinks;

private:
	ZLTreeNode(const ZLTreeNode&);
	const ZLTreeNode &operator = (const ZLTreeNode&);
};

//class ZLTreeActionNode : public ZLTreeNode {
//	static const ZLTypeId TYPE_ID;
//public:
//	// Some action if there is no children
//	virtual void activate() const {}
//};

#endif /* __ZLTREENODE_H__ */
