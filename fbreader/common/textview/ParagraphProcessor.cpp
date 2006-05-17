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

#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "ParagraphCursor.h"
#include "Word.h"

#include "../model/Paragraph.h"
#include "../hyphenation/Hyphenator.h"

ParagraphCursor::ParagraphProcessor::ParagraphProcessor(const Paragraph &paragraph, const std::vector<TextMark> &marks, int paragraphNumber, TextElementVector &elements) : myParagraph(paragraph), myElements(elements) {
	myCheckBreakableCharacters = Hyphenator::instance().useBreakingAlgorithm();
	myFirstMark = std::lower_bound(marks.begin(), marks.end(), TextMark(paragraphNumber, 0, 0));
	myLastMark = myFirstMark;
	for (; (myLastMark != marks.end()) && (myLastMark->ParagraphNumber == paragraphNumber); ++myLastMark);
	myWordCounter = 0;
	myOffset = 0;
}

ParagraphCursor::ParagraphProcessor::~ParagraphProcessor() {
}

void ParagraphCursor::ParagraphProcessor::beforeAddWord() {
	if (myWordCounter == 0) {
		myElements.push_back(TextElementPool::Pool.BeforeParagraphElement);
		if (myParagraph.kind() == Paragraph::TEXT_PARAGRAPH) {
			myElements.push_back(TextElementPool::Pool.IndentElement);
		} else if (myParagraph.kind() == Paragraph::TREE_PARAGRAPH) {
			TreeParagraph &tp = (TreeParagraph&)myParagraph;
			for (int i = 1; i < tp.depth() - 1; i++) {
				myElements.push_back(new TreeElement(TreeElement::TREE_ELEMENT_SKIP));
			}
			if (tp.depth() > 1) {
				TreeElement::TreeElementKind tek =
					(tp.parent()->children().back() == &tp) ?
						TreeElement::TREE_ELEMENT_TOP_RIGHT_LINE :
						TreeElement::TREE_ELEMENT_TOP_BOTTOM_RIGHT_LINE;
				myElements.push_back(new TreeElement(tek));
			}
			if (tp.children().empty()) {
				myElements.push_back(new TreeElement(TreeElement::TREE_ELEMENT_LEAF));
			} else if (tp.isOpen()) {
				myElements.push_back(new TreeElement(TreeElement::TREE_ELEMENT_OPEN_NODE));
			} else {
				myElements.push_back(new TreeElement(TreeElement::TREE_ELEMENT_CLOSED_NODE));
			}
		}
	}
	myWordCounter++;
}

void ParagraphCursor::ParagraphProcessor::addWord(const char *ptr, int offset, int len) {
	beforeAddWord();
	Word *word = TextElementPool::Pool.getWord(ptr, len, offset);
	for (std::vector<TextMark>::const_iterator mit = myFirstMark; mit != myLastMark; ++mit) {
		TextMark mark = *mit;
		if ((mark.Offset < offset + len) && (mark.Offset + mark.Length > offset)) {
			word->addMark(mark.Offset - offset, mark.Length);
		}
	}
	myElements.push_back(word);
}

void ParagraphCursor::ParagraphProcessor::fill() {
	for (Paragraph::Iterator it = myParagraph; !it.isEnd(); it.next()) {
		switch (it.entryKind()) {
			case ParagraphEntry::FORCED_CONTROL_ENTRY:
				myElements.push_back(new ForcedControlElement(it.entry()));
				break;
			case ParagraphEntry::CONTROL_ENTRY:
			case ParagraphEntry::HYPERLINK_CONTROL_ENTRY:
				myElements.push_back(TextElementPool::Pool.getControlElement(it.entry()));
				break;
			case ParagraphEntry::IMAGE_ENTRY:
			{
				beforeAddWord();
				const ZLImage *image = ((ImageEntry&)*it.entry()).image();
				if (image != 0) {
					shared_ptr<ZLImageData> data = ZLImageManager::instance().imageData(*image);
					if (!data.isNull()) {
						myElements.push_back(new ImageElement(data));
					}
				}
				break;
			}
			case ParagraphEntry::TEXT_ENTRY:
			{
				const TextEntry &textEntry = (TextEntry&)*it.entry();
				if (textEntry.dataLength() != 0) {
					const char *start = textEntry.data();
					const char *end = start + textEntry.dataLength();
					ZLUnicodeUtil::Ucs2Char ch;
					ZLUnicodeUtil::firstChar(ch, start);
					bool spaceInserted = false;
					if (ZLUnicodeUtil::isSpace(ch)) {
						myElements.push_back(TextElementPool::Pool.HSpaceElement);
						spaceInserted = true;
					}
					const char *firstNonSpace = 0;
					int charLength = 0;
					bool breakableBefore = false;
					for (const char *ptr = start; ptr < end; ptr += charLength) {
						if (breakableBefore) {
							if (firstNonSpace != 0) {
								addWord(firstNonSpace, myOffset + (firstNonSpace - textEntry.data()), ptr - firstNonSpace);
								firstNonSpace = 0;
								spaceInserted = false;
							}
							charLength = 0;
							breakableBefore = false;
							continue;
						}
						charLength = ZLUnicodeUtil::firstChar(ch, ptr);
						if (ZLUnicodeUtil::isSpace(ch)) {
							if (firstNonSpace != 0) {
								addWord(firstNonSpace, myOffset + (firstNonSpace - textEntry.data()), ptr - firstNonSpace);
								myElements.push_back(TextElementPool::Pool.HSpaceElement);
								spaceInserted = true;
								firstNonSpace = 0;
							} else if (!spaceInserted) {
								myElements.push_back(TextElementPool::Pool.HSpaceElement);
								spaceInserted = true;
							}
						} else if (firstNonSpace == 0) {
							firstNonSpace = ptr;
						} else if (myCheckBreakableCharacters) {
							switch (ZLUnicodeUtil::isBreakable(ch)) {
								case ZLUnicodeUtil::NO_BREAKABLE:
									break;
								case ZLUnicodeUtil::BREAKABLE_BEFORE:
									addWord(firstNonSpace, myOffset + (firstNonSpace - textEntry.data()), ptr - firstNonSpace);
									firstNonSpace = ptr;
									break;
								case ZLUnicodeUtil::BREAKABLE_AFTER:
									breakableBefore = true;
									break;
							}
						}
					}
					if (firstNonSpace != 0) {
						addWord(firstNonSpace, myOffset + (firstNonSpace - textEntry.data()), end - firstNonSpace);
					}
					myOffset += textEntry.dataLength();
				}
				break;
			}
		}
	}
	myElements.push_back(TextElementPool::Pool.AfterParagraphElement);
}
