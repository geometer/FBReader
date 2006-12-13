/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLOptionsDialog.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "ScrollingOptionsPage.h"
#include "../textview/TextView.h"

static const std::string LARGE_SCROLLING = "Large Scrollings";
static const std::string SMALL_SCROLLING = "Small Scrollings";
static const std::string MOUSE_SCROLLING = "Mouse Scrollings";
static const std::string FINGER_TAP_SCROLLING = "Finger Tap Scrollings";

static const std::string NO_OVERLAPPING = "No Overlapping";
static const std::string KEEP_LINES = "Keep Lines";
static const std::string SCROLL_LINES = "Scroll Lines";
static const std::string SCROLL_PERCENTAGE = "Scroll Percentage";
static const std::string DISABLE = "Disable";

class ScrollingTypeEntry : public ZLComboOptionEntry {

public:
	ScrollingTypeEntry(FBReader &fbreader, ScrollingOptionsPage &page);

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &text);
	void onValueSelected(int index);

private:
	FBReader &myFBReader;
	ScrollingOptionsPage &myPage;
	std::string myName;
	std::vector<std::string> myValues;
};

class ScrollingModeEntry : public ZLComboOptionEntry {

private:
	static const std::string &nameByCode(int code);
	static TextView::ScrollingMode codeByName(const std::string &name);
	
public:
	ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &entries, ZLIntegerOption &option, bool isFingerTapOption);

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &text);
	void onValueSelected(int index);
	void onMadeVisible();

private:
	FBReader &myFBReader;
	ScrollingOptionsPage::ScrollingEntries &myEntries;
	ZLIntegerOption &myOption;
	std::string myName;
	std::vector<std::string> myValues;
	int myCurrentIndex;
	bool myIsFingerTapOption;
};

ScrollingTypeEntry::ScrollingTypeEntry(FBReader &fbreader, ScrollingOptionsPage &page) : myFBReader(fbreader), myPage(page) {
	myValues.push_back(LARGE_SCROLLING);
	myValues.push_back(SMALL_SCROLLING);
	if (myFBReader.isMousePresented()) {
		myValues.push_back(MOUSE_SCROLLING);
	}
	if (myFBReader.isFingerTapEventSupported()) {
		myValues.push_back(FINGER_TAP_SCROLLING);
	}
}

const std::string &ScrollingTypeEntry::name() const {
	static const std::string _name = "Options For";
	return _name;
}

const std::string &ScrollingTypeEntry::initialValue() const {
	return LARGE_SCROLLING;
}

const std::vector<std::string> &ScrollingTypeEntry::values() const {
	return myValues;
}

void ScrollingTypeEntry::onAccept(const std::string&) {
}

void ScrollingTypeEntry::onValueSelected(int index) {
	const std::string &selectedValue = values()[index];
	myPage.myLargeScrollingEntries.show(selectedValue == LARGE_SCROLLING);
	myPage.mySmallScrollingEntries.show(selectedValue == SMALL_SCROLLING);
	if (myFBReader.isMousePresented()) {
		myPage.myMouseScrollingEntries.show(selectedValue == MOUSE_SCROLLING);
	}
	if (myFBReader.isFingerTapEventSupported()) {
		myPage.myFingerTapScrollingEntries.show(selectedValue == FINGER_TAP_SCROLLING);
	}
}

const std::string &ScrollingModeEntry::nameByCode(int code) {
	switch (code) {
		case TextView::KEEP_LINES:
			return KEEP_LINES;
		case TextView::SCROLL_LINES:
			return SCROLL_LINES;
		case TextView::SCROLL_PERCENTAGE:
			return SCROLL_PERCENTAGE;
		default:
			return NO_OVERLAPPING;
	}
}

TextView::ScrollingMode ScrollingModeEntry::codeByName(const std::string &name) {
	if (name == KEEP_LINES) {
		return TextView::KEEP_LINES;
	}
	if (name == SCROLL_LINES) {
		return TextView::SCROLL_LINES;
	}
	if (name == SCROLL_PERCENTAGE) {
		return TextView::SCROLL_PERCENTAGE;
	}
	return TextView::NO_OVERLAPPING;
}

