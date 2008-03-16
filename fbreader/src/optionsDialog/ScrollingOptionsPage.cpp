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

#include <ZLOptionsDialog.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include <ZLTextView.h>

#include "ScrollingOptionsPage.h"

class ScrollingTypeEntry : public ZLComboOptionEntry {

public:
	ScrollingTypeEntry(const ZLResource &resource, FBReader &fbreader, ScrollingOptionsPage &page);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &text);
	void onValueSelected(int index);

private:
	std::string myLargeScrollingString;
	std::string mySmallScrollingString;
	std::string myMouseScrollingString;
	std::string myTapScrollingString;

private:
	const ZLResource &myResource;
	FBReader &myFBReader;
	ScrollingOptionsPage &myPage;
	std::vector<std::string> myValues;
};

class ScrollingModeEntry : public ZLComboOptionEntry {

public:
	static std::string ourNoOverlappingString;
	static std::string ourKeepLinesString;
	static std::string ourScrollLinesString;
	static std::string ourScrollPercentageString;
	static std::string ourDisableString;

private:
	static const std::string &nameByCode(int code);
	static ZLTextView::ScrollingMode codeByName(const std::string &name);
	
public:
	ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &entries, ZLIntegerOption &option, bool isTapOption);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &text);
	void onValueSelected(int index);
	void onMadeVisible();

private:
	FBReader &myFBReader;
	ScrollingOptionsPage::ScrollingEntries &myEntries;
	ZLIntegerOption &myOption;
	std::vector<std::string> myValues;
	int myCurrentIndex;
	bool myIsTapOption;
};

ScrollingTypeEntry::ScrollingTypeEntry(const ZLResource &resource, FBReader &fbreader, ScrollingOptionsPage &page) : myResource(resource), myFBReader(fbreader), myPage(page) {
	myLargeScrollingString = resource["large"].value();
	mySmallScrollingString = resource["small"].value();
	myMouseScrollingString = resource["mouse"].value();
	myTapScrollingString = resource["tap"].value();

	myValues.push_back(myLargeScrollingString);
	myValues.push_back(mySmallScrollingString);

	const bool isMousePresented = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::MOUSE_PRESENTED, false).value();
	const bool hasTouchScreen = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::TOUCHSCREEN_PRESENTED, false).value();

	if (isMousePresented) {
		myValues.push_back(myMouseScrollingString);
	}
	if (hasTouchScreen) {
		myValues.push_back(myTapScrollingString);
	}
}

const std::string &ScrollingTypeEntry::initialValue() const {
	return myLargeScrollingString;
}

const std::vector<std::string> &ScrollingTypeEntry::values() const {
	return myValues;
}

void ScrollingTypeEntry::onAccept(const std::string&) {
}

void ScrollingTypeEntry::onValueSelected(int index) {
	const std::string &selectedValue = values()[index];
	myPage.myLargeScrollingEntries.show(selectedValue == myLargeScrollingString);
	myPage.mySmallScrollingEntries.show(selectedValue == mySmallScrollingString);

	const bool isMousePresented = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::MOUSE_PRESENTED, false).value();
	const bool hasTouchScreen = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::TOUCHSCREEN_PRESENTED, false).value();

	if (isMousePresented) {
		myPage.myMouseScrollingEntries.show(selectedValue == myMouseScrollingString);
	}
	if (hasTouchScreen) {
		myPage.myTapScrollingEntries.show(selectedValue == myTapScrollingString);
	}
}

std::string ScrollingModeEntry::ourNoOverlappingString;
std::string ScrollingModeEntry::ourKeepLinesString;
std::string ScrollingModeEntry::ourScrollLinesString;
std::string ScrollingModeEntry::ourScrollPercentageString;
std::string ScrollingModeEntry::ourDisableString;

const std::string &ScrollingModeEntry::nameByCode(int code) {
	switch (code) {
		case ZLTextView::KEEP_LINES:
			return ourKeepLinesString;
		case ZLTextView::SCROLL_LINES:
			return ourScrollLinesString;
		case ZLTextView::SCROLL_PERCENTAGE:
			return ourScrollPercentageString;
		default:
			return ourNoOverlappingString;
	}
}

ZLTextView::ScrollingMode ScrollingModeEntry::codeByName(const std::string &name) {
	if (name == ourKeepLinesString) {
		return ZLTextView::KEEP_LINES;
	}
	if (name == ourScrollLinesString) {
		return ZLTextView::SCROLL_LINES;
	}
	if (name == ourScrollPercentageString) {
		return ZLTextView::SCROLL_PERCENTAGE;
	}
	return ZLTextView::NO_OVERLAPPING;
}

ScrollingModeEntry::ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &page, ZLIntegerOption &option, bool isTapOption) : myFBReader(fbreader), myEntries(page), myOption(option), myIsTapOption(isTapOption) {
	myValues.push_back(ourNoOverlappingString);
	myValues.push_back(ourKeepLinesString);
	myValues.push_back(ourScrollLinesString);
	myValues.push_back(ourScrollPercentageString);
	if (myIsTapOption) {
		myValues.push_back(ourDisableString);
	}
}

const std::string &ScrollingModeEntry::initialValue() const {
	if (myIsTapOption && !myFBReader.EnableTapScrollingOption.value()) {
		return ourDisableString;
	}
	return nameByCode(myOption.value());
}

const std::vector<std::string> &ScrollingModeEntry::values() const {
	return myValues;
}

void ScrollingModeEntry::onAccept(const std::string &text) {
	if (myIsTapOption) {
		if (text == ourDisableString) {
			myFBReader.EnableTapScrollingOption.setValue(false);
		} else {
			myFBReader.EnableTapScrollingOption.setValue(true);
			myOption.setValue(codeByName(text));
		}
	} else {
		myOption.setValue(codeByName(text));
	}
}

