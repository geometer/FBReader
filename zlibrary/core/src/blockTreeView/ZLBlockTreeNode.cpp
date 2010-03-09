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

#include <ZLResource.h>

#include "ZLBlockTreeNode.h"

const ZLTypeId ZLBlockTreeNode::TYPE_ID(ZLObjectWithRTTI::TYPE_ID);

ZLBlockTreeNode::ZLBlockTreeNode(ZLBlockTreeView &view) : myView(view), myParent(0), myChildIndex(0), myIsOpen(true) {
}

ZLBlockTreeNode::ZLBlockTreeNode(ZLBlockTreeNode *parent, size_t atPosition) : myView(parent->myView), myParent(parent), myIsOpen(false) {
	if (atPosition >= parent->myChildren.size()) {
		myChildIndex = parent->myChildren.size();
		parent->myChildren.push_back(this);
	} else {
		myChildIndex = atPosition;
		List::iterator it = parent->myChildren.begin() + atPosition;
		it = parent->myChildren.insert(it, this);
		for (++it; it != parent->myChildren.end(); ++it) {
			++(*it)->myChildIndex;
		}
	}
}

void ZLBlockTreeNode::clear() {
	for (List::iterator it = myChildren.begin(); it != myChildren.end(); ++it) {
		(*it)->myParent = 0;
		delete *it;
	}
	myChildren.clear();
}

ZLBlockTreeNode::~ZLBlockTreeNode() {
	if (myParent != 0) {
		List &children = myParent->myChildren;
		List::iterator it = std::find(children.begin(), children.end(), this);
		if (it != children.end()) {
			it = children.erase(it);
			for (; it != children.end(); ++it) {
				--(*it)->myChildIndex;
			}
		}
	}
	clear();
}

bool ZLBlockTreeNode::isOpen() const {
	return myIsOpen;
}

void ZLBlockTreeNode::open(bool openNotClose) {
	myIsOpen = openNotClose;
}

size_t ZLBlockTreeNode::level() const {
	size_t l = 0;
	for (ZLBlockTreeNode *parent = myParent; parent != 0; parent = parent->myParent) {
		++l;
	}
	return l;
}

void ZLBlockTreeNode::addHyperlink(size_t left, size_t top, size_t right, size_t bottom, shared_ptr<ZLRunnableWithKey> action) {
	myHyperlinks[Rectangle(left, top, right, bottom)] = action;
}

void ZLBlockTreeNode::removeAllHyperlinks() {
	myHyperlinks.clear();
}

bool ZLBlockTreeNode::isOverHyperlink(size_t x, size_t y) {
	for (LinkMap::const_iterator it = myHyperlinks.begin(); it != myHyperlinks.end(); ++it) {
		if (it->first.contains(x, y)) {
			return true;
		}
	}
	return false;
}

void ZLBlockTreeNode::onStylusPress(size_t x, size_t y) {
	for (LinkMap::const_iterator it = myHyperlinks.begin(); it != myHyperlinks.end(); ++it) {
		if (it->first.contains(x, y)) {
			if (!it->second.isNull()) {
				it->second->run();
			}
			break;
		}
	}
}

ZLBlockTreeView &ZLBlockTreeNode::view() {
	return myView;
}

ZLBlockTreeNode::Rectangle::Rectangle(size_t left, size_t top, size_t right, size_t bottom) :
	Left(std::min(left, right)),
	Top(std::min(top, bottom)),
	Right(std::max(left, right)),
	Bottom(std::max(top, bottom)) {
}

bool ZLBlockTreeNode::Rectangle::contains(size_t x, size_t y) const {
	return Left <= x && x <= Right && Top <= y && y <= Bottom;
}

bool ZLBlockTreeNode::Rectangle::operator < (const Rectangle &r) const {
	if (Left != r.Left) return Left < r.Left;
	if (Top != r.Top) return Top < r.Top;
	if (Right != r.Right) return Right < r.Right;
	return Bottom < r.Bottom;
}

ZLBlockTreeNode *ZLBlockTreeNode::parent() const {
	return myParent;
}

ZLBlockTreeNode *ZLBlockTreeNode::previous() const {
	if (myChildIndex == 0) {
		return myParent;
	}
	if (myParent == 0) {
		return 0;
	}
	ZLBlockTreeNode *node = myParent->myChildren[myChildIndex - 1];
	while (node->myIsOpen && !node->myChildren.empty()) {
		node = node->myChildren.back();
	}
	return node;
}

ZLBlockTreeNode *ZLBlockTreeNode::next() const {
	if (myIsOpen && !myChildren.empty()) {
		return myChildren.front();
	}

	const ZLBlockTreeNode *current = this;
	while (current->myParent != 0) {
		const size_t index = current->myChildIndex + 1;
		current = current->myParent;
		if (index < current->myChildren.size()) {
			return current->myChildren[index];
		}
	}
	return 0;
}

const ZLBlockTreeNode::List &ZLBlockTreeNode::children() const {
	return myChildren;
}
