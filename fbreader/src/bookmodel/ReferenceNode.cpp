/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include "../fbreader/FBReader.h"
#include "../fbreader/BookTextView.h"

#include "BookModel.h"

#include "TOCTreeNodes.h"

const ZLTypeId ReferenceNode::TYPE_ID(ZLTreeActionNode::TYPE_ID);

const ZLTypeId &ReferenceNode::typeId() const {
	return TYPE_ID;
}

ReferenceNode::ReferenceNode(const ZLTextTreeParagraph& paragraph): myParagraph(paragraph) {
}

std::string ReferenceNode::title() const {
	return myParagraph.text();
}

bool ReferenceNode::activate() {
	FBReader &fbreader = FBReader::Instance();
	shared_ptr<BookModel> model = fbreader.myModel;
	int paragraphIndex = ((ContentsModel&)*model->contentsModel()).reference(&myParagraph);
	fbreader.bookTextView().gotoParagraph(paragraphIndex);
	return true;
}
