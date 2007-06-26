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
	std::string myFingerTapScrollingString;

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
	static TextView::ScrollingMode codeByName(const std::string &name);
	
public:
	ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &entries, ZLIntegerOption &option, bool isFingerTapOption);

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
	bool myIsFingerTapOption;
};

ScrollingTypeEntry::ScrollingTypeEntry(const ZLResource &resource, FBReader &fbreader, ScrollingOptionsPage &page) : myResource(resource), myFBReader(fbreader), myPage(page) {
	myLargeScrollingString = resource["large"].value();
	mySmallScrollingString = resource["small"].value();
	myMouseScrollingString = resource["mouse"].value();
	myFingerTapScrollingString = resource["finger"].value();

	myValues.push_back(myLargeScrollingString);
	myValues.push_back(mySmallScrollingString);
	if (myFBReader.isMousePresented()) {
		myValues.push_back(myMouseScrollingString);
	}
	if (myFBReader.isFingerTapEventSupported()) {
		myValues.push_back(myFingerTapScrollingString);
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
	if (myFBReader.isMousePresented()) {
		myPage.myMouseScrollingEntries.show(selectedValue == myMouseScrollingString);
	}
	if (myFBReader.isFingerTapEventSupported()) {
		myPage.myFingerTapScrollingEntries.show(selectedValue == myFingerTapScrollingString);
	}
}

std::string ScrollingModeEntry::ourNoOverlappingString;
std::string ScrollingModeEntry::ourKeepLinesString;
std::string ScrollingModeEntry::ourScrollLinesString;
std::string ScrollingModeEntry::ourScrollPercentageString;
std::string ScrollingModeEntry::ourDisableString;

const std::string &ScrollingModeEntry::nameByCode(int code) {
	switch (code) {
		case TextView::KEEP_LINES:
			return ourKeepLinesString;
		case TextView::SCROLL_LINES:
			return ourScrollLinesString;
		case TextView::SCROLL_PERCENTAGE:
			return ourScrollPercentageString;
		default:
			return ourNoOverlappingString;
	}
}

TextView::ScrollingMode ScrollingModeEntry::codeByName(const std::string &name) {
	if (name == ourKeepLinesString) {
		return TextView::KEEP_LINES;
	}
	if (name == ourScrollLinesString) {
		return TextView::SCROLL_LINES;
	}
	if (name == ourScrollPercentageString) {
		return TextView::SCROLL_PERCENTAGE;
	}
	return TextView::NO_OVERLAPPING;
}

ScrollingModeEntry::ScrollingModeEntry(FBReader &fbreader, ScrollingOptionsPage::ScrollingEntries &page, ZLIntegerOption &option, bool isFingerTapOption) : myFBReader(fbreader), myEntries(page), myOption(option), myIsFingerTapOption(isFingerTapOption) {
	myValues.push_back(ourNoOverlappingString);
	myValues.push_back(ourKeepLinesString);
	myValues.push_back(ourScrollLinesString);
	myValues.push_back(ourScrollPercentageString);
	if (myIsFingerTapOption) {
		myValues.push_back(ourDisableString);
	}
}

const std::string &ScrollingModeEntry::initialValue() const {
	if (myIsFingerTapOption && !myFBReader.EnableFingerScrollingOption.value()) {
		return ourDisableString;
	}
	return nameByCode(myOption.value());
}

const std::vector<std::string> &ScrollingModeEntry::values() const {
	return myValues;
}

void ScrollingModeEntry::onAccept(const std::string &text) {
	if (myIsFingerTapOption && (text == ourDisableString)) {
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

void ScrollingOptionsPage::ScrollingEntries::init(FBReader &fbreader, FBReader::ScrollingOptions &options) {
	myDelayEntry = new ZLSimpleSpinOptionEntry(options.DelayOption, 50);
	myModeEntry = new ScrollingModeEntry(fbreader, *this, options.ModeOption, &options == &fbreader.FingerTapScrollingOptions);
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
