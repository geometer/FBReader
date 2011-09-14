#ifndef __ROOTTREE_H__
#define __ROOTTREE_H__

#include <ZLTreeNode.h>
#include <ZLTreeListener.h>
#include <ZLResource.h>

#include "../tree/FBNode.h"

class LibraryTreeNode : ZLTreeListener::RootNode {

public:
	const ZLTypeId &typeId() const;

public:
	LibraryTreeNode(ZLTreeListener &listener);

};

class AuthorTreeNode : FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	AuthorTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

class TagTreeNode : FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	TagTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

class RecentBooksTreeNode : FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	RecentBooksTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

#endif /* __ROOTTREE_H__ */
