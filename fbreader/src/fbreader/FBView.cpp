/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLUnicodeUtil.h>

#include "FBView.h"
#include "FBReader.h"
#include "FBReaderActions.h"

FBView::FBView(FBReader &reader, ZLPaintContext &context) : TextView(reader, context) {
}

bool FBView::onFingerTap(int, int y) {
	if (2 * y < fbreader().context().height()) {
		fbreader().doAction(ACTION_FINGER_TAP_SCROLL_BACKWARD);
	} else {
		fbreader().doAction(ACTION_FINGER_TAP_SCROLL_FORWARD);
	}
	return true;
}

const std::string &FBView::caption() const {
	return myCaption;
}

void FBView::setCaption(const std::string &caption) {
	myCaption = caption;
}

bool FBView::onStylusPress(int x, int y) {
	if (TextView::onStylusPress(x, y)) {
		return true;
	}
	
	if (_onStylusPress(x, y)) {
		return true;
	}

	activateSelection(x, y);
	return true;
}

bool FBView::_onStylusPress(int, int) {
	return false;
}

std::string FBView::word(const TextElementArea &area) const {
	std::string txt;

	if (area.Kind == ZLTextElement::WORD_ELEMENT) {
		ZLTextWordCursor cursor = startCursor();
		cursor.moveToParagraph(area.ParagraphNumber);
		cursor.moveTo(area.TextElementNumber, 0);
		const ZLTextWord &word = (ZLTextWord&)cursor.element();
		ZLUnicodeUtil::Ucs2String ucs2;
		ZLUnicodeUtil::utf8ToUcs2(ucs2, word.Data, word.Size);
		ZLUnicodeUtil::Ucs2String::iterator it = ucs2.begin();
		while ((it != ucs2.end()) && !ZLUnicodeUtil::isLetter(*it)) {
			++it;
		}
		if (it != ucs2.end()) {
			ucs2.erase(ucs2.begin(), it);
			it = ucs2.end() - 1;
			while (!ZLUnicodeUtil::isLetter(*it)) {
				--it;
			}
			ucs2.erase(it + 1, ucs2.end());
    
			ZLUnicodeUtil::ucs2ToUtf8(txt, ucs2);
		}
	}
	return txt;
}
