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

#include "TextView.h"
#include "TextStyle.h"

static const std::string INDICATOR = "Indicator";

PositionIndicatorStyle::PositionIndicatorStyle() :
	ShowOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Show", true),
	IsSensitiveOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "TouchSensitive", true),
	ColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Color", ZLColor(127, 127, 127)),
	HeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Height", 1, 100, 16),
	OffsetOption(ZLOption::LOOK_AND_FEEL_CATEGORY, INDICATOR, "Offset", 0, 100, 4) {
}

TextView::PositionIndicator::PositionIndicator(TextView &textView) : myTextView(textView) {
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
	return bottom() - TextStyleCollection::instance().indicatorStyle().HeightOption.value() + 1;
}

int TextView::PositionIndicator::left() const {
	return 0;
}

int TextView::PositionIndicator::right() const {
	return context().width() - 1;
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

void TextView::PositionIndicator::draw() {
	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();

	ZLPaintContext &context = this->context();

	const long bottom = this->bottom();
	const long top = this->top();
	const long left = this->left();
	const long right = this->right();

	const size_t startIndex = startTextIndex();
	const size_t endIndex = endTextIndex();

	WordCursor endCursor = myTextView.endCursor();
	bool isEndOfText = false;
	if (endCursor.isEndOfParagraph()) {
		isEndOfText = !endCursor.nextParagraph();
	}

	size_t fillWidth = right - left - 1;

	if (!isEndOfText) {
		size_t paragraphNumber = endCursor.paragraphCursor().index();
		const std::vector<size_t> &textSizeVector = myTextView.myTextSize;
		size_t sizeOfTextBeforeParagraph = textSizeVector[paragraphNumber] - textSizeVector[startIndex];
		size_t fullTextSize = textSizeVector[endIndex] - textSizeVector[startIndex];
		size_t paragraphLength = endCursor.paragraphCursor().paragraphLength();
	
		if (paragraphLength == 0) {
			fillWidth = (size_t)(1.0 * fillWidth * sizeOfTextBeforeParagraph / fullTextSize);
		} else {
			size_t sizeOfParagraph = textSizeVector[paragraphNumber + 1] - textSizeVector[paragraphNumber];
			fillWidth = (size_t)(
				(sizeOfTextBeforeParagraph + 1.0 * sizeOfParagraph * endCursor.wordNumber() / paragraphLength) *
				fillWidth / fullTextSize
			);
		}
	}

	context.setColor(TextStyleCollection::instance().baseStyle().RegularTextColorOption.value());
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
					size_t sizeOfTextBeforeParagraph = textSizeVector[paragraphNumber] - textSizeVector[startIndex];
					size_t sizeOfParagraph = textSizeVector[paragraphNumber + 1] - textSizeVector[paragraphNumber];
					size_t wordNum =
						(size_t)((1.0 * (x - left - 1) / (right - left - 1) * fullTextSize
											- 1.0 * sizeOfTextBeforeParagraph)
										 / sizeOfParagraph * paragraphLength);
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