void ScrollingModeEntry::onMadeVisible() {
	onValueSelected(myCurrentIndex);
}

void ScrollingModeEntry::onValueSelected(int index) {
	myCurrentIndex = index;
	const std::string &selectedValue = values()[index];
	if (myEntries.myFingerOnlyEntry != 0) {
		myEntries.myFingerOnlyEntry->setVisible(selectedValue != ourDisableString);
	}
	myEntries.myDelayEntry->setVisible(selectedValue != ourDisableString);
	myEntries.myLinesToKeepEntry->setVisible(selectedValue == ourKeepLinesString);
	myEntries.myLinesToScrollEntry->setVisible(selectedValue == ourScrollLinesString);
	myEntries.myPercentToScrollEntry->setVisible(selectedValue == ourScrollPercentageString);
}

static const ZLResourceKey delayKey("delay");
static const ZLResourceKey modeKey("mode");
static const ZLResourceKey linesToKeepKey("linesToKeep");
static const ZLResourceKey linesToScrollKey("linesToScroll");
static const ZLResourceKey percentToScrollKey("percentToScroll");
static const ZLResourceKey fingerOnlyKey("fingerOnly");

void ScrollingOptionsPage::ScrollingEntries::init(FBReader &fbreader, FBReader::ScrollingOptions &options) {
	const bool isTapOption = &options == &fbreader.TapScrollingOptions;
	const bool isFingerTapDetectionSupported = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::FINGER_TAP_DETECTABLE, false).value();
	if (isTapOption && isFingerTapDetectionSupported) {
		myFingerOnlyEntry = new ZLSimpleBooleanOptionEntry(fbreader.TapScrollingOnFingerOnlyOption);
	} else {
		myFingerOnlyEntry = 0;
	}
	myDelayEntry = new ZLSimpleSpinOptionEntry(options.DelayOption, 50);
	myModeEntry = new ScrollingModeEntry(fbreader, *this, options.ModeOption, isTapOption);
	myLinesToKeepEntry = new ZLSimpleSpinOptionEntry(options.LinesToKeepOption, 1);
	myLinesToScrollEntry = new ZLSimpleSpinOptionEntry(options.LinesToScrollOption, 1);
	myPercentToScrollEntry = new ZLSimpleSpinOptionEntry(options.PercentToScrollOption, 5);
	myModeEntry->onStringValueSelected(myModeEntry->initialValue());
}

void ScrollingOptionsPage::ScrollingEntries::connect(ZLDialogContent &dialogTab) {
	dialogTab.addOption(delayKey, myDelayEntry);
	dialogTab.addOption(modeKey, myModeEntry);
	dialogTab.addOption(linesToKeepKey, myLinesToKeepEntry);
	dialogTab.addOption(linesToScrollKey, myLinesToScrollEntry);
	dialogTab.addOption(percentToScrollKey, myPercentToScrollEntry);
	if (myFingerOnlyEntry != 0) {
		dialogTab.addOption(fingerOnlyKey, myFingerOnlyEntry);
	}
}

void ScrollingOptionsPage::ScrollingEntries::show(bool visible) {
	if (myDelayEntry != 0) {
		if (myFingerOnlyEntry != 0) {
			myFingerOnlyEntry->setVisible(visible);
		}
		myDelayEntry->setVisible(visible);
		myModeEntry->setVisible(visible);
		if (visible) {
			((ScrollingModeEntry*)myModeEntry)->onMadeVisible();
		} else {
			myLinesToKeepEntry->setVisible(false);
			myLinesToScrollEntry->setVisible(false);
			myPercentToScrollEntry->setVisible(false);
		}
	}
}

ScrollingOptionsPage::ScrollingOptionsPage(ZLDialogContent &dialogTab, FBReader &fbreader) {
	const ZLResourceKey optionsForKey("optionsFor");
	ZLComboOptionEntry *mainEntry = new ScrollingTypeEntry(dialogTab.resource(optionsForKey), fbreader, *this);
	dialogTab.addOption(optionsForKey, mainEntry);

	const ZLResource &modeResource = dialogTab.resource(modeKey);
	ScrollingModeEntry::ourNoOverlappingString = modeResource["noOverlapping"].value();
	ScrollingModeEntry::ourKeepLinesString = modeResource["keepLines"].value();
	ScrollingModeEntry::ourScrollLinesString = modeResource["scrollLines"].value();
	ScrollingModeEntry::ourScrollPercentageString = modeResource["scrollPercentage"].value();
	ScrollingModeEntry::ourDisableString = modeResource["disable"].value();

	myLargeScrollingEntries.init(fbreader, fbreader.LargeScrollingOptions);
	mySmallScrollingEntries.init(fbreader, fbreader.SmallScrollingOptions);

	const bool isMousePresented = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::MOUSE_PRESENTED, false).value();
	const bool hasTouchScreen = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::TOUCHSCREEN_PRESENTED, false).value();

	if (isMousePresented) {
		myMouseScrollingEntries.init(fbreader, fbreader.MouseScrollingOptions);
	}
	if (hasTouchScreen) {
		myTapScrollingEntries.init(fbreader, fbreader.TapScrollingOptions);
	}

	mainEntry->onStringValueSelected(mainEntry->initialValue());

	myLargeScrollingEntries.connect(dialogTab);
	mySmallScrollingEntries.connect(dialogTab);
	if (isMousePresented) {
		myMouseScrollingEntries.connect(dialogTab);
	}
	if (hasTouchScreen) {
		myTapScrollingEntries.connect(dialogTab);
	}
}
