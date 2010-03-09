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
#include "../library/Author.h"

LibraryByAuthorView::LibraryByAuthorView(ZLPaintContext &context) : LibraryView(context) {
}

void LibraryByAuthorView::addAuthorSubtree(shared_ptr<Author> author, size_t atPosition) {
	static const std::set<shared_ptr<Book> > emptySet;
	fillAuthorSubtree(new AuthorNode(&rootNode(), atPosition, author), emptySet);
}

void LibraryByAuthorView::fillAuthorSubtree(AuthorNode *authorNode, const std::set<shared_ptr<Book> > &visibleBooks) {
	const BookList &books = Library::Instance().books(authorNode->author());
	SeriesNode *seriesNode = 0;
	for (BookList::const_iterator it = books.begin(); it != books.end(); ++it) {
		std::string series = (*it)->seriesTitle();

		if (!series.empty() && (seriesNode == 0 || seriesNode->book()->seriesTitle() != series)) {
			BookList::const_iterator jt = it + 1;
			if (jt == books.end() || (*jt)->seriesTitle() != series) {
				series.clear();
			}
		}

		if (series.empty()) {
			seriesNode = 0;
			new BookNode(authorNode, *it);
		} else {
			if (seriesNode == 0 || seriesNode->book()->seriesTitle() != series) {
				seriesNode = new SeriesNode(authorNode);
			}
			new BookNode(seriesNode, *it);
			if (visibleBooks.find(*it) != visibleBooks.end()) {
				seriesNode->open(true);
			}
		}
	}
}

bool LibraryByAuthorView::isSubtreeUpToDate(AuthorNode *authorNode) {
	const BookList &books = Library::Instance().books(authorNode->author());
	BookList::const_iterator it = books.begin();

	const ZLBlockTreeNode::List &nodes = authorNode->children();
	for (ZLBlockTreeNode::List::const_iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt) {
		FBReaderNode &node = *(FBReaderNode*)*nIt;
		if (node.isInstanceOf(BookNode::TYPE_ID)) {
			shared_ptr<Book> book = ((BookNode&)node).book();
			if (it == books.end() || *it != book || !book->seriesTitle().empty()) {
				return false;
			}
			++it;
		} else /* if (node.isInstanceOf(SeriesNode::TYPE_ID)) */ {
			const ZLBlockTreeNode::List &bNodes = node.children();
			for (ZLBlockTreeNode::List::const_iterator bookIt = bNodes.begin(); bookIt != bNodes.end(); ++bookIt) {
				shared_ptr<Book> book = ((BookNode*)*bookIt)->book();
				if (it == books.end() || *it != book || book->seriesTitle().empty()) {
					return false;
				}
				++it;
			}
		}
	}
	return it == books.end();
}

void LibraryByAuthorView::updateAuthorSubtree(AuthorNode *authorNode) {
	std::set<shared_ptr<Book> > visibleBooks;

	const ZLBlockTreeNode::List &nodes = authorNode->children();
	for (ZLBlockTreeNode::List::const_iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt) {
		FBReaderNode &node = *(FBReaderNode*)*nIt;
		if (node.isInstanceOf(BookNode::TYPE_ID)) {
			visibleBooks.insert(((BookNode&)node).book());
		} else if (node.isOpen()) {
			const ZLBlockTreeNode::List &bNodes = node.children();
			for (ZLBlockTreeNode::List::const_iterator bookIt = bNodes.begin(); bookIt != bNodes.end(); ++bookIt) {
				visibleBooks.insert(((BookNode*)*bookIt)->book());
			}
		}
	}

	authorNode->clear();
	fillAuthorSubtree(authorNode, visibleBooks);
}

void LibraryByAuthorView::makeUpToDate() {
	ZLBlockTreeNode *topNode = firstVisibleNode();
	AuthorNode *topAuthorNode = 0;
	if (topNode != &rootNode()) {
		FBReaderNode *lNode = (FBReaderNode*)topNode;
		while (!lNode->isInstanceOf(AuthorNode::TYPE_ID)) {
			lNode = (FBReaderNode*)lNode->parent();
		}
		topAuthorNode = (AuthorNode*)lNode;
	}

	bool topAuthorNodeIsUpdated = false;
	const AuthorList &authors = Library::Instance().authors();
	std::set<ZLBlockTreeNode*> nodesToDelete;
	ZLBlockTreeNode::List rootChildren = rootNode().children();
	AuthorComparator comparator;

	ZLBlockTreeNode::List::iterator nodeIt = rootChildren.begin();
	size_t nodeCount = 0;
	for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		bool processed = false;
		while (nodeIt != rootChildren.end()) {
			AuthorNode *authorNode = (AuthorNode*)*nodeIt;
			if (authorNode->author() == *it) {
				if (!isSubtreeUpToDate(authorNode)) {
					updateAuthorSubtree(authorNode);
					if (authorNode == topAuthorNode) {
						topAuthorNodeIsUpdated = true;
					}
				}
				++nodeIt;
				++nodeCount;
				processed = true;
				break;
			} else if (comparator(authorNode->author(), *it)) {
				nodesToDelete.insert(*nodeIt);
				++nodeIt;
				++nodeCount;
			} else {
				break;
			}
		}
		if (!processed) {
			addAuthorSubtree(*it, nodeCount);
			++nodeCount;
		}
	}

	nodesToDelete.insert(nodeIt, rootChildren.end());

	if (topAuthorNodeIsUpdated) {
		setFirstVisibleNode(topAuthorNode);
	} else if (nodesToDelete.find(topAuthorNode) != nodesToDelete.end()) {
		ZLBlockTreeNode *visible = topAuthorNode->previous();
		while (nodesToDelete.find(visible) != nodesToDelete.end()) {
			visible = visible->previous();
		}
		setFirstVisibleNode(visible);
	}

	for (std::set<ZLBlockTreeNode*>::iterator it = nodesToDelete.begin(); it != nodesToDelete.end(); ++it) {
		delete *it;
	}
}
