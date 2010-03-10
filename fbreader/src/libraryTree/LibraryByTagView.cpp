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

#include "LibraryView.h"
#include "LibraryNodes.h"

#include "../library/Library.h"
#include "../library/Book.h"
#include "../library/Tag.h"
#include "../library/Comparators.h"
#include "../libraryActions/BooksUtil.h"

LibraryByTagView::LibraryByTagView(ZLPaintContext &context) : LibraryView(context) {
}

void LibraryByTagView::collectTagNodes(const ZLBlockTreeNode &root, std::map<shared_ptr<Tag>,TagNode*,TagComparator> &nodeMap) {
	const ZLBlockTreeNode::List &children = root.children();
	for (ZLBlockTreeNode::List::const_iterator it = children.begin(); it != children.end(); ++it) {
		if ((*it)->isInstanceOf(TagNode::TYPE_ID)) {
			TagNode *tagNode = (TagNode*)*it;
			nodeMap[tagNode->tag()] = tagNode;
			collectTagNodes(*tagNode, nodeMap);
		}
	}
}

void LibraryByTagView::updateBookList(TagNode *tagNode) {
	const BookList &books = Library::Instance().books(tagNode->tag());
	const ZLBlockTreeNode::List &subNodes = tagNode->children();
	BookList::const_iterator jt = books.begin();
	ZLBlockTreeNode::List::const_iterator kt = subNodes.begin();
	for (; jt != books.end() && kt != subNodes.end(); ++jt, ++kt) {
		if (!(*kt)->isInstanceOf(BookNode::TYPE_ID)) {
			break;
		}
		if (((BookNode*)(*kt))->book()->filePath() != (*jt)->filePath()) {
			break;
		}
	}

	size_t index = jt - books.begin();
	while (tagNode->children().size() > index) {
		ZLBlockTreeNode *bookNode = tagNode->children()[index];
		if (!bookNode->isInstanceOf(BookNode::TYPE_ID)) {
			break;
		}
		delete bookNode;
	}

	for (; jt != books.end(); ++jt) {
		new BookNode(tagNode, index++, *jt);
	}
}

void LibraryByTagView::makeUpToDate() {
	TagList tags;
	BooksUtil::collectTagsFromLibrary(tags);

	std::map<shared_ptr<Tag>,TagNode*,TagComparator> nodeMap;
	collectTagNodes(rootNode(), nodeMap);

	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		shared_ptr<Tag> tag = *it;
		TagNode *tagNode = nodeMap[tag];
		if (tagNode == 0) {
			tagNode =
				(tag.isNull() || tag->parent().isNull()) ?
					new TagNode(&rootNode(), tag) :
					new TagNode(nodeMap[tag->parent()], tag);
			nodeMap[tag] = tagNode;
		}
		updateBookList(tagNode);
	}

	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		nodeMap.erase(nodeMap.find(*it));
	}

	for (std::map<shared_ptr<Tag>,TagNode*,TagComparator>::reverse_iterator it = nodeMap.rbegin(); it != nodeMap.rend(); ++it) {
		delete it->second;
	}
}
