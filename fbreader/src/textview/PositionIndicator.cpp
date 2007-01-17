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

TextView::PositionIndicator::PositionIndicator(TextView &textView) :
	myTextView(textView) {
}

void TextView::PositionIndicator::draw() {
	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	if (indicatorStyle.ShowOption.value()) {
		ZLPaintContext &context = myTextView.context();

		long bottom = context.height();
		long top = bottom - indicatorStyle.HeightOption.value() + 1;
		long left = 0;
		long right = context.width() - 1;

		std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
		size_t startIndex = (i != myTextView.myTextBreaks.begin()) ? *(i - 1) : 0;
		size_t endIndex = (i != myTextView.myTextBreaks.end()) ? *i : myTextView.myModel->paragraphsNumber();

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
}

bool TextView::PositionIndicator::onStylusPress(int x, int y) {
	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	if (!indicatorStyle.ShowOption.value() || !indicatorStyle.IsSensitiveOption.value()) {
		return false;
	}

	long bottom = myTextView.context().height();
	long top = bottom - indicatorStyle.HeightOption.value() + 1;
	long left = 0;
	long right = myTextView.context().width() - 1;

	if ((x < left) || (x > right) || (y < top) || (y > bottom)) {
		return false;
	}

	const std::vector<size_t> &textSizeVector = myTextView.myTextSize;
	if (textSizeVector.size() <= 1) {
		return true;
	}

	std::vector<size_t>::const_iterator i = myTextView.nextBreakIterator();
	size_t startIndex = (i != myTextView.myTextBreaks.begin()) ? *(i - 1) : 0;
	size_t endIndex = (i != myTextView.myTextBreaks.end()) ? *i : myTextView.myModel->paragraphsNumber();

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
