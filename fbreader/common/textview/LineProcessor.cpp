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

#include <abstract/ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"

#include "../view/PaintContext.h"

#include "../hyphenation/Hyphenator.h"

WordCursor TextView::LineProcessor::process(const WordCursor &start, const WordCursor &end) {
	WordCursor cursor = start;
	WordCursor current = cursor;

	myWidth = myStyle.style().leftIndent();
	int newWidth = myWidth;
	myHeight = 0;
	mySpaceCounter = 0;
	int newHeight = myHeight;
	int maxWidth = myStyle.context().width() - myStyle.style().rightIndent();
	bool wordOccured = false;
	int lastSpaceWidth = 0;
	int internalSpaceCounter = 0;
	int removeLastSpace = false;

	TextElement::Kind elementKind = current.element().kind();

	do {
		newWidth += myStyle.elementWidth(current);
		int eltHeight = myStyle.elementHeight(current);
		if (eltHeight > newHeight) {
			newHeight = eltHeight;
		}
		switch (elementKind) {
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)current.element(), false);
				break;
			case TextElement::WORD_ELEMENT:
			case TextElement::IMAGE_ELEMENT:
				wordOccured = true;
				break;
			case TextElement::HSPACE_ELEMENT:
				if (wordOccured) {
					wordOccured = false;
					internalSpaceCounter++;
					lastSpaceWidth = myStyle.context().spaceWidth();
					newWidth += lastSpaceWidth;
				}
				break;
			default:
				break;
		}

		if ((newWidth > maxWidth) && !cursor.sameElementAs(start)) {
			break;
		}

		current.nextWord();
		bool allowBreak = current.sameElementAs(end);
		if (!allowBreak) {
			elementKind = current.element().kind();
			allowBreak =
				(elementKind != TextElement::WORD_ELEMENT) &&
				(elementKind != TextElement::IMAGE_ELEMENT) &&
				(elementKind != TextElement::CONTROL_ELEMENT);
		}
		if (allowBreak) {
			myWidth = newWidth;
			if (newHeight > myHeight) {
				myHeight = newHeight;
			}
			cursor = current;
			mySpaceCounter = internalSpaceCounter;
			removeLastSpace = !wordOccured && (mySpaceCounter > 0);
		}
	} while (!current.sameElementAs(end));

	if (TextView::AutoHyphenationOption.value() && myStyle.style().allowHyphenations()) {
		if (!current.sameElementAs(end) && (current.element().kind() == TextElement::WORD_ELEMENT)) {
			newWidth -= myStyle.elementWidth(current);
			const Word &word = (Word&)current.element();
			int spaceLeft = maxWidth - newWidth;
			if ((word.length() > 3) && (spaceLeft > 2 * myStyle.context().spaceWidth())) {
				ZLUnicodeUtil::Ucs2String ucs2string;
				ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.data(), word.size());
				HyphenationInfo info = Hyphenator::instance().info(word);
				int hyphenationPosition = word.length() - 1;
				int subwordWidth = 0;
				for (; hyphenationPosition > 0; hyphenationPosition--) {
					if (info.isHyphenationPossible(hyphenationPosition)) {
						subwordWidth = myStyle.wordWidth(word, 0, hyphenationPosition, ucs2string[hyphenationPosition - 1] != '-');
						if (subwordWidth <= spaceLeft) {
							break;
						}
					}
				}
				if (hyphenationPosition > 0) {
					myWidth = newWidth + subwordWidth;
					if (newHeight > myHeight) {
						myHeight = newHeight;
					}
					cursor = current;
					mySpaceCounter = internalSpaceCounter;
					removeLastSpace = false;
					cursor.setCharNumber(hyphenationPosition);
				}
			}
		}
	}

	if (removeLastSpace) {
		myWidth -= lastSpaceWidth;
		mySpaceCounter--;
	}

	myStyle.applyControls(cursor, current, true);

	return cursor;
}
