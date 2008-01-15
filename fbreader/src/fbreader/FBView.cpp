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

#include "FBView.h"
#include "FBReader.h"
#include "FBReaderActions.h"

static const std::string OPTIONS = "Options";

FBMargins::FBMargins() :
	LeftMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "LeftMargin", 0, 1000, 4),
	RightMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "RightMargin", 0, 1000, 4),
	TopMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "TopMargin", 0, 1000, 0),
	BottomMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "BottomMargin", 0, 1000, 4) {
}

static const std::string INDICATOR = "Indicator";

FBIndicatorStyle::FBIndicatorStyle() :
	ShowOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Show", true),
	IsSensitiveOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "TouchSensitive", true),
	ShowTextPositionOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "PositionText", true),
	ShowTimeOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Time", true),
	ColorOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Color", ZLColor(127, 127, 127)),
	HeightOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Height", 1, 100, 16),
	OffsetOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Offset", 0, 100, 3),
	FontSizeOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "FontSize", 4, 72, 14) {
}

bool FBIndicatorStyle::isVisible() const {
	return ShowOption.value();
}

bool FBIndicatorStyle::isSensitive() const {
	return IsSensitiveOption.value();
}

bool FBIndicatorStyle::isTextPositionShown() const {
	return ShowTextPositionOption.value();
}

bool FBIndicatorStyle::isTimeShown() const {
	return ShowTimeOption.value();
}

ZLColor FBIndicatorStyle::color() const {
	return ColorOption.value();
}

int FBIndicatorStyle::height() const {
	return HeightOption.value();
}

int FBIndicatorStyle::offset() const {
	return OffsetOption.value();
}

int FBIndicatorStyle::fontSize() const {
	return FontSizeOption.value();
}

shared_ptr<ZLTextPositionIndicatorInfo> FBView::ourIndicatorInfo;
shared_ptr<FBMargins> FBView::ourMargins;
shared_ptr<ZLBooleanOption> FBView::ourSelectionOption;

FBIndicatorStyle& FBView::commonIndicatorInfo() {
	if (ourIndicatorInfo.isNull()) {
		ourIndicatorInfo = new FBIndicatorStyle();
	}
	return (FBIndicatorStyle&)*ourIndicatorInfo;
}

FBView::FBView(FBReader &reader, shared_ptr<ZLPaintContext> context) : ZLTextView(reader, context) {
}

shared_ptr<ZLTextPositionIndicatorInfo> FBView::indicatorInfo() const {
	if (ourIndicatorInfo.isNull()) {
		ourIndicatorInfo = new FBIndicatorStyle();
	}
	return ourIndicatorInfo;
}

void FBView::doTapScrolling(int y) {
	if (2 * y < context().height()) {
		fbreader().doAction(ActionCode::TAP_SCROLL_BACKWARD);
	} else {
		fbreader().doAction(ActionCode::TAP_SCROLL_FORWARD);
	}
}

bool FBView::onFingerTap(int, int y) {
	doTapScrolling(y);
	return true;
}

const std::string &FBView::caption() const {
	return myCaption;
}

void FBView::setCaption(const std::string &caption) {
	myCaption = caption;
}

bool FBView::onStylusPress(int x, int y) {
	if (ZLTextView::onStylusPress(x, y)) {
		return true;
	}
	
	if (_onStylusPress(x, y)) {
		return true;
	}

	if (fbreader().EnableTapScrollingOption.value() &&
			(!ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::FINGER_TAP_DETECTABLE, false).value() ||
			 !fbreader().TapScrollingOnFingerOnlyOption.value())) {
		doTapScrolling(y);
		return true;
	}

	activateSelection(x, y);
	return true;
}

bool FBView::_onStylusPress(int, int) {
	return false;
}

std::string FBView::word(const ZLTextElementArea &area) const {
	std::string txt;

	if (area.Kind == ZLTextElement::WORD_ELEMENT) {
		ZLTextWordCursor cursor = startCursor();
		cursor.moveToParagraph(area.ParagraphNumber);
		cursor.moveTo(area.TextElementNumber, 0);
		const ZLTextWord &word = (ZLTextWord&)cursor.element();
		ZLUnicodeUtil::Ucs2String ucs2;
		ZLUnicodeUtil::utf8ToUcs2(ucs2, word.Data, word.Size);
		ZLUnicodeUtil::Ucs2String::iterator it = ucs2.begin();
		while ((it != ucs2.end()) && !ZLUnicodeUtil::isLetter(*it)) {
			++it;
		}
		if (it != ucs2.end()) {
			ucs2.erase(ucs2.begin(), it);
			it = ucs2.end() - 1;
			while (!ZLUnicodeUtil::isLetter(*it)) {
				--it;
			}
			ucs2.erase(it + 1, ucs2.end());
    
			ZLUnicodeUtil::ucs2ToUtf8(txt, ucs2);
		}
	}
	return txt;
}

int FBView::leftMargin() const {
	return margins().LeftMarginOption.value();
}

int FBView::rightMargin() const {
	return margins().RightMarginOption.value();
}

int FBView::topMargin() const {
	return margins().TopMarginOption.value();
}

int FBView::bottomMargin() const {
	return margins().BottomMarginOption.value();
}

ZLBooleanOption &FBView::selectionOption() {
	if (ourSelectionOption.isNull()) {
		ourSelectionOption = new ZLBooleanOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "IsSelectionEnabled", true);
	}
	return *ourSelectionOption;
}

bool FBView::isSelectionEnabled() const {
	return selectionOption().value();
}
