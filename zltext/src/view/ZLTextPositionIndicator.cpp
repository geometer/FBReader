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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
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

static const std::string INDICATOR = "Indicator";

ZLTextPositionIndicatorStyle::ZLTextPositionIndicatorStyle() :
	ShowOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Show", true),
	IsSensitiveOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "TouchSensitive", true),
	ShowTextPositionOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "PositionText", true),
	ShowTimeOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Time", true),
	ColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Color", ZLColor(127, 127, 127)),
	HeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Height", 1, 100, 16),
	OffsetOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Offset", 0, 100, 3),
	FontSizeOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "FontSize", 4, 72, 14) {
}

TextView::PositionIndicator::PositionIndicator(TextView &textView) : myTextView(textView), myExtraWidth(0) {
}

TextView::PositionIndicator::~PositionIndicator() {
}

const TextView &TextView::PositionIndicator::textView() const {
	return myTextView;
}

ZLPaintContext &TextView::PositionIndicator::context() const {
	return myTextView.context();
}

int TextView::PositionIndicator::bottom() const {
	return context().height();
}

int TextView::PositionIndicator::top() const {
	return bottom() - ZLTextStyleCollection::instance().indicatorStyle().HeightOption.value() + 1;
}

int TextView::PositionIndicator::left() const {
	return 0;
}

int TextView::PositionIndicator::right() const {
	return context().width() - myExtraWidth - 1;
}

const std::vector<size_t> &TextView::PositionIndicator::textSize() const {
	return myTextView.myTextSize;
}

size_t TextView::PositionIndicator::startTextIndex() const {
	std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
	return (i != myTextView.myTextBreaks.begin()) ? *(i - 1) : 0;
}

size_t TextView::PositionIndicator::endTextIndex() const {
	std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
	return (i != myTextView.myTextBreaks.end()) ? *i : myTextView.myModel->paragraphsNumber();
}

void TextView::PositionIndicator::drawExtraText(const std::string &text) {
	ZLTextPositionIndicatorStyle &indicatorStyle = ZLTextStyleCollection::instance().indicatorStyle();
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();

	context().setFont(baseStyle.fontFamily(), indicatorStyle.FontSizeOption.value(), false, false);
	context().setColor(baseStyle.RegularTextColorOption.value());

	int width = context().stringWidth(text.data(), text.length());
	context().drawString(right() - width, bottom() - 2, text.data(), text.length());
	myExtraWidth += text.length() * context().stringWidth("0", 1) + context().spaceWidth();
}

size_t TextView::PositionIndicator::sizeOfTextBeforeParagraph(size_t paragraphNumber) const {
	return myTextView.myTextSize[paragraphNumber] - myTextView.myTextSize[startTextIndex()];
}

size_t TextView::PositionIndicator::sizeOfParagraph(size_t paragraphNumber) const {
	return myTextView.myTextSize[paragraphNumber + 1] - myTextView.myTextSize[paragraphNumber];
}

size_t TextView::PositionIndicator::sizeOfTextBeforeCursor() const {
	WordCursor endCursor = myTextView.endCursor();
	const size_t paragraphNumber = endCursor.paragraphCursor().index();
	const size_t paragraphLength = endCursor.paragraphCursor().paragraphLength();

	if (paragraphLength == 0) {
		return sizeOfTextBeforeParagraph(paragraphNumber);
	} else {
		return
			sizeOfTextBeforeParagraph(paragraphNumber) +
			sizeOfParagraph(paragraphNumber) * endCursor.wordNumber() / paragraphLength;
	}
}

