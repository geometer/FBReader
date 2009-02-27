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

#include <ZLUnicodeUtil.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"

void ZLTextView::paint() {
	preparePaintInfo();

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	context().clear(ZLTextStyleCollection::instance().baseStyle().BackgroundColorOption.value());

	if (empty()) {
		return;
	}

	std::vector<size_t> labels;
	labels.reserve(myLineInfos.size() + 1);
	labels.push_back(0);

	int y = topMargin();
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		prepareTextLine(info, y);
		y += info.Height + info.Descent + info.VSpaceAfter;
		labels.push_back(myTextElementMap.size());
	}

	mySelectionModel.update();

	y = topMargin();
	int index = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		drawTextLine(info, y, labels[index], labels[index + 1]);
		y += info.Height + info.Descent + info.VSpaceAfter;
		++index;
	}

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		positionIndicator()->draw();
	}

	if (myDoUpdateScrollbar && !indicatorInfo.isNull()) {
		myDoUpdateScrollbar = false;
		const size_t full = positionIndicator()->sizeOfTextBeforeParagraph(positionIndicator()->endTextIndex());
		const size_t from = positionIndicator()->sizeOfTextBeforeCursor(startCursor());
		const size_t to = positionIndicator()->sizeOfTextBeforeCursor(endCursor());

		bool showScrollbar =
			(indicatorInfo->type() == ZLTextPositionIndicatorInfo::OS_SCROLLBAR) &&
			(to - from < full);
		if (showScrollbar) {
			setScrollbarEnabled(VERTICAL, true);
			setScrollbarParameters(VERTICAL, full, from, to);
		} else {
			setScrollbarEnabled(VERTICAL, false);
		}
	}

	ZLTextParagraphCursorCache::cleanup();
}

int ZLTextView::areaBound(const ZLTextParagraphCursor &paragraph, const ZLTextElementArea &area, int toCharIndex, bool mainDir) {
	myStyle.setTextStyle(area.Style, area.BidiLevel);
	const ZLTextWord &word = (const ZLTextWord&)paragraph[area.ElementIndex];
	int length = toCharIndex - area.StartCharIndex;
	bool selectHyphenationSign = false;
	if (length >= area.Length) {
		selectHyphenationSign = area.AddHyphenationSign;
		length = area.Length;
	}
	int areaLen = (length > 0) ?
		myStyle.wordWidth(word, area.StartCharIndex, length, selectHyphenationSign) : 0;
	return mainDir ? area.XStart + areaLen : area.XEnd - areaLen;
}

typedef std::vector<ZLTextSelectionModel::Range> RangeVector;

bool contains(const ZLTextSelectionModel::Range &range, const ZLTextElementArea &area) {
	return
		((range.first.ParagraphIndex < area.ParagraphIndex) ||
		 ((range.first.ParagraphIndex == area.ParagraphIndex) &&
			(range.first.ElementIndex <= area.ElementIndex))) &&
		((range.second.ParagraphIndex > area.ParagraphIndex) ||
		 ((range.second.ParagraphIndex == area.ParagraphIndex) &&
			(range.second.ElementIndex >= area.ElementIndex)));
}

bool strongContains(const ZLTextSelectionModel::Range &range, const ZLTextWordCursor &cursor) {
	const int pn = cursor.paragraphCursor().index();
	const int wn = cursor.elementIndex();
	return
		((range.first.ParagraphIndex < pn) ||
		 ((range.first.ParagraphIndex == pn) &&
			(range.first.ElementIndex < wn))) &&
		((range.second.ParagraphIndex > pn) ||
		 ((range.second.ParagraphIndex == pn) &&
			(range.second.ElementIndex > wn)));
}

static RangeVector::const_iterator findRange(const RangeVector &ranges, const ZLTextElementArea &area) {
	// TODO: binary search
	for (RangeVector::const_iterator it = ranges.begin(); it != ranges.end(); ++it) {
		if (contains(*it, area)) {
			return it;
		}
	}
	return ranges.end();
}

