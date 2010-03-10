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

#include <ZLResource.h>
#include <ZLImage.h>

#include "LibraryNodes.h"

#include "../library/Tag.h"
#include "../library/Comparators.h"
#include "../libraryActions/LibraryTagActions.h"

const ZLTypeId TagNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &TagNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &TagNode::resource() const {
	return ZLResource::resource("libraryView")["tagNode"];
}

size_t TagNode::positionToInsert(ZLBlockTreeNode *parent, shared_ptr<Tag> tag) {
	const ZLBlockTreeNode::List &children = parent->children();
	ZLBlockTreeNode::List::const_reverse_iterator it = children.rbegin();
	for (; it != children.rend(); ++it) {
		if (!(*it)->isInstanceOf(TagNode::TYPE_ID) ||
				TagComparator()(((TagNode*)*it)->tag(), tag)) {
			break;
		}
	}
	return children.rend() - it;
}

TagNode::TagNode(ZLBlockTreeView::RootNode *parent, shared_ptr<Tag> tag) : FBReaderNode(parent, positionToInsert(parent, tag)), myTag(tag) {
}

TagNode::TagNode(TagNode *parent, shared_ptr<Tag> tag) : FBReaderNode(parent, positionToInsert(parent, tag)), myTag(tag) {
}

void TagNode::init() {
	registerExpandTreeAction();
	if (!myTag.isNull()) {
		registerAction(new TagEditAction(myTag));
		registerAction(new TagCloneAction(myTag));
		registerAction(new TagRemoveAction(myTag));
	}
}

shared_ptr<Tag> TagNode::tag() const {
	return myTag;
}

std::string TagNode::title() const {
	if (myTag.isNull()) {
		return resource()["noTags"].value();
	}
	return myTag->name();
}

shared_ptr<ZLImage> TagNode::extractCoverImage() const {
	return defaultCoverImage("booktree-tag.png");
}
