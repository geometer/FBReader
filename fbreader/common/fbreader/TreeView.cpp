/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLFileImage.h>
#include <abstract/ZLDialogManager.h>

#include "TreeView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

TreeView::TreeView(FBReader &reader, ZLPaintContext &context) : FBView(reader, context) {
	myTreeStateIsFrozen = false;
}

void TreeView::gotoParagraph(int num, bool last) {
	if ((num >= 0) && (num < (int)model()->paragraphsNumber())) {
		TreeParagraph *tp = (TreeParagraph*)(*model())[num];
		if (myTreeStateIsFrozen) {
			int corrected = num;
			TreeParagraph *parent = tp->parent();
			while ((corrected > 0) && (parent != 0) && !parent->isOpen()) {
				for (--corrected; ((corrected > 0) && parent != (*model())[corrected]); --corrected);
				parent = parent->parent();
			}
			if (last && (corrected != num)) {
				++corrected;
			}
			num = corrected;
		} else {
			tp->openTree();
			rebuildPaintInfo(true);
		}
	}
	TextView::gotoParagraph(num, last);
}

bool TreeView::onStylusPress(int x, int y) {
	myTreeStateIsFrozen = true;
	bool processedByParent = TextView::onStylusPress(x, y);
	myTreeStateIsFrozen = false;
	if (processedByParent) {
		return true;
	}

	const ParagraphPosition *position = paragraphByCoordinate(y);
	if (position == 0) {
		return false;
	}

	int paragraphNumber = position->ParagraphNumber;
	if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() <= paragraphNumber)) {
		return false;
	}

	TreeParagraph *paragraph = (TreeParagraph*)(*model())[paragraphNumber];
	if (paragraph->children().empty()) {
		return false;
	}

	const TextElementPosition *elementPosition = elementByCoordinates(x, y);
	if ((elementPosition == 0) || (elementPosition->Kind != TextElement::TREE_ELEMENT)) {
		return false;
	}

	paragraph->open(!paragraph->isOpen());
	rebuildPaintInfo(true);
	preparePaintInfo();
	if (paragraph->isOpen()) {
		int nextParagraphNumber = paragraphNumber + paragraph->fullSize();
		int lastParagraphNumber = endCursor().paragraphCursor().index();
		if (endCursor().isEndOfParagraph()) {
			++lastParagraphNumber;
		}
		if (lastParagraphNumber < nextParagraphNumber) {
			gotoParagraph(nextParagraphNumber, true);
			preparePaintInfo();
		}
	}
	int firstParagraphNumber = startCursor().paragraphCursor().index();
	if (startCursor().isStartOfParagraph()) {
		--firstParagraphNumber;
	}
	if (firstParagraphNumber >= paragraphNumber) {
		gotoParagraph(paragraphNumber);
		preparePaintInfo();
	}
	repaintView();

	return true;
}
