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
	myY = topMargin();
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		prepareTextLine(**it);
		labels.push_back(myTextElementMap.size());
	}
	mySelectionModel.update();
	myY = topMargin();
	int index = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		drawTextLine(**it, labels[index], labels[index + 1]);
		++index;
	}

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && indicatorInfo->isVisible()) {
		positionIndicator()->draw();
	}

	ZLTextParagraphCursorCache::cleanup();
}

static bool operator <= (const ZLTextElementArea &area, const ZLTextSelectionModel::BoundElement &element) {
	return
		(area.ParagraphNumber < element.ParagraphNumber) ||
		((area.ParagraphNumber == element.ParagraphNumber) &&
		 (area.TextElementNumber <= element.TextElementNumber));
}

static bool operator > (const ZLTextElementArea &area, const ZLTextSelectionModel::BoundElement &element) {
	return !(area <= element);
}

static bool operator < (const ZLTextWordCursor &cursor, const ZLTextSelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn < element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() < element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() < element.CharNumber))));
}

static bool operator >= (const ZLTextWordCursor &cursor, const ZLTextSelectionModel::BoundElement &element) {
	return !(cursor < element);
}

static bool operator > (const ZLTextWordCursor &cursor, const ZLTextSelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn > element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() > element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() > element.CharNumber))));
}

static bool operator <= (const ZLTextWordCursor &cursor, const ZLTextSelectionModel::BoundElement &element) {
	return !(cursor > element);
}

static ZLTextElementIterator findLast(ZLTextElementIterator from, ZLTextElementIterator to, const ZLTextSelectionModel::BoundElement &bound) {
	if (*from > bound) {
		return from;
	}
	for (++from; (from != to) && (*from <= bound); ++from) {
	}
	return --from;
}

int ZLTextView::areaLength(const ZLTextParagraphCursor &paragraph, const ZLTextElementArea &area, int toCharNumber) {
	myStyle.setTextStyle(area.Style);
	const ZLTextWord &word = (const ZLTextWord&)paragraph[area.TextElementNumber];
	int length = toCharNumber - area.StartCharNumber;
	bool selectHyphenationSign = false;
	if (length >= area.Length) {
		selectHyphenationSign = area.AddHyphenationSign;
		length = area.Length;
	}
	if (length > 0) {
		return myStyle.wordWidth(word, area.StartCharNumber, length, selectHyphenationSign);
	}
	return 0;
}

void ZLTextView::drawTextLine(const ZLTextLineInfo &info, size_t from, size_t to) {
	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();

	const ZLTextElementIterator fromIt = myTextElementMap.begin() + from;
	const ZLTextElementIterator toIt = myTextElementMap.begin() + to;

	if (!mySelectionModel.isEmpty() && (from != to)) {
		std::pair<ZLTextSelectionModel::BoundElement,ZLTextSelectionModel::BoundElement> range = mySelectionModel.range();

		int left = viewWidth() + leftMargin() - 1;
		if (info.Start > range.first) {
			left = leftMargin();
		} else if (info.End >= range.first) {
			ZLTextElementIterator jt = findLast(fromIt, toIt, range.first);
			left = jt->XStart;
			if (jt->Kind == ZLTextElement::WORD_ELEMENT) {
				left += areaLength(paragraph, *jt, range.first.CharNumber);
			}
		}

		const int top = myY + 1;
		int bottom = myY + info.Height + info.Descent;
		int right = leftMargin();
		if (info.End < range.second) {
			right = viewWidth() + leftMargin() - 1;
			bottom += info.VSpaceAfter;
		} else if (info.Start <= range.second) {
			ZLTextElementIterator jt = findLast(fromIt, toIt, range.second);
			if (jt->Kind == ZLTextElement::WORD_ELEMENT) {
				right = jt->XStart + areaLength(paragraph, *jt, range.second.CharNumber) - 1;
			} else {
				right = jt->XEnd - 1;
			}
		}

		if (left < right) {
			context().setFillColor(ZLTextStyleCollection::instance().baseStyle().SelectionBackgroundColorOption.value());
			context().fillRectangle(left, top, right, bottom);
		}
	}

	myY += info.Height;
	int maxY = topMargin() + textAreaHeight();
	if (myY > maxY) {
	  myY = maxY;
	}
	myX = leftMargin();
	if (!info.NodeInfo.isNull()) {
		drawTreeLines(*info.NodeInfo, info.Height, info.Descent + info.VSpaceAfter);
	}
	ZLTextElementIterator it = fromIt;
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalWordNumber(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.wordNumber()];
		ZLTextElement::Kind kind = element.kind();
	
		if ((kind == ZLTextElement::WORD_ELEMENT) || (kind == ZLTextElement::IMAGE_ELEMENT)) {
			if (it->ChangeStyle) {
				myStyle.setTextStyle(it->Style);
			}
			const int x = it->XStart;
			const int y = it->YEnd - myStyle.elementDescent(element) - myStyle.textStyle()->verticalShift();
			if (kind == ZLTextElement::WORD_ELEMENT) {
				drawWord(x, y, (const ZLTextWord&)element, pos.charNumber(), -1, false);
			} else {
				context().drawImage(x, y, ((const ZLTextImageElement&)element).image());
			}
			++it;
		}
	}
	if (it != toIt) {
		if (it->ChangeStyle) {
			myStyle.setTextStyle(it->Style);
		}
		int start = 0;
		if (info.Start.equalWordNumber(info.End)) {
			start = info.Start.charNumber();
		}
		int len = info.End.charNumber() - start;
		const ZLTextWord &word = (const ZLTextWord&)info.End.element();
		context().setColor(myStyle.textStyle()->color());
		const int x = it->XStart;
		const int y = it->YEnd - myStyle.elementDescent(word) - myStyle.textStyle()->verticalShift();
		drawWord(x, y, word, start, len, it->AddHyphenationSign);
	}
	myY += info.Descent + info.VSpaceAfter;
}