static RangeVector::const_iterator strongFindRange(const RangeVector &ranges, const ZLTextWordCursor &cursor) {
	// TODO: binary search
	for (RangeVector::const_iterator it = ranges.begin(); it != ranges.end(); ++it) {
		if (strongContains(*it, cursor)) {
			return it;
		}
	}
	return ranges.end();
}

void ZLTextView::drawSelectionRectangle(int left, int top, int right, int bottom) {
	left = std::max(left, lineStartMargin());
	right = std::min(right, viewWidth() + lineStartMargin() - 1);
	if (left < right) {
		context().setFillColor(ZLTextStyleCollection::instance().baseStyle().SelectionBackgroundColorOption.value());
		context().fillRectangle(visualX(left), top, visualX(right), bottom);
	}
}

void ZLTextView::drawTextLine(const ZLTextLineInfo &info, int y, size_t from, size_t to) {
	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();

	const ZLTextElementIterator fromIt = myTextElementMap.begin() + from;
	const ZLTextElementIterator toIt = myTextElementMap.begin() + to;

	if (!mySelectionModel.isEmpty() && (from != to)) {
		const std::vector<ZLTextSelectionModel::Range> &ranges = mySelectionModel.ranges();

		if (!ranges.empty()) {
			RangeVector::const_iterator rt = ranges.end();
			const int top = y + 1;
			int bottom = y + info.Height + info.Descent;
			if (strongFindRange(ranges, info.End) != ranges.end()) {
				bottom += info.VSpaceAfter;
			}
			int left = viewWidth() + lineStartMargin() - 1;
			int right = lineStartMargin();
			const int baseRTL = myStyle.baseBidiLevel() % 2;

			for (ZLTextElementIterator it = fromIt; it < toIt; ++it) {
				const ZLTextElementArea &area = *it;
				RangeVector::const_iterator rt2 = findRange(ranges, area);
				if (rt2 == rt) {
					if (rt != ranges.end()) {
						const bool mainDir = area.BidiLevel % 2 == baseRTL;
						int r = area.XEnd;
						const ZLTextSelectionModel::BoundElement &bound =
							mainDir ? rt->second : rt->first;
						if (bound.ElementIndex == area.ElementIndex) {
							const ZLTextElement &element = paragraph[area.ElementIndex];
							if (element.kind() == ZLTextElement::WORD_ELEMENT) {
								r = areaBound(paragraph, area, bound.CharIndex, mainDir);
							}
						}
						right = std::max(right, r);
					}
				} else {
					if (rt != ranges.end()) {
						drawSelectionRectangle(left, top, right, bottom);
						left = viewWidth() + lineStartMargin() - 1;
						right = lineStartMargin();
					}
					rt = rt2;
					if (rt != ranges.end()) {
						if ((it == fromIt) &&
								(info.StartBidiLevel % 2 == baseRTL) &&
								strongContains(*rt, info.Start)) {
							left = lineStartMargin();
						}

						const bool mainDir = area.BidiLevel % 2 == baseRTL;

						int l = area.XStart - 1;
						int r = area.XEnd;

						const ZLTextSelectionModel::BoundElement &rightBound =
							mainDir ? rt->second : rt->first;
						const ZLTextSelectionModel::BoundElement &leftBound =
							mainDir ? rt->first : rt->second;
						if (paragraph[area.ElementIndex].kind() == ZLTextElement::WORD_ELEMENT) {
							if (rightBound.ElementIndex == area.ElementIndex) {
								r = areaBound(paragraph, area, rightBound.CharIndex, mainDir);
							}
							if (leftBound.ElementIndex == area.ElementIndex) {
								l = areaBound(paragraph, area, leftBound.CharIndex, mainDir);
							}
						}

						left = std::min(left, l);
						right = std::max(right, r);
					}
				}
			}
			if (rt != ranges.end()) {
				if ((paragraph.index() < (size_t)rt->second.ParagraphIndex) &&
						strongContains(*rt, info.End)) {
					right = viewWidth() + lineStartMargin() - 1;
				}
				drawSelectionRectangle(left, top, right, bottom);
			}
		}
	}

	y = std::min(y + info.Height, topMargin() + textAreaHeight());
	int x = lineStartMargin();
	if (!info.NodeInfo.isNull()) {
		drawTreeLines(*info.NodeInfo, x, y, info.Height, info.Descent + info.VSpaceAfter);
	}
	ZLTextElementIterator it = fromIt;
	const int endElementIndex = info.End.elementIndex();
	for (; (it != toIt) && (it->ElementIndex != endElementIndex); ++it) {
		const ZLTextElement &element = paragraph[it->ElementIndex];
		ZLTextElement::Kind kind = element.kind();
	
		if ((kind == ZLTextElement::WORD_ELEMENT) || (kind == ZLTextElement::IMAGE_ELEMENT)) {
			myStyle.setTextStyle(it->Style, it->BidiLevel);
			const int wx = myStyle.baseIsRtl() ? context().width() - it->XEnd : it->XStart;
			const int wy = it->YEnd - myStyle.elementDescent(element) - myStyle.textStyle()->verticalShift();
			if (kind == ZLTextElement::WORD_ELEMENT) {
				drawWord(wx, wy, (const ZLTextWord&)element, it->StartCharIndex, -1, false);
			} else {
				context().drawImage(wx, wy, *((const ZLTextImageElement&)element).image());
			}
		}
	}
	if (it != toIt) {
		myStyle.setTextStyle(it->Style, it->BidiLevel);
		int start = 0;
		if (info.Start.equalElementIndex(info.End)) {
			start = info.Start.charIndex();
		}
		int len = info.End.charIndex() - start;
		const ZLTextWord &word = (const ZLTextWord&)info.End.element();
		context().setColor(myStyle.textStyle()->color());
		const int x = myStyle.baseIsRtl() ? context().width() - it->XEnd : it->XStart;
		const int y = it->YEnd - myStyle.elementDescent(word) - myStyle.textStyle()->verticalShift();
		drawWord(x, y, word, start, len, it->AddHyphenationSign);
	}
}

