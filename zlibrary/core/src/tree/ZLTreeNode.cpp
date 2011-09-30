/*
 * Copyright (C) 2009-2011 Geometer Plus <contact@geometerplus.com>
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

#include "ZLTreeNode.h"
#include "ZLTreeListener.h"
#include "../resources/ZLResource.h"
#include <algorithm>

const ZLTypeId ZLTreeAction::TYPE_ID(ZLExecutionData::TYPE_ID);

ZLTreeAction::ZLTreeAction() {
}

ZLTreeAction::~ZLTreeAction() {
}

const ZLTypeId &ZLTreeAction::typeId() const {
	return TYPE_ID;
}

std::string ZLTreeAction::text(const ZLResource &resource) const {
	return resource[key()].value();
}

bool ZLTreeAction::makesSense() const {
	return true;
}

const ZLTypeId ZLTreeNode::TYPE_ID(ZLObjectWithRTTI::TYPE_ID);

ZLTreeNode::ZLTreeNode() : myParent(0), myChildIndex(-1) {
}

ZLTreeNode::~ZLTreeNode() {
	for (List::iterator it = myChildren.begin(); it != myChildren.end(); ++it) {
		delete *it;
	}
}

const ZLTypeId &ZLTreeNode::typeId() const {
	return TYPE_ID;
}

void ZLTreeNode::clear() {
	ZLTreeListener * const handler = listener();
	List::iterator it;
	for (int i = myChildren.size() - 1; i >= 0; --i) {
		it = myChildren.begin() + i;
		if (handler)
			handler->onNodeBeginRemove(this, i);
		delete *it;
		myChildren.erase(it);
		if (handler)
			handler->onNodeEndRemove();
	}
}

size_t ZLTreeNode::level() const {
	size_t l = 0;
	for (ZLTreeNode *parent = myParent; parent; parent = parent->myParent) {
		++l;
	}
	return l;
}

ZLTreeListener *ZLTreeNode::listener() const {
	if (myParent)
		return myParent->listener();
	else if (const ZLTreeListener::RootNode *node = zlobject_cast<const ZLTreeListener::RootNode*>(this))
		return &node->myListener;
	else
		return 0;
}

ZLTreeNode *ZLTreeNode::parent() const {
	return myParent;
}

ZLTreeNode *ZLTreeNode::previous() const {
	if (!myParent || myChildIndex == 0)
		return 0;
	return myParent->children().at(myChildIndex - 1);
}

ZLTreeNode *ZLTreeNode::next() const {
	if (!myParent || myChildIndex + 1 >= myParent->children().size())
		return 0;
	return myParent->children().at(myChildIndex + 1);
}

const ZLTreeNode::List &ZLTreeNode::children() const {
	return myChildren;
}

size_t ZLTreeNode::childIndex() const {
	return myChildIndex;
}

void ZLTreeNode::requestChildren(shared_ptr<ZLExecutionData::Listener> listener) {
        if (!listener.isNull())
            listener->finished();
}

const ZLResource &ZLTreeNode::resource() const {
	static const ZLResource &emptyResource = ZLResource::resource(std::string());
	return emptyResource;
}

void ZLTreeNode::registerAction(shared_ptr<ZLTreeAction> action) {
	if (!action.isNull()) {
		myActions.push_back(action);
	}
}

const std::vector<shared_ptr<ZLTreeAction> > &ZLTreeNode::actions() const {
	return myActions;
}

std::string ZLTreeNode::actionText(const shared_ptr<ZLTreeAction> &action) const {
	return action.isNull() ? std::string() : action->text(resource());
}

void ZLTreeNode::close() {
	if (ZLTreeListener *treeListener = listener())
		treeListener->onCloseRequest();
}

void ZLTreeNode::insert(ZLTreeNode *node, size_t index) {
	if (myChildren.end() != std::find(myChildren.begin(), myChildren.end(), node))
		return;
	index = std::min(index, myChildren.size());
	node->myChildIndex = index;
	node->myParent = this;
	ZLTreeListener * const handler = listener();
	if (handler)
		handler->onNodeBeginInsert(this, index);
	List::iterator it = myChildren.insert(myChildren.begin() + index, node);
	for (; it != myChildren.end(); ++it) {
		++(*it)->myChildIndex;
	}
	if (handler)
		handler->onNodeEndInsert();
}

void ZLTreeNode::append(ZLTreeNode *node) {
	insert(node, myChildren.size());
}

void ZLTreeNode::remove(ZLTreeNode *node) {
	if (node->parent() == this)
		remove(node->childIndex());
}

void ZLTreeNode::remove(size_t index) {
	if (index >= myChildren.size())
		return;
	ZLTreeListener * const handler = listener();
	if (handler)
		handler->onNodeBeginRemove(this, index);
	List::iterator it = myChildren.erase(myChildren.begin() + index);
	for (; it != myChildren.end(); ++it) {
		--(*it)->myChildIndex;
	}
	if (handler)
		handler->onNodeEndRemove();
}

void ZLTreeNode::updated() {
	if (ZLTreeListener *handler = listener())
		handler->onNodeUpdated(this);
}
