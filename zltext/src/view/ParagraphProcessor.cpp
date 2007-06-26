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

#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLImage.h>

#include <ZLTextParagraph.h>

#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"

ParagraphCursor::ParagraphProcessor::ParagraphProcessor(const ZLTextParagraph &paragraph, const std::vector<ZLTextMark> &marks, int paragraphNumber, TextElementVector &elements) : myParagraph(paragraph), myElements(elements) {
	myFirstMark = std::lower_bound(marks.begin(), marks.end(), ZLTextMark(paragraphNumber, 0, 0));
	myLastMark = myFirstMark;
	for (; (myLastMark != marks.end()) && (myLastMark->ParagraphNumber == paragraphNumber); ++myLastMark);
	myOffset = 0;
}

ParagraphCursor::ParagraphProcessor::~ParagraphProcessor() {
}

void ParagraphCursor::ParagraphProcessor::addWord(const char *ptr, int offset, int len) {
	Word *word = TextElementPool::Pool.getWord(ptr, len, offset);
	for (std::vector<ZLTextMark>::const_iterator mit = myFirstMark; mit != myLastMark; ++mit) {
		ZLTextMark mark = *mit;
		if ((mark.Offset < offset + len) && (mark.Offset + mark.Length > offset)) {
			word->addMark(mark.Offset - offset, mark.Length);
		}
	}
	myElements.push_back(word);
}

void ParagraphCursor::ParagraphProcessor::fill() {
	for (ZLTextParagraph::Iterator it = myParagraph; !it.isEnd(); it.next()) {
		switch (it.entryKind()) {
			case ZLTextParagraphEntry::FORCED_CONTROL_ENTRY:
				myElements.push_back(new ForcedControlElement(it.entry()));
				break;
			case ZLTextParagraphEntry::FIXED_HSPACE_ENTRY:
				myElements.push_back(new FixedHSpaceElement(((ZLTextFixedHSpaceEntry&)*it.entry()).length()));
				break;
			case ZLTextParagraphEntry::CONTROL_ENTRY:
			case ZLTextParagraphEntry::HYPERLINK_CONTROL_ENTRY:
				myElements.push_back(TextElementPool::Pool.getControlElement(it.entry()));
				break;
			case ZLTextParagraphEntry::IMAGE_ENTRY:
			{
				ImageEntry &imageEntry = (ImageEntry&)*it.entry();
				shared_ptr<const ZLImage> image = imageEntry.image();
				if (!image.isNull()) {
					shared_ptr<ZLImageData> data = ZLImageManager::instance().imageData(*image);
					if (!data.isNull()) {
						myElements.push_back(new ImageElement(imageEntry.id(), data));
					}
				}
				break;
			}
			case ZLTextParagraphEntry::TEXT_ENTRY:
				processTextEntry((const ZLTextEntry&)*it.entry());
				break;
		}
	}
}
