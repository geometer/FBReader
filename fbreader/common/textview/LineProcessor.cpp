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

	do {
		newWidth += myStyle.elementWidth(current);
		int eltHeight = myStyle.elementHeight(current);
		if (eltHeight > newHeight) {
			newHeight = eltHeight;
		}
		TextElement::Kind elementKind = current.element().kind();
		if (elementKind == TextElement::CONTROL_ELEMENT) { 
			myStyle.applyControl((const ControlElement&)current.element(), false);
		} else if ((elementKind == TextElement::WORD_ELEMENT) || (elementKind == TextElement::IMAGE_ELEMENT)) { 
			wordOccured = true;
		} else if (wordOccured && (elementKind == TextElement::HSPACE_ELEMENT)) { 
			wordOccured = false;
			internalSpaceCounter++;
			lastSpaceWidth = myStyle.context().spaceWidth();
			newWidth += lastSpaceWidth;
		}

		if ((newWidth > maxWidth) && !cursor.sameElementAs(start)) {
			break;
		}

		current.nextWord();
		bool allowBreak = current.sameElementAs(end);
		if (!allowBreak) {
			TextElement::Kind nextElementKind = current.element().kind();
			allowBreak =
				(nextElementKind != TextElement::WORD_ELEMENT) &&
				(nextElementKind != TextElement::IMAGE_ELEMENT) &&
				(nextElementKind != TextElement::CONTROL_ELEMENT);
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
			HyphenationInfo info = Hyphenator::instance().info(word);
			int spaceLeft = maxWidth - newWidth;
			int hyphenationPosition = word.length() - 1;
			int subwordWidth = 0;
			for (; hyphenationPosition > 0; hyphenationPosition--) {
				if (info.isHyphenationPossible(hyphenationPosition)) {
					subwordWidth = myStyle.context().wordWidth(word, 0, hyphenationPosition, word.charAt(hyphenationPosition - 1) != '-');
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

	if (removeLastSpace) {
		myWidth -= lastSpaceWidth;
		mySpaceCounter--;
	}

	myStyle.applyControls(cursor, current, true);

	return cursor;
}
