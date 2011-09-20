#ifndef __ROOTTREE_H__
#define __ROOTTREE_H__

#include <ZLTreeNode.h>
#include <ZLTreeListener.h>
#include <ZLTreeActionNode.h>
#include <ZLOpenFileDialog.h>
#include <ZLOptions.h>

#include <ZLResource.h>

#include "../tree/FBNode.h"

class AuthorTreeNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	AuthorTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

class TagTreeNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	TagTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

class RecentBooksTreeNode : public FBNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	const ZLTypeId &typeId() const;

public:
	RecentBooksTreeNode();
	std::string title() const;
	std::string subtitle() const;
};

class OpenFileSystemNode : public ZLTreeActionNode {
	public:
		static const ZLTypeId TYPE_ID;

	public:
		const ZLTypeId &typeId() const;
		//TODO may be other name
		OpenFileSystemNode();

	private:
		class FileFilter : public ZLOpenFileDialog::Filter {
		private:
			bool accepts(const ZLFile &file) const;
		};

	public:
		std::string title() const;
		std::string subtitle() const;
		virtual bool activate();

	private:
		ZLStringOption DirectoryOption;
		ZLStringOption FileOption;


};

#endif /* __ROOTTREE_H__ */
