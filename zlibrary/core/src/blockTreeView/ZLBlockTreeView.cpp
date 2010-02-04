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

#include <algorithm>

#include <ZLApplication.h>

#include "ZLBlockTreeView.h"

const ZLTypeId ZLBlockTreeView::TYPE_ID(ZLView::TYPE_ID);

const ZLTypeId &ZLBlockTreeView::typeId() const {
	return TYPE_ID;
}

ZLBlockTreeView::ZLBlockTreeView(ZLPaintContext &context) : ZLView(context), myRootNode(*this) {
	myFirstVisibleNode = &myRootNode;
	myNodePartToSkip = 0;
}

void ZLBlockTreeView::paint() {
	ZLPaintContext &context = this->context();
	context.clear(backgroundColor());

	bool firstNodeFound = false;
	int vOffset = -myNodePartToSkip;
	const int maxY = context.height();

	size_t before = myNodePartToSkip;

	for (ZLBlockTreeNode *node = &myRootNode; node != 0; node = node->next()) {
		size_t h = node->height(context);
		if (!firstNodeFound) {
			if (node == myFirstVisibleNode) {
				firstNodeFound = true;
			}
			if (!firstNodeFound) {
				before += h;
				continue;
			}
		}
		if (vOffset <= maxY) {
			node->paint(context, vOffset);
		}
		vOffset += h;
	}

	if (!firstNodeFound ||
			(((int) before) + vOffset <= maxY &&
			 (myFirstVisibleNode != &myRootNode || myNodePartToSkip != 0)
			)
		 ) {
		myFirstVisibleNode = &myRootNode;
		myNodePartToSkip = 0;
		paint();
		return;
	}

	const size_t after = (vOffset <= maxY) ? 0 : vOffset - maxY;

	const bool showScrollbar = before > 0 || after > 0;
	myCanScrollForward = after > 0;
	setScrollbarEnabled(VERTICAL, showScrollbar);
	if (showScrollbar) {
		setScrollbarParameters(VERTICAL,
			before + maxY + after,
			before,
			before + maxY
		);
	}
}

ZLBlockTreeNode *ZLBlockTreeView::findNode(int &y) {
	y += myNodePartToSkip;
	for (ZLBlockTreeNode *node = myFirstVisibleNode; node != 0; node = node->next()) {
		const int h = node->height(context());
		if (y < h) {
			return node;
		}
		y -= h;
	}
	return 0;
}

bool ZLBlockTreeView::onStylusPress(int x, int y) {
	ZLBlockTreeNode *node = findNode(y);
	if (node != 0) {
		node->onStylusPress(x, y);
	}
	return true;
}

bool ZLBlockTreeView::onStylusMove(int x, int y) {
	ZLBlockTreeNode *node = findNode(y);
	ZLApplication::Instance().setHyperlinkCursor(node != 0 && node->isOverHyperlink(x, y));
	return true;
}

ZLBlockTreeView::RootNode &ZLBlockTreeView::rootNode() {
	return myRootNode;
}

void ZLBlockTreeView::clear() {
	myRootNode.clear();
	myFirstVisibleNode = &myRootNode;
	myNodePartToSkip = 0;
}

void ZLBlockTreeView::onScrollbarMoved(Direction direction, size_t full, size_t from, size_t) {
	if (direction != VERTICAL) {
		return;
	}

	from = std::min(from, full - context().height());

	for (ZLBlockTreeNode *node = &myRootNode; node != 0; node = node->next()) {
		size_t h = node->height(context());
		if (from < h) {
			myFirstVisibleNode = node;
			myNodePartToSkip = from;
			break;
		}
		from -= h;
	}
	ZLApplication::Instance().refreshWindow();
}

void ZLBlockTreeView::onScrollbarStep(Direction direction, int steps) {
	if (direction != VERTICAL) {
		return;
	}

	const bool back = steps < 0;
	if (back) {
		steps = -steps;
	}
	if (!back && !myCanScrollForward) {
		return;
	}
	while (steps > 0) {
		if (back && myNodePartToSkip > 0) {
			myNodePartToSkip = 0;
			--steps;
			continue;
		}

		if (myFirstVisibleNode->height(context()) > 0) {
			--steps;
		}
		ZLBlockTreeNode * const node = back ?
			myFirstVisibleNode->previous() :
			myFirstVisibleNode->next();
		if (node == 0) {
			break;
		}
		myFirstVisibleNode = node;
	}
	ZLApplication::Instance().refreshWindow();
}

