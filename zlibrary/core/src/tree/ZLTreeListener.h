#ifndef __ZLTREELISTENER_H__
#define __ZLTREELISTENER_H__

#include "ZLTreeNode.h"

// Listener's methods should be called on changes of the tree,
// i.e. adding of new children or receiving of the node's image.
class ZLTreeListener {

public:
	class RootNode : public ZLTreeNode {

	public:
		static const ZLTypeId TYPE_ID;
 
	public:
		RootNode(ZLTreeListener &view);

	private:
		const ZLTypeId &typeId() const;
		void paint(ZLPaintContext &context, int vOffset);
		int height(ZLPaintContext &context) const;
	};

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLTreeListener(ZLPaintContext &context);
	const ZLTypeId &typeId() const;

	RootNode &rootNode();
	void clear();

public:
	ZLTreeNode *firstVisibleNode() const;
	void setFirstVisibleNode(ZLTreeNode *node);

	enum VisibilityMode {
		INVISIBLE,
		VISIBLE,
		VISIBLE_PARTIAL_AT_TOP,
		VISIBLE_PARTIAL_AT_BOTTOM
	};

	VisibilityMode visibilityMode(ZLTreeNode *node) const;

	void ensureVisible(ZLTreeNode *node);

	enum ScrollingMode {
		NONE,
		PAGE,
		ITEM,
	};
	void scroll(ScrollingMode mode, bool back);

	// TODO: new API
	// ** begin
	// Parent has new child at index :)
	virtual void onChildAdded(ZLTreeNode *parent, int index) {}
	// This is also easy to understand
	virtual void onChildRemoved(ZLTreeNode *parent, int index) {}
	// This method should be called at every node state change except of adding/removing of children
	virtual void onChildUpdated(ZLTreeNode *node) {}
	// ** end

private:
	ZLTreeNode *findNode(int &y);

private:
	RootNode myRootNode;
	ZLTreeNode *myFirstVisibleNode;
	size_t myNodePartToSkip;
	bool myCanScrollForward;
};

#endif /* __ZLTREELISTENER_H__ */
