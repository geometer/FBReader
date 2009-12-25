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
#include "ZLTextPositionIndicator.h"
#include "../area/ZLTextLineInfo.h"
#include "../area/ZLTextAreaStyle.h"

void ZLTextView::paint() {
	preparePaintInfo();

	myTextArea.myTextElementMap.clear();
	myTextArea.myTreeNodeMap.clear();
	myTextArea.context().clear(backgroundColor());

	if (textArea().isEmpty()) {
		return;
	}

	std::vector<size_t> labels;
	labels.reserve(myTextArea.myLineInfos.size() + 1);
	labels.push_back(0);

	ZLTextArea::Style style(textArea(), baseStyle());

	int y = topMargin();
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		prepareTextLine(style, info, y);
		y += info.Height + info.Descent + info.VSpaceAfter;
		labels.push_back(myTextArea.myTextElementMap.size());
	}

	mySelectionModel.update();

	y = topMargin();
	int index = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		drawTextLine(style, info, y, labels[index], labels[index + 1]);
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
		const size_t from = positionIndicator()->sizeOfTextBeforeCursor(textArea().startCursor());
		const size_t to = positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor());

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

int ZLTextView::rectangleBound(ZLTextArea::Style &style, const ZLTextParagraphCursor &paragraph, const ZLTextElementRectangle &rectangle, int toCharIndex, bool mainDir) {
	style.setTextStyle(rectangle.Style, rectangle.BidiLevel);
	const ZLTextWord &word = (const ZLTextWord&)paragraph[rectangle.ElementIndex];
	int length = toCharIndex - rectangle.StartCharIndex;
	bool selectHyphenationSign = false;
	if (length >= rectangle.Length) {
		selectHyphenationSign = rectangle.AddHyphenationSign;
		length = rectangle.Length;
	}
	int rectangleLen = (length > 0) ?
		style.wordWidth(word, rectangle.StartCharIndex, length, selectHyphenationSign) : 0;
	return mainDir ? rectangle.XStart + rectangleLen : rectangle.XEnd - rectangleLen;
}

typedef std::vector<ZLTextSelectionModel::Range> RangeVector;

