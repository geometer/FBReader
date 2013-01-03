/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLBLOCKTREENODE_H__
#define __ZLBLOCKTREENODE_H__

#include <map>
#include <vector>

#include <shared_ptr.h>

#include <ZLTypeId.h>
#include <ZLRunnable.h>
#include <ZLPaintContext.h>

class ZLBlockTreeView;

class ZLBlockTreeNode : public ZLObjectWithRTTI {

public:
	typedef std::vector<ZLBlockTreeNode*> List;

protected:
	struct Rectangle {
		const std::size_t Left;
		const std::size_t Top;
		const std::size_t Right;
		const std::size_t Bottom;

		Rectangle(std::size_t left, std::size_t vOffset, std::size_t right, std::size_t bottom);

		bool contains(std::size_t x, std::size_t y) const;
		bool operator < (const Rectangle &r) const;
	};

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLBlockTreeNode(ZLBlockTreeView &view);
	ZLBlockTreeNode(ZLBlockTreeNode *parent, std::size_t atPosition = -1);

public:
	virtual ~ZLBlockTreeNode();
	void clear();

public:
	ZLBlockTreeNode *parent() const;
	ZLBlockTreeNode *previous() const;
	ZLBlockTreeNode *next() const;
	const List &children() const;

	void open(bool openNotClose);
	bool isOpen() const;

protected:
	std::size_t level() const;

	ZLBlockTreeView &view();

public:
	virtual void paint(ZLPaintContext &context, int vOffset) = 0;
	virtual int height(ZLPaintContext &context) const = 0;

	void onStylusPress(std::size_t x, std::size_t y);
	bool isOverHyperlink(std::size_t x, std::size_t y);

protected:
	void addHyperlink(std::size_t left, std::size_t top, std::size_t right, std::size_t bottom, shared_ptr<ZLRunnableWithKey> action);
	void removeAllHyperlinks();

private:
	ZLBlockTreeView &myView;
	ZLBlockTreeNode *myParent;
	std::size_t myChildIndex;

	List myChildren;
	bool myIsOpen;

	typedef std::map<Rectangle,shared_ptr<ZLRunnableWithKey> > LinkMap;
	LinkMap myHyperlinks;

private:
	ZLBlockTreeNode(const ZLBlockTreeNode&);
	const ZLBlockTreeNode &operator = (const ZLBlockTreeNode&);
};

#endif /* __ZLBLOCKTREENODE_H__ */