std::string TextView::PositionIndicator::textPositionString() const {
	std::string buffer;
	ZLStringUtil::appendNumber(buffer, 1 + sizeOfTextBeforeCursor() / 2048);
	buffer += '/';
	ZLStringUtil::appendNumber(buffer, 1 + sizeOfTextBeforeParagraph(endTextIndex()) / 2048);

	return buffer;

	/*
	std::string buffer;

	const std::vector<size_t> &textSizeVector = myTextView.myTextSize;
	const size_t fullTextSize = textSizeVector[endTextIndex()] - textSizeVector[startTextIndex()];
	ZLStringUtil::appendNumber(buffer, 100 * sizeOfTextBeforeCursor() / fullTextSize);

	return buffer + '%';
	*/
}

std::string TextView::PositionIndicator::timeString() const {
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

void TextView::PositionIndicator::draw() {
	ZLTextPositionIndicatorStyle &indicatorStyle = ZLTextStyleCollection::instance().indicatorStyle();
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();

	ZLPaintContext &context = this->context();

	WordCursor endCursor = myTextView.endCursor();
	bool isEndOfText = false;
	if (endCursor.isEndOfParagraph()) {
		isEndOfText = !endCursor.nextParagraph();
	}

	myExtraWidth = 0;
	if (indicatorStyle.ShowTimeOption.value()) {
		drawExtraText(timeString());
	}
	if (indicatorStyle.ShowTextPositionOption.value()) {
		drawExtraText(textPositionString());
	}

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	size_t fillWidth = right - left - 1;

	if (!isEndOfText) {
		fillWidth =
			(size_t)(1.0 * fillWidth * sizeOfTextBeforeCursor() / sizeOfTextBeforeParagraph(endTextIndex()));
	}

	context.setColor(baseStyle.RegularTextColorOption.value());
	context.setFillColor(indicatorStyle.ColorOption.value());
	context.fillRectangle(left + 1, top + 1, left + fillWidth + 1, bottom - 1);
	context.drawLine(left, top, right, top);
	context.drawLine(left, bottom, right, bottom);
	context.drawLine(left, bottom, left, top);
	context.drawLine(right, bottom, right, top);
}

bool TextView::PositionIndicator::onStylusPress(int x, int y) {
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
	const size_t startIndex = startTextIndex();
	const size_t endIndex = endTextIndex();

	size_t fullTextSize = textSizeVector[endIndex] - textSizeVector[startIndex];
	size_t textSize = (size_t)(1.0 * fullTextSize * (x - left - 1) / (right - left - 1)) + textSizeVector[startIndex];
	std::vector<size_t>::const_iterator it = std::lower_bound(textSizeVector.begin(), textSizeVector.end(), textSize);
	size_t paragraphNumber = std::min((size_t)(it - textSizeVector.begin()), endIndex) - 1;
	if (paragraphNumber == startIndex) {
		myTextView.gotoParagraph(startIndex, false);
		myTextView.repaintView();
	} else {
		myTextView.gotoParagraph(paragraphNumber, true);
		myTextView.preparePaintInfo();
		const WordCursor &endCursor = myTextView.endCursor();
		if (!endCursor.isNull() && (paragraphNumber == endCursor.paragraphCursor().index())) {
			if (!endCursor.paragraphCursor().isLast() || !endCursor.isEndOfParagraph()) {
				size_t paragraphLength = endCursor.paragraphCursor().paragraphLength();
				if (paragraphLength > 0) {
					size_t wordNum =
						(size_t)((1.0 * (x - left - 1) / (right - left - 1) * fullTextSize
											- 1.0 * sizeOfTextBeforeParagraph(paragraphNumber))
										 / sizeOfParagraph(paragraphNumber) * paragraphLength);
					myTextView.moveEndCursor(endCursor.paragraphCursor().index(), wordNum, 0);
					myTextView.repaintView();
				}
			}
		} else {
			myTextView.repaintView();
		}
	}
	return true;
}

shared_ptr<TextView::PositionIndicator> TextView::createPositionIndicator() {
	return new PositionIndicator(*this);
}

TextView::PositionIndicator &TextView::positionIndicator() {
	if (myPositionIndicator.isNull()) {
		myPositionIndicator = createPositionIndicator();
	}
	return *myPositionIndicator;
}
