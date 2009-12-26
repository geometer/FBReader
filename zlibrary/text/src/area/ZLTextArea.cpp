/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLMirroredPaintContext.h>

#include <ZLTextModel.h>

#include "ZLTextArea.h"
#include "ZLTextLineInfo.h"

ZLTextArea::ZLTextArea(ZLPaintContext &context, const ColorMap &colorMap) : myContext(context), myColorMap(colorMap), myWidth(0), myHeight(0) {
}

ZLTextArea::~ZLTextArea() {
}

int ZLTextArea::realX(int x) const {
	return myMirroredContext.isNull() ? x : myMirroredContext->mirroredX(x);
}

void ZLTextArea::setModel(shared_ptr<ZLTextModel> model) {
	clear();

	if (model.isNull() || model->paragraphsNumber() == 0) {
		myModel = 0;
		return;
	}

	myModel = model;
	if (model->isRtl()) {
		myMirroredContext = new ZLMirroredPaintContext(myContext);
	} else {
		myMirroredContext.reset();
	}
	myStartCursor = ZLTextParagraphCursor::cursor(*model);
	myEndCursor = 0;
}

void ZLTextArea::clear() {
	myStartCursor = 0;
	myEndCursor = 0;

	myLineInfos.clear();
	myTextElementMap.clear();
	myTreeNodeMap.clear();
}

int ZLTextArea::paragraphIndexByCoordinates(int x, int y, bool absolute) const {
	if (absolute) {
		x -= myHOffset;
		y -= myVOffset;
	}

	x = realX(x);
	int paragraphIndex = -1;
	int yBottom = -1;
	int xLeft = context().width() + 1;
	int xRight = -1;

	for (ZLTextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
		if (it->YEnd < y) {
			paragraphIndex = it->ParagraphIndex;
			if (it->YStart > yBottom) {
				yBottom = it->YEnd;
				xLeft = it->XStart;
				xRight = -1;
			}
			xRight = it->XEnd;
			continue;
		}
		if (it->YStart > y) {
			return
				((paragraphIndex == it->ParagraphIndex) &&
				 (xLeft <= x) && (x <= xRight)) ?
				paragraphIndex : -1;
		}
		if (it->XEnd < x) {
			paragraphIndex = it->ParagraphIndex;
			if (it->YStart > yBottom) {
				yBottom = it->YEnd;
				xLeft = it->XStart;
				xRight = -1;
			}
			xRight = it->XEnd;
			continue;
		}
		if (it->XStart > x) {
			return
				((paragraphIndex == it->ParagraphIndex) &&
				 (it->YStart <= yBottom) && (xLeft < x)) ?
				paragraphIndex : -1;
		}
		return it->ParagraphIndex;
	}
	return -1;
}

const ZLTextElementRectangle *ZLTextArea::elementByCoordinates(int x, int y, bool absolute) const {
	if (absolute) {
		x -= myHOffset;
		y -= myVOffset;
	}

	ZLTextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), ZLTextRectangle::RangeChecker(realX(x), y));
	return (it != myTextElementMap.end()) ? &*it : 0;
}

const ZLTextTreeNodeRectangle *ZLTextArea::treeNodeByCoordinates(int x, int y, bool absolute) const {
	if (absolute) {
		x -= myHOffset;
		y -= myVOffset;
	}

	ZLTextTreeNodeMap::const_iterator it =
		std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), ZLTextRectangle::RangeChecker(x, y));
	return (it != myTreeNodeMap.end()) ? &*it : 0;
}

void ZLTextArea::drawSelectionRectangle(int left, int top, int right, int bottom) {
	left = std::max(left, 0);
	right = std::min(right, (int)width() - 1);
	if (left < right) {
		context().setFillColor(myColorMap.color(ZLTextStyle::SELECTION_BACKGROUND));
		context().fillRectangle(
			myHOffset + left,
			myVOffset + top,
			myHOffset + right,
			myVOffset + bottom
		);
	}
}