void ZLTextView::addAreaToTextMap(const ZLTextElementArea &area) {
	const unsigned char index = myStyle.bidiLevel() - myStyle.baseBidiLevel();
	if (index > 0) {
		while (myTextElementsToRevert.size() < index) {
			myTextElementsToRevert.push_back(ZLTextElementMap());
		}
		myTextElementsToRevert[index - 1].push_back(area);
	} else {
		myTextElementMap.push_back(area);
	}
}

void ZLTextView::flushRevertedElements(unsigned char bidiLevel) {
	const int index = (int)bidiLevel - (int)myStyle.baseBidiLevel();
	if ((index < 0) || (myTextElementsToRevert.size() <= (size_t)index)) {
		return;
	}
	ZLTextElementMap &from =
		myTextElementsToRevert[index];
	ZLTextElementMap &to = (index > 0) ?
		myTextElementsToRevert[index - 1] :
		myTextElementMap;
	if (!from.empty()) {
		const int sum = from[from.size() - 1].XEnd + from[0].XStart;
		for (ZLTextElementMap::reverse_iterator it = from.rbegin(); it != from.rend(); ++it) {
			int tmp = sum - it->XStart;
			it->XStart = sum - it->XEnd;
			it->XEnd = tmp;
			to.push_back(*it);
		}
		from.clear();
	}
}

