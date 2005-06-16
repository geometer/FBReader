/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/PaintContext.h>

#include "TextView.h"

void TextView::drawTreeNode(TreeElement::TreeElementKind kind) {
	int x = context().x();
	int y = context().y();
	switch (kind) {
		case TreeElement::TREE_ELEMENT_CLOSED_NODE:
		case TreeElement::TREE_ELEMENT_OPEN_NODE:
		{
			int size = context().stringHeight() / 3 * 2;
			x += size / 2;
			int space = size / 5;
			if (space < 2) {
				space = 2;
			}
			context().setColor(TreeLinesColorOption.value());
			context().drawLine(x, y, x, y - size);
			context().drawLine(x + size, y, x + size, y - size);
			context().drawLine(x, y, x + size, y);
			context().drawLine(x, y - size, x + size, y - size);
			context().drawLine(x + space, y - size / 2, x + size - space, y - size / 2);
			if (kind == TreeElement::TREE_ELEMENT_CLOSED_NODE) {
				context().drawLine(x + size / 2, y - space, x + size / 2, y - size + space);
			}
			break;
		}
		case TreeElement::TREE_ELEMENT_LEAF:
		{
			int size = context().stringHeight();
			context().setColor(TreeLinesColorOption.value());
			context().drawLine(x, y - size / 3, x + size, y - size / 3);
			break;
		}
		case TreeElement::TREE_ELEMENT_TOP_RIGHT_LINE:
		{
			int size = context().stringHeight();
			context().setColor(TreeLinesColorOption.value());
			context().drawLine(x + size / 3 * 2, y - size / 3, x + size / 3 * 2, y - myLineProcessor.height() + 1);
			context().drawLine(x + size / 3 * 2, y - size / 3, x + size * 4 / 3, y - size / 3);
			break;
		}
		case TreeElement::TREE_ELEMENT_TOP_BOTTOM_RIGHT_LINE:
		{
			int size = context().stringHeight();
			context().setColor(TreeLinesColorOption.value());
			context().drawLine(x + size / 3 * 2, y, x + size / 3 * 2, y - myLineProcessor.height() + 1);
			context().drawLine(x + size / 3 * 2, y - size / 3, x + size * 4 / 3, y - size / 3);
			break;
		}
		case TreeElement::TREE_ELEMENT_SKIP:
			break;
	}
}
