/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLBLOCKTREEVIEW_H__
#define __ZLBLOCKTREEVIEW_H__

#include <ZLView.h>

#include <ZLBlockTreeNode.h>

class ZLBlockTreeView : public ZLView {

public:
	class RootNode : public ZLBlockTreeNode {

	public:
		static const ZLTypeId TYPE_ID;
 
	public:
		RootNode(ZLBlockTreeView &view);

	private:
		const ZLTypeId &typeId() const;
		void paint(ZLPaintContext &context, int vOffset);
		int height(ZLPaintContext &context) const;
	};

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLBlockTreeView(ZLPaintContext &context);
	const ZLTypeId &typeId() const;

	RootNode &rootNode();
	void clear();

public:
	ZLBlockTreeNode *firstVisibleNode() const;
	void setFirstVisibleNode(ZLBlockTreeNode *node);

	enum VisibilityMode {
		INVISIBLE,
		VISIBLE,
		VISIBLE_PARTIAL_AT_TOP,
		VISIBLE_PARTIAL_AT_BOTTOM
	};

	VisibilityMode visibilityMode(ZLBlockTreeNode *node) const;

	void ensureVisible(ZLBlockTreeNode *node);

	enum ScrollingMode {
		NONE,
		PAGE,
		ITEM,
	};
	void scroll(ScrollingMode mode, bool back);

protected:
	void paint();

private:
	bool onStylusPress(int x, int y);
	bool onStylusMove(int x, int y);

	void onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to);
	void onScrollbarStep(Direction direction, int steps);
	void onScrollbarPageStep(Direction direction, int steps);

private:
	ZLBlockTreeNode *findNode(int &y);

private:
	RootNode myRootNode;
	ZLBlockTreeNode *myFirstVisibleNode;
	size_t myNodePartToSkip;
	bool myCanScrollForward;
};

#endif /* __ZLBLOCKTREEVIEW_H__ */
