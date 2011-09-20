#ifndef __ZLTREEDIALOG_H__
#define __ZLTREEDIALOG_H__

#include <ZLTreeListener.h>

// This object should be pure successor of ZLTreeListener
// or be merged with it.
class ZLTreeDialog : public ZLTreeListener {

protected:
	ZLTreeDialog();

public:
	virtual ~ZLTreeDialog();

	//TODO maybe run() should return an integer?
	virtual void run() = 0;

	// Parent has new child at index
	virtual void onNodeBeginInsert(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndInsert() = 0;
	// This is also easy to understand
	virtual void onNodeBeginRemove(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndRemove() = 0;
	// This method should be called at every node state change except of adding/removing of children
	virtual void onNodeUpdated(ZLTreeNode *node) = 0;

protected:
};

#endif /* __ZLTREEDIALOG_H__ */