bool contains(const ZLTextSelectionModel::Range &range, const ZLTextElementRectangle &rectangle) {
	return
		((range.first.ParagraphIndex < rectangle.ParagraphIndex) ||
		 ((range.first.ParagraphIndex == rectangle.ParagraphIndex) &&
			(range.first.ElementIndex <= rectangle.ElementIndex))) &&
		((range.second.ParagraphIndex > rectangle.ParagraphIndex) ||
		 ((range.second.ParagraphIndex == rectangle.ParagraphIndex) &&
			(range.second.ElementIndex >= rectangle.ElementIndex)));
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

static RangeVector::const_iterator findRange(const RangeVector &ranges, const ZLTextElementRectangle &rectangle) {
	// TODO: binary search
	for (RangeVector::const_iterator it = ranges.begin(); it != ranges.end(); ++it) {
		if (contains(*it, rectangle)) {
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
		myTextArea.context().setFillColor(color(ZLTextStyle::SELECTION_BACKGROUND));
		myTextArea.context().fillRectangle(left, top, right, bottom);
	}
}

void ZLTextView::drawTextLine(ZLTextArea::Style &style, const ZLTextLineInfo &info, int y, size_t from, size_t to) {
	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();

	const ZLTextElementIterator fromIt = myTextArea.myTextElementMap.begin() + from;
	const ZLTextElementIterator toIt = myTextArea.myTextElementMap.begin() + to;

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
			const int baseRTL = myTextArea.isRtl() ? 1 : 0;

			for (ZLTextElementIterator it = fromIt; it < toIt; ++it) {
				const ZLTextElementRectangle &rectangle = *it;
				RangeVector::const_iterator rt2 = findRange(ranges, rectangle);
				if (rt2 == rt) {
					if (rt != ranges.end()) {
						const bool mainDir = rectangle.BidiLevel % 2 == baseRTL;
						int r = rectangle.XEnd;
						const ZLTextSelectionModel::BoundElement &bound =
							mainDir ? rt->second : rt->first;
						if (bound.ElementIndex == rectangle.ElementIndex) {
							const ZLTextElement &element = paragraph[rectangle.ElementIndex];
							if (element.kind() == ZLTextElement::WORD_ELEMENT) {
								r = rectangleBound(style, paragraph, rectangle, bound.CharIndex, mainDir);
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

						const bool mainDir = rectangle.BidiLevel % 2 == baseRTL;

						int l = rectangle.XStart - 1;
						int r = rectangle.XEnd;

						const ZLTextSelectionModel::BoundElement &rightBound =
							mainDir ? rt->second : rt->first;
						const ZLTextSelectionModel::BoundElement &leftBound =
							mainDir ? rt->first : rt->second;
						if (paragraph[rectangle.ElementIndex].kind() == ZLTextElement::WORD_ELEMENT) {
							if (rightBound.ElementIndex == rectangle.ElementIndex) {
								r = rectangleBound(style, paragraph, rectangle, rightBound.CharIndex, mainDir);
							}
							if (leftBound.ElementIndex == rectangle.ElementIndex) {
								l = rectangleBound(style, paragraph, rectangle, leftBound.CharIndex, mainDir);
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

	y = std::min(y + info.Height, topMargin() + textHeight());
	int x = lineStartMargin();
	if (!info.NodeInfo.isNull()) {
		myTextArea.drawTreeLines(*info.NodeInfo, x, y, info.Height, info.Descent + info.VSpaceAfter);
	}
	ZLTextElementIterator it = fromIt;
	const int endElementIndex = info.End.elementIndex();
	for (; (it != toIt) && (it->ElementIndex != endElementIndex); ++it) {
		const ZLTextElement &element = paragraph[it->ElementIndex];
		ZLTextElement::Kind kind = element.kind();
	
		if ((kind == ZLTextElement::WORD_ELEMENT) || (kind == ZLTextElement::IMAGE_ELEMENT)) {
			style.setTextStyle(it->Style, it->BidiLevel);
			const int wx = it->XStart;
			const int wy = it->YEnd - style.elementDescent(element) - style.textStyle()->verticalShift();
			if (kind == ZLTextElement::WORD_ELEMENT) {
				myTextArea.drawWord(style, wx, wy, (const ZLTextWord&)element, it->StartCharIndex, -1, false);
			} else {
				myTextArea.context().drawImage(wx, wy, *((const ZLTextImageElement&)element).image(), viewWidth(), textHeight(), ZLPaintContext::SCALE_REDUCE_SIZE);
			}
		}
	}
	if (it != toIt) {
		style.setTextStyle(it->Style, it->BidiLevel);
		int start = 0;
		if (info.Start.equalElementIndex(info.End)) {
			start = info.Start.charIndex();
		}
		int len = info.End.charIndex() - start;
		const ZLTextWord &word = (const ZLTextWord&)info.End.element();
		context().setColor(color(style.textStyle()->colorStyle()));
		const int x = it->XStart;
		const int y = it->YEnd - style.elementDescent(word) - style.textStyle()->verticalShift();
		myTextArea.drawWord(style, x, y, word, start, len, it->AddHyphenationSign);
	}
}

void ZLTextView::addRectangleToTextMap(ZLTextArea::Style &style, const ZLTextElementRectangle &rectangle) {
	const unsigned char index = style.bidiLevel() - (myTextArea.isRtl() ? 1 : 0);
	if (index > 0) {
		while (myTextArea.myTextElementsToRevert.size() < index) {
			myTextArea.myTextElementsToRevert.push_back(ZLTextElementMap());
		}
		myTextArea.myTextElementsToRevert[index - 1].push_back(rectangle);
	} else {
		myTextArea.myTextElementMap.push_back(rectangle);
	}
}

void ZLTextView::flushRevertedElements(unsigned char bidiLevel) {
	const int index = (int)bidiLevel - (myTextArea.isRtl() ? 1 : 0);
	if ((index < 0) || (myTextArea.myTextElementsToRevert.size() <= (size_t)index)) {
		return;
	}
	ZLTextElementMap &from =
		myTextArea.myTextElementsToRevert[index];
	ZLTextElementMap &to = (index > 0) ?
		myTextArea.myTextElementsToRevert[index - 1] :
		myTextArea.myTextElementMap;
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

void ZLTextView::prepareTextLine(ZLTextArea::Style &style, const ZLTextLineInfo &info, int y) {
	y = std::min(y + info.Height, topMargin() + textHeight());

	style.setTextStyle(info.StartStyle, info.StartBidiLevel);
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	bool wordOccured = false;

	int x = lineStartMargin() + info.StartIndent;

	const int fontSize = style.textStyle()->fontSize();
	// TODO: change metrics at font change
	const ZLTextStyleEntry::Metrics metrics(fontSize, fontSize / 2, viewWidth(), textHeight());

	switch (style.textStyle()->alignment()) {
		case ALIGN_RIGHT:
			if (!myTextArea.isRtl()) {
				x += metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, false) - info.Width;
			}
			break;
		case ALIGN_LEFT:
			if (myTextArea.isRtl()) {
				x += metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, true) - info.Width;
			}
			break;
		case ALIGN_LINESTART:
			break;
		case ALIGN_CENTER:
			x += (metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, myTextArea.isRtl()) - info.Width) / 2;
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (info.End.element().kind() != ZLTextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, myTextArea.isRtl()) - info.Width;
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
		int width = style.elementWidth(element, pos.charIndex(), metrics);
	
		style.applySingleControl(element);
		switch (kind) {
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
			{
				const int height = style.elementHeight(element, metrics);
				const int descent = style.elementDescent(element);
				const int length = (kind == ZLTextElement::WORD_ELEMENT) ? ((const ZLTextWord&)element).Length - pos.charIndex() : 0;
				addRectangleToTextMap(
					style,
					ZLTextElementRectangle(
						paragraphIndex, pos.elementIndex(), pos.charIndex(), length, false,
						style.textStyle(), kind,
						x, x + width - 1, y - height + 1, y + descent, style.bidiLevel()
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
					x += myTextArea.context().spaceWidth() + correction;
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
				//myTextArea.context().setColor(ZLColor(0, 255, 0));
				//myTextArea.context().drawLine(x, y, x, y - 20);
				break;
			case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
				flushRevertedElements(style.bidiLevel());
				//myTextArea.context().setColor(ZLColor(255, 0, 0));
				//myTextArea.context().drawLine(x, y, x, y - 20);
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
			const int width = style.wordWidth(word, start, len, addHyphenationSign);
			const int height = style.elementHeight(word, metrics);
			const int descent = style.elementDescent(word);
			addRectangleToTextMap(
				style,
				ZLTextElementRectangle(
					paragraphIndex, info.End.elementIndex(), start, len, addHyphenationSign,
					style.textStyle(), ZLTextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent, word.BidiLevel
				)
			);
		}
	}

	unsigned char minBidiLevel = myTextArea.isRtl() ? 1 : 0;
	for (unsigned char i = style.bidiLevel(); i > minBidiLevel; --i) {
		flushRevertedElements(i - 1);
	}
}