void ZLTextView::prepareTextLine(const ZLTextLineInfo &info, int y) {
	y = std::min(y + info.Height, topMargin() + textAreaHeight());

	myStyle.setTextStyle(info.StartStyle, info.StartBidiLevel);
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	bool wordOccured = false;

	int x = lineStartMargin() + info.StartIndent;

	const int fontSize = myStyle.textStyle()->fontSize();
	// TODO: change metrics at font change
	const ZLTextStyleEntry::Metrics metrics(fontSize, fontSize / 2, viewWidth(), textAreaHeight());

	switch (myStyle.textStyle()->alignment()) {
		case ALIGN_RIGHT:
			if (!myStyle.baseIsRtl()) {
				x += metrics.FullWidth - myStyle.textStyle()->lineEndIndent(metrics, myStyle.baseIsRtl()) - info.Width;
			}
			break;
		case ALIGN_LEFT:
			if (myStyle.baseIsRtl()) {
				x += metrics.FullWidth - myStyle.textStyle()->lineEndIndent(metrics, myStyle.baseIsRtl()) - info.Width;
			}
			break;
		case ALIGN_CENTER:
			x += (metrics.FullWidth - myStyle.textStyle()->lineEndIndent(metrics, myStyle.baseIsRtl()) - info.Width) / 2;
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (info.End.element().kind() != ZLTextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = metrics.FullWidth - myStyle.textStyle()->lineEndIndent(metrics, myStyle.baseIsRtl()) - info.Width;
			}
			break;
		case ALIGN_UNDEFINED:
			break;
	}

	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();
	int paragraphIndex = paragraph.index();
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalElementIndex(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.elementIndex()];
		ZLTextElement::Kind kind = element.kind();
		int width = myStyle.elementWidth(element, pos.charIndex(), metrics);
	
		myStyle.applySingleControl(element);
		switch (kind) {
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
			{
				const int height = myStyle.elementHeight(element, metrics);
				const int descent = myStyle.elementDescent(element);
				const int length = (kind == ZLTextElement::WORD_ELEMENT) ? ((const ZLTextWord&)element).Length - pos.charIndex() : 0;
				addAreaToTextMap(
					ZLTextElementArea(
						paragraphIndex, pos.elementIndex(), pos.charIndex(), length, false,
						myStyle.textStyle(), kind,
						x, x + width - 1, y - height + 1, y + descent, myStyle.bidiLevel()
					)
				);
				wordOccured = true;
				break;
			}
			case ZLTextElement::CONTROL_ELEMENT:
			case ZLTextElement::FORCED_CONTROL_ELEMENT:
				break;
			case ZLTextElement::HSPACE_ELEMENT:
			case ZLTextElement::NB_HSPACE_ELEMENT:
				if (wordOccured && (spaceCounter > 0)) {
					int correction = fullCorrection / spaceCounter;
					x += context().spaceWidth() + correction;
					fullCorrection -= correction;
					wordOccured = false;
					--spaceCounter;
				}
				break;
			case ZLTextElement::INDENT_ELEMENT:
			case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
			case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
			case ZLTextElement::EMPTY_LINE_ELEMENT:
			case ZLTextElement::FIXED_HSPACE_ELEMENT:
				break;
			case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
				//context().setColor(ZLColor(0, 255, 0));
				//context().drawLine(visualX(x), y, visualX(x), y - 20);
				break;
			case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
				flushRevertedElements(myStyle.bidiLevel());
				//context().setColor(ZLColor(255, 0, 0));
				//context().drawLine(visualX(x), y, visualX(x), y - 20);
				break;
		}

		x += width;
	}
	if (!endOfParagraph && (info.End.element().kind() == ZLTextElement::WORD_ELEMENT)) {
		int start = 0;
		if (info.End.equalElementIndex(info.RealStart)) {
			start = info.RealStart.charIndex();
		}
		const int len = info.End.charIndex() - start;
		if (len > 0) {
			const ZLTextWord &word = (const ZLTextWord&)info.End.element();
			ZLUnicodeUtil::Ucs4String ucs4string;
			ZLUnicodeUtil::utf8ToUcs4(ucs4string, word.Data, word.Size);
			const bool addHyphenationSign = ucs4string[start + len - 1] != '-';
			const int width = myStyle.wordWidth(word, start, len, addHyphenationSign);
			const int height = myStyle.elementHeight(word, metrics);
			const int descent = myStyle.elementDescent(word);
			addAreaToTextMap(
				ZLTextElementArea(
					paragraphIndex, info.End.elementIndex(), start, len, addHyphenationSign,
					myStyle.textStyle(), ZLTextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent, word.BidiLevel
				)
			);
		}
	}

	for (unsigned char i = myStyle.bidiLevel(); i > myStyle.baseBidiLevel(); --i) {
		flushRevertedElements(i - 1);
	}
}