void ZLBlockTreeView::onScrollbarPageStep(Direction direction, int steps) {
	if (direction != VERTICAL) {
		return;
	}

	const bool back = steps < 0;
	if (back) {
		steps = -steps;
	}
	if (!back && !myCanScrollForward) {
		return;
	}
	for (; steps > 0; --steps) {
		int height = context().height();
		if (back) {
			height -= myNodePartToSkip;
		} else {
			height -= myFirstVisibleNode->height(context()) - myNodePartToSkip;
		}
		ZLBlockTreeNode *node = myFirstVisibleNode;
		while (height >= 0) {
			node = back ? node->previous() : node->next();
			if (node == 0) {
				break;
			}
			height -= node->height(context());
			if (!back || height > 0) {
				myFirstVisibleNode = node;
			}
		}
		myNodePartToSkip = 0;
	}
	ZLApplication::Instance().refreshWindow();
}

void ZLBlockTreeView::scroll(ScrollingMode mode, bool back) {
	switch (mode) {
		case PAGE:
			onScrollbarPageStep(VERTICAL, back ? -1 : 1);
			break;
		case ITEM:
			onScrollbarStep(VERTICAL, back ? -1 : 1);
			break;
		case NONE:
			break;
	}
}

ZLBlockTreeNode *ZLBlockTreeView::firstVisibleNode() const {
	return myFirstVisibleNode;
}

void ZLBlockTreeView::setFirstVisibleNode(ZLBlockTreeNode *node) {
	if (myFirstVisibleNode != node) {
		myFirstVisibleNode = node;
		myNodePartToSkip = 0;
	}
}

ZLBlockTreeView::VisibilityMode ZLBlockTreeView::visibilityMode(ZLBlockTreeNode *node) const {
	bool firstNodeFound = false;
	for (const ZLBlockTreeNode *it = &myRootNode; it != 0; it = it->next()) {
		if (it == myFirstVisibleNode) {
			firstNodeFound = true;
			break;
		}
	}
	if (!firstNodeFound) {
		return INVISIBLE;
	}

	if (node == myFirstVisibleNode) {
		return myNodePartToSkip == 0 ? VISIBLE : VISIBLE_PARTIAL_AT_TOP;
	}

	const ZLBlockTreeNode *jt = myFirstVisibleNode;
	int height = context().height() + myNodePartToSkip;
	for (; jt != 0 && height > 0; jt = jt->next()) {
		if (jt == node) {
			return height >= node->height(context()) ?
				VISIBLE : VISIBLE_PARTIAL_AT_BOTTOM;
		}
		height -= jt->height(context());
	}
	return INVISIBLE;
}

void ZLBlockTreeView::ensureVisible(ZLBlockTreeNode *node) {
	if (visibilityMode(node) == VISIBLE) {
		return;
	}

	for (ZLBlockTreeNode *parent = node->parent(); parent != 0; parent = parent->parent()) {
		parent->open(true);
	}
	ZLBlockTreeNode *previous = node->previous();
	setFirstVisibleNode(previous != 0 ? previous : node);

	if (previous && visibilityMode(node) != VISIBLE) {
		setFirstVisibleNode(node);
	}
}

const ZLTypeId ZLBlockTreeView::RootNode::TYPE_ID(ZLBlockTreeNode::TYPE_ID);

ZLBlockTreeView::RootNode::RootNode(ZLBlockTreeView &view) : ZLBlockTreeNode(view) {
}

void ZLBlockTreeView::RootNode::paint(ZLPaintContext&, int) {
}

const ZLTypeId &ZLBlockTreeView::RootNode::typeId() const {
	return TYPE_ID;
}

int ZLBlockTreeView::RootNode::height(ZLPaintContext&) const {
	return 0;
}
