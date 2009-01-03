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

#include <ZLTime.h>
#include <ZLStringUtil.h>

#include "ZLTextView.h"
#include "ZLTextStyle.h"

/*
 * Calculates m0 * m1 / d
 *   We assume m0 <= d or m1 <= d so result is small enough for size_t
 */
static size_t muldiv(size_t m0, size_t m1, size_t d) {
	static const size_t HALF = 1 << sizeof(size_t) / 2;
	if ((m0 < HALF) && (m1 < HALF)) {
		return m0 * m1 / d;
	}

	if (m0 > d) {
		size_t swap = m0;
		m0 = m1;
		m1 = swap;
	}

	size_t result = 0;
	size_t remainder = 0;

	while ((m0 != 0) && (m1 != 0) && ((m0 >= HALF) || (m1 >= HALF))) {
		const size_t mul = (d - 1) / m0 + 1;
		const size_t rem = m0 * (m1 % mul);
		if (remainder + rem < remainder) {
			++result;
		}
		remainder += rem;
		m1 /= mul;
		result += m1;
		m0 = m0 * mul - d;
	}
	const size_t product = m0 * m1;
	result += product / d;
	if (remainder + product % d < remainder) {
		++result;
	}
	return result;
}

ZLTextPositionIndicatorInfo::ZLTextPositionIndicatorInfo() {
}

ZLTextPositionIndicatorInfo::~ZLTextPositionIndicatorInfo() {
}

ZLTextView::PositionIndicator::PositionIndicator(ZLTextView &textView, const ZLTextPositionIndicatorInfo &info) : myTextView(textView), myInfo(info), myExtraWidth(0) {
}

ZLTextView::PositionIndicator::~PositionIndicator() {
}

const ZLTextView &ZLTextView::PositionIndicator::textView() const {
	return myTextView;
}

ZLPaintContext &ZLTextView::PositionIndicator::context() const {
	return myTextView.context();
}

int ZLTextView::PositionIndicator::bottom() const {
	return context().height() - myTextView.bottomMargin() - 1;
}

int ZLTextView::PositionIndicator::top() const {
	return bottom() - myInfo.height() + 1;
}

int ZLTextView::PositionIndicator::left() const {
	return myTextView.lineStartMargin();
}

int ZLTextView::PositionIndicator::right() const {
	return myTextView.lineStartMargin() + myTextView.viewWidth() - myExtraWidth - 1;
}

const std::vector<size_t> &ZLTextView::PositionIndicator::textSize() const {
	return myTextView.myTextSize;
}

size_t ZLTextView::PositionIndicator::startTextIndex() const {
	std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
	return (i != myTextView.myTextBreaks.begin()) ? *(i - 1) + 1 : 0;
}

size_t ZLTextView::PositionIndicator::endTextIndex() const {
	std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
	return (i != myTextView.myTextBreaks.end()) ? *i : myTextView.myModel->paragraphsNumber();
}

void ZLTextView::PositionIndicator::drawExtraText(const std::string &text) {
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();

	context().setFont(baseStyle.fontFamily(), myInfo.fontSize(), false, false);
	context().setColor(baseStyle.RegularTextColorOption.value());

	int width = context().stringWidth(text.data(), text.length(), false);
	if (myTextView.visualX(0) == 0) {
		context().drawString(right() - width, bottom() - 2, text.data(), text.length(), false);
	} else {
		context().drawString(myTextView.visualX(right()), bottom() - 2, text.data(), text.length(), false);
	}
	myExtraWidth += text.length() * context().stringWidth("0", 1, false) + context().spaceWidth();
}

size_t ZLTextView::PositionIndicator::sizeOfTextBeforeParagraph(size_t paragraphIndex) const {
	if (myTextView.myModel->kind() == ZLTextModel::TREE_MODEL) {
		ZLTextWordCursor cursor = myTextView.startCursor();
		if (cursor.isNull()) {
			cursor = myTextView.endCursor();
		}
		if (!cursor.isNull()) {
			const ZLTextTreeModel &treeModel = (const ZLTextTreeModel&)*myTextView.myModel;
			size_t sum = 0;
			for (size_t i = 0; i < paragraphIndex; ++i) {
				const ZLTextTreeParagraph *para = (const ZLTextTreeParagraph*)treeModel[i];
				if (para->parent()->isOpen()) {
					sum += sizeOfParagraph(i);
				}
			}
			return sum;
		}
	}
	return myTextView.myTextSize[paragraphIndex] - myTextView.myTextSize[startTextIndex()];
}

size_t ZLTextView::PositionIndicator::sizeOfParagraph(size_t paragraphIndex) const {
	return myTextView.myTextSize[paragraphIndex + 1] - myTextView.myTextSize[paragraphIndex];
}

