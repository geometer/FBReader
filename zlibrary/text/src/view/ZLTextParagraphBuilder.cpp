/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <linebreak.h>

#include <ZLImage.h>

#include <ZLTextParagraph.h>

#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"
#include "ZLTextParagraphBuilder.h"

ZLTextParagraphBuilder::ZLTextParagraphBuilder(const std::string &language, const ZLTextParagraph &paragraph, const std::vector<ZLTextMark> &marks, int paragraphNumber, ZLTextElementVector &elements) : myParagraph(paragraph), myElements(elements), myLanguage(language), myRTL(language == "ar") {
	myFirstMark = std::lower_bound(marks.begin(), marks.end(), ZLTextMark(paragraphNumber, 0, 0));
	myLastMark = myFirstMark;
	for (; (myLastMark != marks.end()) && (myLastMark->ParagraphNumber == paragraphNumber); ++myLastMark);
	myOffset = 0;

	static bool lineBreakInitialized = false;
	if (!lineBreakInitialized) {
		init_linebreak();
		lineBreakInitialized = true;
	}
}

void ZLTextParagraphBuilder::updateRTLState(bool state) {
	if (myCurrentRTL != state) {
		myCurrentRTL = state;
		myElements.push_back(
			(myCurrentRTL != myRTL) ?
				ZLTextElementPool::Pool.StartReversedSequenceElement :
				ZLTextElementPool::Pool.EndReversedSequenceElement
		);
	}
}

void ZLTextParagraphBuilder::addWord(const char *ptr, int offset, int len) {
	ZLTextWord *word = ZLTextElementPool::Pool.getWord(ptr, len, offset, myCurrentRTL);
	for (std::vector<ZLTextMark>::const_iterator mit = myFirstMark; mit != myLastMark; ++mit) {
		ZLTextMark mark = *mit;
		if ((mark.Offset < offset + len) && (mark.Offset + mark.Length > offset)) {
			word->addMark(mark.Offset - offset, mark.Length);
		}
	}
	myElements.push_back(word);
}

void ZLTextParagraphBuilder::fill() {
	myBidiCharType = myRTL ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
	myCurrentRTL = myRTL;

	for (ZLTextParagraph::Iterator it = myParagraph; !it.isEnd(); it.next()) {
		switch (it.entryKind()) {
			case ZLTextParagraphEntry::STYLE_ENTRY:
				myElements.push_back(new ZLTextStyleElement(it.entry()));
				break;
			case ZLTextParagraphEntry::FIXED_HSPACE_ENTRY:
				myElements.push_back(new ZLTextFixedHSpaceElement(((ZLTextFixedHSpaceEntry&)*it.entry()).length()));
				break;
			case ZLTextParagraphEntry::CONTROL_ENTRY:
			case ZLTextParagraphEntry::HYPERLINK_CONTROL_ENTRY:
				myElements.push_back(ZLTextElementPool::Pool.getControlElement(it.entry()));
				break;
			case ZLTextParagraphEntry::IMAGE_ENTRY:
			{
				ImageEntry &imageEntry = (ImageEntry&)*it.entry();
				shared_ptr<const ZLImage> image = imageEntry.image();
				if (!image.isNull()) {
					shared_ptr<ZLImageData> data = ZLImageManager::instance().imageData(*image);
					if (!data.isNull()) {
						myElements.push_back(new ZLTextImageElement(imageEntry.id(), data));
					}
				}
				break;
			}
			case ZLTextParagraphEntry::TEXT_ENTRY:
				processTextEntry((const ZLTextEntry&)*it.entry());
				break;
		}
	}

	updateRTLState(myRTL);
}

void ZLTextParagraphBuilder::processTextEntry(const ZLTextEntry &textEntry) {
	const size_t dataLength = textEntry.dataLength();
	if (dataLength == 0) {
		return;
	}

	myUcs4String.clear();
	ZLUnicodeUtil::utf8ToUcs4(myUcs4String, textEntry.data(), dataLength);
	int len = myUcs4String.size();
	myUcs4String.push_back(0);
	myBidiLevels.clear();
	myBidiLevels.reserve(len + 1);
	fribidi_log2vis(&myUcs4String[0], len, &myBidiCharType, 0, 0, 0, &myBidiLevels[0]);

	myBreaksTable.clear();
	myBreaksTable.reserve(dataLength);
	const char *start = textEntry.data();
	const char *end = start + dataLength;
	set_linebreaks_utf8((const utf8_t*)start, dataLength, myLanguage.c_str(), &myBreaksTable[0]);

	ZLUnicodeUtil::Ucs4Char ch;
	enum { NO_SPACE, SPACE, NON_BREAKABLE_SPACE } spaceState = NO_SPACE;
	int charLength = 0;
	int index = 0;
	const char *wordStart = start;
	updateRTLState(myBidiLevels[0] % 2 == 1);
	for (const char *ptr = start; ptr < end; ptr += charLength, ++index) {
		charLength = ZLUnicodeUtil::firstChar(ch, ptr);
		if (ZLUnicodeUtil::isSpace(ch)) {
			if ((spaceState == NO_SPACE) && (ptr != wordStart)) {
				addWord(wordStart, myOffset + (wordStart - start), ptr - wordStart);
			}
			spaceState = SPACE;
		} else if (ZLUnicodeUtil::isNBSpace(ch)) {
			if (spaceState == NO_SPACE) {
				if (ptr != wordStart) {
					addWord(wordStart, myOffset + (wordStart - start), ptr - wordStart);
				}
				spaceState = NON_BREAKABLE_SPACE;
			}
		} else {
			switch (spaceState) {
				case SPACE:
					if (myBreaksTable[ptr - start - 1] == LINEBREAK_NOBREAK) {
						myElements.push_back(ZLTextElementPool::Pool.NBHSpaceElement);
					} else {
						myElements.push_back(ZLTextElementPool::Pool.HSpaceElement);
					}
					wordStart = ptr;
					break;
				case NON_BREAKABLE_SPACE:
					myElements.push_back(ZLTextElementPool::Pool.NBHSpaceElement);
					wordStart = ptr;
					break;
				case NO_SPACE:
					if ((ptr > start) &&
							(((myBreaksTable[ptr - start - 1] != LINEBREAK_NOBREAK) && (ptr != wordStart)) ||
							 (myBidiLevels[index - 1] != myBidiLevels[index]))) {
						addWord(wordStart, myOffset + (wordStart - start), ptr - wordStart);
						wordStart = ptr;
					}
					break;
			}
			spaceState = NO_SPACE;
		}
		updateRTLState(myBidiLevels[index] % 2 == 1);
	}
	switch (spaceState) {
		case SPACE:
			myElements.push_back(ZLTextElementPool::Pool.HSpaceElement);
			break;
		case NON_BREAKABLE_SPACE:
			myElements.push_back(ZLTextElementPool::Pool.NBHSpaceElement);
			break;
		case NO_SPACE:
			addWord(wordStart, myOffset + (wordStart - start), end - wordStart);
			break;
	}
	myOffset += dataLength;
}
