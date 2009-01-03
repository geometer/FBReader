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

#include <algorithm>

#include <ZLPaintContext.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"

void ZLTextView::drawTreeLines(const ZLTextTreeNodeInfo &info, int x, int y, int height, int vSpaceAfter) {
	context().setColor(ZLTextStyleCollection::instance().baseStyle().TreeLinesColorOption.value());

	const int qstep = (context().stringHeight() + 2) / 3;

	const std::vector<bool> &Stack = info.VerticalLinesStack;
	const int depth = Stack.size();
	for (int i = depth - 1; i >= 0; --i) {
		if (Stack[i]) {
			context().drawLine(visualX(x + 2 * qstep), y + vSpaceAfter, visualX(x + 2 * qstep), y - height + 1);
		}
		x += 4 * qstep;
	}

	if (info.IsFirstLine) {
		if ((depth > 0) && !Stack[0]) {
			context().drawLine(visualX(x - 2 * qstep), y - qstep, visualX(x - 2 * qstep), y - height + 1);
		}

		if (info.IsLeaf) {
			if (depth > 0) {
				context().drawLine(visualX(x - 2 * qstep), y - qstep, visualX(x + 3 * qstep), y - qstep);
			}
		} else {
			int space = std::max(qstep * 2 / 5, 2);
			if (depth > 0) {
				context().drawLine(visualX(x - 2 * qstep), y - qstep, visualX(x + qstep), y - qstep);
			}
			const int x0 = x + qstep, x1 = x + 3 * qstep;
			const int y0 = y, y1 = y - 2 * qstep;
			context().drawLine(visualX(x0), y0, visualX(x0), y1);
			context().drawLine(visualX(x1), y0, visualX(x1), y1);
			context().drawLine(visualX(x0), y0, visualX(x1), y0);
			context().drawLine(visualX(x0), y1, visualX(x1), y1);
			context().drawLine(visualX(x0 + space), y - qstep, visualX(x1 - space), y - qstep);
			if (info.IsOpen) {
				context().drawLine(visualX(x + 2 * qstep), y + vSpaceAfter, visualX(x + 2 * qstep), y);
			} else {
				context().drawLine(visualX(x + 2 * qstep), y0 - space, visualX(x + 2 * qstep), y1 + space);
			}
			const int left = std::min(visualX(x), visualX(x + 4 * qstep));
			const int right = std::max(visualX(x), visualX(x + 4 * qstep));
			myTreeNodeMap.push_back(ZLTextTreeNodeArea(info.ParagraphIndex, left, right, y - height + 1, y));
		}
	} else if (!info.IsLeaf && info.IsOpen) {
		context().drawLine(visualX(x + 2 * qstep), y + vSpaceAfter, visualX(x + 2 * qstep), y - height + 1);
	}
}