size_t ZLTextView::PositionIndicator::sizeOfTextBeforeCursor(const ZLTextWordCursor &cursor) const {
	const size_t paragraphIndex = cursor.paragraphCursor().index();
	const size_t paragraphLength = cursor.paragraphCursor().paragraphLength();

	if (paragraphLength == 0) {
		return sizeOfTextBeforeParagraph(paragraphIndex);
	} else {
		return
			sizeOfTextBeforeParagraph(paragraphIndex) +
			muldiv(sizeOfParagraph(paragraphIndex), cursor.elementIndex(), paragraphLength);
	}
}

std::string ZLTextView::PositionIndicator::textPositionString() const {
	std::string buffer;
	ZLStringUtil::appendNumber(buffer, 1 + sizeOfTextBeforeCursor(myTextView.endCursor()) / 2048);
	buffer += '/';
	ZLStringUtil::appendNumber(buffer, 1 + sizeOfTextBeforeParagraph(endTextIndex()) / 2048);

	return buffer;

	/*
	std::string buffer;

	const std::vector<size_t> &textSizeVector = myTextView.myTextSize;
	const size_t fullTextSize = textSizeVector[endTextIndex()] - textSizeVector[startTextIndex()];
	ZLStringUtil::appendNumber(buffer, 100 * sizeOfTextBeforeCursor(myTextView.endCursor()) / fullTextSize);

	return buffer + '%';
	*/
}

std::string ZLTextView::PositionIndicator::timeString() const {
	std::string buffer;
	ZLTime time;
	const short hours = time.hours();
	ZLStringUtil::appendNumber(buffer, hours / 10);
	ZLStringUtil::appendNumber(buffer, hours % 10);

	buffer += ':';

	const short minutes = time.minutes();
	ZLStringUtil::appendNumber(buffer, minutes / 10);
	ZLStringUtil::appendNumber(buffer, minutes % 10);

	return buffer;
}

void ZLTextView::PositionIndicator::draw() {
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();

	ZLPaintContext &context = this->context();

	ZLTextWordCursor endCursor = myTextView.endCursor();
	bool isEndOfText = false;
	if (endCursor.isEndOfParagraph()) {
		isEndOfText = !endCursor.nextParagraph();
	}

	myExtraWidth = 0;
	if (myInfo.isTimeShown()) {
		drawExtraText(timeString());
	}
	if (myInfo.isTextPositionShown()) {
		drawExtraText(textPositionString());
	}

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	if (left >= right) {
		return;
	}

	size_t fillWidth = right - left - 1;

	if (!isEndOfText) {
		fillWidth =
			muldiv(fillWidth, sizeOfTextBeforeCursor(myTextView.endCursor()), sizeOfTextBeforeParagraph(endTextIndex()));
	}

	context.setColor(baseStyle.RegularTextColorOption.value());
	context.setFillColor(myInfo.color());
	context.fillRectangle(myTextView.visualX(left + 1), top + 1, myTextView.visualX(left + fillWidth + 1), bottom - 1);
	context.drawLine(myTextView.visualX(left), top, myTextView.visualX(right), top);
	context.drawLine(myTextView.visualX(left), bottom, myTextView.visualX(right), bottom);
	context.drawLine(myTextView.visualX(left), bottom, myTextView.visualX(left), top);
	context.drawLine(myTextView.visualX(right), bottom, myTextView.visualX(right), top);
}

bool ZLTextView::PositionIndicator::onStylusPress(int x, int y) {
	x = myTextView.visualX(x);

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	if ((x < left) || (x > right) || (y < top) || (y > bottom)) {
		return false;
	}

	const std::vector<size_t> &textSizeVector = myTextView.myTextSize;
	if (textSizeVector.size() <= 1) {
		return true;
	}

	if (myTextView.endCursor().isNull()) {
		return false;
	}
	size_t fullTextSize = sizeOfTextBeforeParagraph(endTextIndex());
	size_t textSize = muldiv(fullTextSize, x - left + 1, right - left + 1);

	myTextView.gotoCharIndex(textSize);

	return true;
}

shared_ptr<ZLTextView::PositionIndicator> ZLTextView::createPositionIndicator(const ZLTextPositionIndicatorInfo &info) {
	return new PositionIndicator(*this, info);
}

shared_ptr<ZLTextView::PositionIndicator> ZLTextView::positionIndicator() {
	if (myPositionIndicator.isNull()) {
		shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
		if (!indicatorInfo.isNull()) {
			myPositionIndicator = createPositionIndicator(*indicatorInfo);
		}
	}
	return myPositionIndicator;
}
