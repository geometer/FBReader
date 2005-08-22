/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"

#include "../hyphenation/Hyphenator.h"

TextView::LineInfo TextView::processTextLine(const WordCursor &start, const WordCursor &end) {
	LineInfo info(start, myStyle.style());

#ifndef PALM_TEMPORARY
	std::set<LineInfo>::const_iterator it = myLineInfoCache.find(info);
	if (it != myLineInfoCache.end()) {
		const LineInfo &storedInfo = *it;
		myStyle.applyControls(storedInfo.Start, storedInfo.End);
		return storedInfo;
	}
#endif // PALM_TEMPORARY

	TextStylePtr storedStyle = myStyle.style();

	info.Width = myStyle.style()->leftIndent();
	int newWidth = info.Width;
	int newHeight = info.Height;
	int maxWidth = myStyle.context().width() - myStyle.style()->rightIndent();
	bool wordOccured = false;
	bool isVisible = false;
	int lastSpaceWidth = 0;
	int internalSpaceCounter = 0;
	int removeLastSpace = false;

	WordCursor current = start;
	const ParagraphCursor &paragraph = current.paragraphCursor();
	TextElement::Kind elementKind = paragraph[current.wordNumber()].kind();

	do {
		const TextElement &element = paragraph[current.wordNumber()];
		newWidth += myStyle.elementWidth(element, current.charNumber());
		newHeight = std::max(newHeight, myStyle.elementHeight(element));
		switch (elementKind) {
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)element);
				break;
			case TextElement::WORD_ELEMENT:
			case TextElement::IMAGE_ELEMENT:
				wordOccured = true;
				isVisible = true;
				break;
			case TextElement::HSPACE_ELEMENT:
				if (wordOccured) {
					wordOccured = false;
					internalSpaceCounter++;
					lastSpaceWidth = myStyle.context().spaceWidth();
					newWidth += lastSpaceWidth;
				}
				break;
			case TextElement::EMPTY_LINE_ELEMENT:
			case TextElement::TREE_ELEMENT:
				isVisible = true;
			default:
				break;
		}

		if ((newWidth > maxWidth) && !info.End.sameElementAs(start)) {
			break;
		}

		current.nextWord();
		bool allowBreak = current.sameElementAs(end);
		if (!allowBreak) {
			elementKind = paragraph[current.wordNumber()].kind();
			allowBreak =
				(elementKind != TextElement::WORD_ELEMENT) &&
				(elementKind != TextElement::IMAGE_ELEMENT) &&
				(elementKind != TextElement::CONTROL_ELEMENT);
		}
		if (allowBreak) {
			info.IsVisible = isVisible;
			info.Width = newWidth;
			info.Height = std::max(info.Height, newHeight);
			info.End = current;
			storedStyle = myStyle.style();
			info.SpaceCounter = internalSpaceCounter;
			removeLastSpace = !wordOccured && (info.SpaceCounter > 0);
		}
	} while (!current.sameElementAs(end));

	const TextElement &element = paragraph[current.wordNumber()];
	if (TextView::AutoHyphenationOption.value() && myStyle.style()->allowHyphenations()) {
		if (!current.sameElementAs(end) && (element.kind() == TextElement::WORD_ELEMENT)) {
			newWidth -= myStyle.elementWidth(element, current.charNumber());
			const Word &word = (Word&)element;
			int spaceLeft = maxWidth - newWidth;
			if ((word.Length > 3) && (spaceLeft > 2 * myStyle.context().spaceWidth())) {
				ZLUnicodeUtil::Ucs2String ucs2string;
				ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
				HyphenationInfo hyphenationInfo = Hyphenator::instance().info(word);
				int hyphenationPosition = word.Length - 1;
				int subwordWidth = 0;
				for (; hyphenationPosition > 0; hyphenationPosition--) {
					if (hyphenationInfo.isHyphenationPossible(hyphenationPosition)) {
						subwordWidth = myStyle.wordWidth(word, 0, hyphenationPosition, ucs2string[hyphenationPosition - 1] != '-');
						if (subwordWidth <= spaceLeft) {
							break;
						}
					}
				}
				if (hyphenationPosition > 0) {
					info.IsVisible = true;
					info.Width = newWidth + subwordWidth;
					info.Height = std::max(info.Height, newHeight);
					info.End = current;
					storedStyle = myStyle.style();
					info.SpaceCounter = internalSpaceCounter;
					removeLastSpace = false;
					info.End.setCharNumber(hyphenationPosition);
				}
			}
		}
	}

	if (removeLastSpace) {
		info.Width -= lastSpaceWidth;
		info.SpaceCounter--;
	}

	myStyle.setStyle(storedStyle);

#ifndef PALM_TEMPORARY
	if (!info.End.sameElementAs(end) || end.isEndOfParagraph()) {
		myLineInfoCache.insert(info);
	}
#endif // PALM_TEMPORARY
	return info;
}
