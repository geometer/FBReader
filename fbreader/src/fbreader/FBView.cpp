/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <cmath>
#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLTimeManager.h>
#include <ZLTextSelectionModel.h>

#include "FBView.h"
#include "FBReader.h"
#include "FBReaderActions.h"
#include "../options/FBOptions.h"
#include "../options/FBTextStyle.h"

static const std::string INDICATOR = "Indicator";

FBIndicatorStyle::FBIndicatorStyle() :
	TypeOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Type", 0, 2, ZLTextPositionIndicatorInfo::OS_SCROLLBAR),
	IsSensitiveOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "TouchSensitive", true),
	ShowTextPositionOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "PositionText", false),
	ShowTimeOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Time", false),
	ColorOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Color", ZLColor(127, 127, 127)),
	HeightOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Height", 1, 100, 16),
	OffsetOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "Offset", 0, 100, 3),
	FontSizeOption(ZLCategoryKey::LOOK_AND_FEEL, INDICATOR, "FontSize", 4, 72, 14) {
}

ZLTextPositionIndicatorInfo::Type FBIndicatorStyle::type() const {
	return (ZLTextPositionIndicatorInfo::Type)TypeOption.value();
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
shared_ptr<ZLBooleanOption> FBView::ourSelectionOption;

FBIndicatorStyle& FBView::commonIndicatorInfo() {
	if (ourIndicatorInfo.isNull()) {
		ourIndicatorInfo = new FBIndicatorStyle();
	}
	return (FBIndicatorStyle&)*ourIndicatorInfo;
}

FBView::FBView(ZLPaintContext &context) : ZLTextView(context) {
}

shared_ptr<ZLTextPositionIndicatorInfo> FBView::indicatorInfo() const {
	if (ourIndicatorInfo.isNull()) {
		ourIndicatorInfo = new FBIndicatorStyle();
	}
	return ourIndicatorInfo;
}

void FBView::doTapScrolling(int y) {
	if (2 * y < context().height()) {
		FBReader::Instance().doAction(ActionCode::TAP_SCROLL_BACKWARD);
	} else {
		FBReader::Instance().doAction(ActionCode::TAP_SCROLL_FORWARD);
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
	std::replace(myCaption.begin(), myCaption.end(), '\n', ' ');
	std::replace(myCaption.begin(), myCaption.end(), '\r', ' ');
	ZLUnicodeUtil::cleanUtf8String(myCaption);
}

bool FBView::onStylusPress(int x, int y) {
	if (!myTapScroller.isNull()) {
		ZLTimeManager::Instance().removeTask(myTapScroller);
		myTapScroller.reset();
	}

	myPressedX = x;
	myPressedY = y;
	myIsReleasedWithoutMotion = false;

	if (ZLTextView::onStylusPress(x, y)) {
		return true;
	}

	myIsReleasedWithoutMotion = true;
	
	if (_onStylusPress(x, y)) {
		return true;
	}

	return true;
}

bool FBView::_onStylusPress(int, int) {
	return false;
}

class FBView::TapScroller : public ZLRunnable {

public:
	TapScroller(FBView &view, int y);

private:
	void run();

private:
	FBView &myView;
	const int myY;
};

FBView::TapScroller::TapScroller(FBView &view, int y) : myView(view), myY(y) {
}

void FBView::TapScroller::run() {
	myView.doTapScrolling(myY);
}

bool FBView::onStylusRelease(int x, int y) {
	const bool hadSelection = !selectionModel().isEmpty();

	if (!myTapScroller.isNull()) {
		ZLTimeManager::Instance().removeTask(myTapScroller);
		myTapScroller.reset();
	}

	if (ZLTextView::onStylusRelease(x, y)) {
		return true;
	}
	
	if (_onStylusRelease(x, y)) {
		return true;
	}

	FBReader &fbreader = FBReader::Instance();
	myIsReleasedWithoutMotion =
		myIsReleasedWithoutMotion && (abs(x - pressedX()) <= 5) && (abs(y - pressedY()) <= 5);
	if (!hadSelection && isReleasedWithoutMotion() &&
			fbreader.EnableTapScrollingOption.value() &&
			(!ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::FINGER_TAP_DETECTABLE, false).value() ||
			 !fbreader.TapScrollingOnFingerOnlyOption.value())) {
		myTapScroller = new TapScroller(*this, y);
		ZLTimeManager::Instance().addAutoRemovableTask(myTapScroller, doubleClickDelay());
		return true;
	}

	return false;
}

bool FBView::_onStylusRelease(int, int) {
	return false;
}

bool FBView::onStylusMove(int x, int y) {
	if (ZLTextView::onStylusMove(x, y)) {
		return true;
	}
	
	if (_onStylusMove(x, y)) {
		return true;
	}

	return false;
}

bool FBView::_onStylusMove(int, int) {
	return false;
}

bool FBView::onStylusMovePressed(int x, int y) {
	if (myIsReleasedWithoutMotion) {
		if ((abs(x - pressedX()) > 5) || (abs(y - pressedY()) > 5)) {
			myIsReleasedWithoutMotion = false;
			activateSelection(pressedX(), pressedY());
		}
	}

	if (ZLTextView::onStylusMovePressed(x, y)) {
		return true;
	}
	
	if (_onStylusMovePressed(x, y)) {
		return true;
	}

	return false;
}

bool FBView::_onStylusMovePressed(int, int) {
	return false;
}

std::string FBView::word(const ZLTextElementRectangle &rectangle) const {
	std::string txt;

	if (rectangle.Kind == ZLTextElement::WORD_ELEMENT) {
		ZLTextWordCursor cursor = textArea().startCursor();
		cursor.moveToParagraph(rectangle.ParagraphIndex);
		cursor.moveTo(rectangle.ElementIndex, 0);
		const ZLTextWord &word = (ZLTextWord&)cursor.element();
		ZLUnicodeUtil::Ucs4String ucs4;
		ZLUnicodeUtil::utf8ToUcs4(ucs4, word.Data, word.Size);
		ZLUnicodeUtil::Ucs4String::iterator it = ucs4.begin();
		while ((it != ucs4.end()) && !ZLUnicodeUtil::isLetter(*it)) {
			++it;
		}
		if (it != ucs4.end()) {
			ucs4.erase(ucs4.begin(), it);
			it = ucs4.end() - 1;
			while (!ZLUnicodeUtil::isLetter(*it)) {
				--it;
			}
			ucs4.erase(it + 1, ucs4.end());
    
			ZLUnicodeUtil::ucs4ToUtf8(txt, ucs4);
		}
	}
	return txt;
}

int FBView::leftMargin() const {
	return FBOptions::Instance().LeftMarginOption.value();
}

int FBView::rightMargin() const {
	return FBOptions::Instance().RightMarginOption.value();
}

int FBView::topMargin() const {
	return FBOptions::Instance().TopMarginOption.value();
}

int FBView::bottomMargin() const {
	return FBOptions::Instance().BottomMarginOption.value();
}

ZLColor FBView::backgroundColor() const {
	return FBOptions::Instance().BackgroundColorOption.value();
}

ZLColor FBView::color(const std::string &colorStyle) const {
	return FBOptions::Instance().colorOption(colorStyle).value();
}

shared_ptr<ZLTextStyle> FBView::baseStyle() const {
	return FBTextStyle::InstanceAsPtr();
}

ZLBooleanOption &FBView::selectionOption() {
	if (ourSelectionOption.isNull()) {
		ourSelectionOption = new ZLBooleanOption(ZLCategoryKey::LOOK_AND_FEEL, "Options", "IsSelectionEnabled", true);
	}
	return *ourSelectionOption;
}

bool FBView::isSelectionEnabled() const {
	return selectionOption().value();
}

int FBView::doubleClickDelay() const {
	return isSelectionEnabled() ? 200 : 0;
}

bool FBView::hasContents() const {
	return true;
}