ScrollingModeEntry::ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &page, ZLIntegerOption &option, bool isFingerTapOption) : myFBReader(fbreader), myEntries(page), myOption(option), myIsFingerTapOption(isFingerTapOption) {
	myValues.push_back(NO_OVERLAPPING);
	myValues.push_back(KEEP_LINES);
	myValues.push_back(SCROLL_LINES);
	myValues.push_back(SCROLL_PERCENTAGE);
	if (myIsFingerTapOption) {
		myValues.push_back(DISABLE);
	}
}

const std::string &ScrollingModeEntry::name() const {
	static const std::string _name = "Scrolling Mode";
	return _name;
}

const std::string &ScrollingModeEntry::initialValue() const {
	if (myIsFingerTapOption && !myFBReader.EnableFingerScrollingOption.value()) {
		return DISABLE;
	}
	return nameByCode(myOption.value());
}

const std::vector<std::string> &ScrollingModeEntry::values() const {
	return myValues;
}

void ScrollingModeEntry::onAccept(const std::string &text) {
	if (myIsFingerTapOption && (text == DISABLE)) {
		myFBReader.EnableFingerScrollingOption.setValue(false);
	} else {
		myFBReader.EnableFingerScrollingOption.setValue(true);
		myOption.setValue(codeByName(text));
	}
}

void ScrollingModeEntry::onMadeVisible() {
	onValueSelected(myCurrentIndex);
}

void ScrollingModeEntry::onValueSelected(int index) {
	myCurrentIndex = index;
	const std::string &selectedValue = values()[index];
	myEntries.myDelayEntry->setVisible(selectedValue != DISABLE);
	myEntries.myLinesToKeepEntry->setVisible(selectedValue == KEEP_LINES);
	myEntries.myLinesToScrollEntry->setVisible(selectedValue == SCROLL_LINES);
	myEntries.myPercentToScrollEntry->setVisible(selectedValue == SCROLL_PERCENTAGE);
}

void ScrollingOptionsPage::ScrollingEntries::init(FBReader &fbreader, FBReader::ScrollingOptions &options) {
	myDelayEntry = new ZLSimpleSpinOptionEntry("Delay Between Scrollings, msecs", options.DelayOption, 50);
	myModeEntry = new ScrollingModeEntry(fbreader, *this, options.ModeOption, &options == &fbreader.FingerTapScrollingOptions);
	myLinesToKeepEntry = new ZLSimpleSpinOptionEntry("Lines To Keep", options.LinesToKeepOption, 1);
	myLinesToScrollEntry = new ZLSimpleSpinOptionEntry("Lines To Scroll", options.LinesToScrollOption, 1);
	myPercentToScrollEntry = new ZLSimpleSpinOptionEntry("Percent To Scroll", options.PercentToScrollOption, 5);
	myModeEntry->onStringValueSelected(myModeEntry->initialValue());
}

void ScrollingOptionsPage::ScrollingEntries::connect(ZLDialogContent &dialogTab) {
	dialogTab.addOption(myDelayEntry);
	dialogTab.addOption(myModeEntry);
	dialogTab.addOption(myLinesToKeepEntry);
	dialogTab.addOption(myLinesToScrollEntry);
	dialogTab.addOption(myPercentToScrollEntry);
}

void ScrollingOptionsPage::ScrollingEntries::show(bool visible) {
	if (myDelayEntry != 0) {
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
	ZLComboOptionEntry *mainEntry = new ScrollingTypeEntry(fbreader, *this);
	dialogTab.addOption(mainEntry);

	myLargeScrollingEntries.init(fbreader, fbreader.LargeScrollingOptions);
	mySmallScrollingEntries.init(fbreader, fbreader.SmallScrollingOptions);
	if (fbreader.isMousePresented()) {
		myMouseScrollingEntries.init(fbreader, fbreader.MouseScrollingOptions);
	}
	if (fbreader.isFingerTapEventSupported()) {
		myFingerTapScrollingEntries.init(fbreader, fbreader.FingerTapScrollingOptions);
	}

	mainEntry->onStringValueSelected(mainEntry->initialValue());

	myLargeScrollingEntries.connect(dialogTab);
	mySmallScrollingEntries.connect(dialogTab);
	if (fbreader.isMousePresented()) {
		myMouseScrollingEntries.connect(dialogTab);
	}
	if (fbreader.isFingerTapEventSupported()) {
		myFingerTapScrollingEntries.connect(dialogTab);
	}
}
