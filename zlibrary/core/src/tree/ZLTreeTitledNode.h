#ifndef __ZLTREETITLEDNODE_H__
#define __ZLTREETITLEDNODE_H__

#include "ZLTreeNode.h"

class ZLTreeTitledNode : public ZLTreeNode {

public:
	static const ZLTypeId TYPE_ID;

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
	virtual List &children() const {}
};

#endif /* __ZLTREETITLEDNODE_H__ */