void ZLTextView::prepareTextLine(const ZLTextLineInfo &info) {
	myStyle.setTextStyle(info.StartStyle);
	const int y = std::min(myY + info.Height, topMargin() + textAreaHeight());
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	bool wordOccured = false;
	bool changeStyle = true;

	myX = leftMargin() + info.LeftIndent;

	const int fontSize = myStyle.textStyle()->fontSize();
	// TODO: change metrics at font change
	const ZLTextStyleEntry::Metrics metrics(fontSize, fontSize / 2, viewWidth(), textAreaHeight());

	switch (myStyle.textStyle()->alignment()) {
		case ALIGN_RIGHT:
			myX += metrics.FullWidth - myStyle.textStyle()->rightIndent(metrics) - info.Width;
			break;
		case ALIGN_CENTER:
			myX += (metrics.FullWidth - myStyle.textStyle()->rightIndent(metrics) - info.Width) / 2;
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (info.End.element().kind() != ZLTextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = metrics.FullWidth - myStyle.textStyle()->rightIndent(metrics) - info.Width;
			}
			break;
		case ALIGN_LEFT:
		case ALIGN_UNDEFINED:
			break;
	}

	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();
	int paragraphNumber = paragraph.index();
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalWordNumber(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.wordNumber()];
		ZLTextElement::Kind kind = element.kind();
		const int x = myX;
		int width = myStyle.elementWidth(element, pos.charNumber(), metrics);
	
		switch (kind) {
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
			{
				const int height = myStyle.elementHeight(element, metrics);
				const int descent = myStyle.elementDescent(element);
				const int length = (kind == ZLTextElement::WORD_ELEMENT) ? ((const ZLTextWord&)element).Length - pos.charNumber() : 0;
				myTextElementMap.push_back(
					ZLTextElementArea(
						paragraphNumber, pos.wordNumber(), pos.charNumber(), length, false,
						changeStyle, myStyle.textStyle(), kind,
						x, x + width - 1, y - height + 1, y + descent
					)
				);
				changeStyle = false;
				wordOccured = true;
				break;
			}
			case ZLTextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextControlElement&)element);
				changeStyle = true;
				break;
			case ZLTextElement::FORCED_CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextStyleElement&)element);
				changeStyle = true;
				break;
			case ZLTextElement::HSPACE_ELEMENT:
			case ZLTextElement::NB_HSPACE_ELEMENT:
				if (wordOccured && (spaceCounter > 0)) {
					int correction = fullCorrection / spaceCounter;
					myX += context().spaceWidth() + correction;
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
		}

		myX += width;
	}
	if (!endOfParagraph && (info.End.element().kind() == ZLTextElement::WORD_ELEMENT)) {
		int start = 0;
		if (info.End.equalWordNumber(info.RealStart)) {
			start = info.RealStart.charNumber();
		}
		const int len = info.End.charNumber() - start;
		if (len > 0) {
			const ZLTextWord &word = (const ZLTextWord&)info.End.element();
			ZLUnicodeUtil::Ucs4String ucs4string;
			ZLUnicodeUtil::utf8ToUcs4(ucs4string, word.Data, word.Size);
			const bool addHyphenationSign = ucs4string[start + len - 1] != '-';
			const int x = myX; 
			const int width = myStyle.wordWidth(word, start, len, addHyphenationSign);
			const int height = myStyle.elementHeight(word, metrics);
			const int descent = myStyle.elementDescent(word);
			myTextElementMap.push_back(
				ZLTextElementArea(
					paragraphNumber, info.End.wordNumber(), start, len, addHyphenationSign,
					changeStyle, myStyle.textStyle(), ZLTextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent
				)
			);
		}
	}

	myY += info.Height + info.Descent + info.VSpaceAfter;
}
