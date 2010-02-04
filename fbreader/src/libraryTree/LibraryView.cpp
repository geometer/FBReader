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

#include <queue>

#include <ZLResource.h>

#include "LibraryView.h"
#include "LibraryNodes.h"

#include "../library/Library.h"
#include "../library/Book.h"
#include "../options/FBOptions.h"

LibraryView::LibraryView(ZLPaintContext &context) : ZLBlockTreeView(context), myCollectionRevision(0) {
}

void LibraryView::paint() {
	const size_t revision = Library::Instance().revision();
	if (myCollectionRevision < revision) {
		myCollectionRevision = revision;
		makeUpToDate();
	}

	ZLBlockTreeView::paint();
}

const std::string &LibraryView::caption() const {
	return ZLResource::resource("library")["caption"].value();
}

ZLColor LibraryView::backgroundColor() const {
	return FBOptions::Instance().BackgroundColorOption.value();
}

void LibraryView::showBook(shared_ptr<Book> book) {
	makeUpToDate();
	ZLBlockTreeNode::List bookNodes;
	std::queue<ZLBlockTreeNode*> nodesQueue;
	nodesQueue.push(&rootNode());
	while (!nodesQueue.empty()) {
		const ZLBlockTreeNode::List &children = nodesQueue.front()->children();
		nodesQueue.pop();
		for (ZLBlockTreeNode::List::const_iterator it = children.begin(); it != children.end(); ++it) {
			if ((*it)->isInstanceOf(BookNode::TYPE_ID)) {
				// TODO: replace with == for shared_ptr<Book>
				//if (((BookNode*)*it)->book() == book) {
				if (((BookNode*)*it)->book()->filePath() == book->filePath()) {
					bookNodes.push_back(*it);
				}
			} else {
				nodesQueue.push(*it);
			}
		}
	}
	if (bookNodes.empty()) {
		return;
	}
	ZLBlockTreeNode *nodeToShow = bookNodes[0];
	VisibilityMode mode = INVISIBLE;
	for (ZLBlockTreeNode::List::iterator it = bookNodes.begin(); it != bookNodes.end(); ++it) {
		VisibilityMode nodeMode = visibilityMode(*it);
		if ((nodeMode == VISIBLE && mode != VISIBLE) ||
				(nodeMode != INVISIBLE && mode == INVISIBLE)) {
			nodeToShow = *it;
			mode = nodeMode;
		}
	}
	ensureVisible(nodeToShow);
}
