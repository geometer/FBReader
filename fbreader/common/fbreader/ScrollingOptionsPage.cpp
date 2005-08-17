/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLDeviceInfo.h>

#include "ScrollingOptionsPage.h"
#include "../textview/TextView.h"

static const std::string LARGE_SCROLLING = "Large Scrollings";
static const std::string SMALL_SCROLLING = "Small Scrollings";
static const std::string MOUSE_SCROLLING = "Mouse Scrollings";

static const std::string NO_OVERLAPPING = "No Overlapping";
static const std::string KEEP_LINES = "Keep Lines";
static const std::string SCROLL_LINES = "Scroll Lines";
static const std::string SCROLL_PERCENTAGE = "Scroll Percentage";

class MainEntry : public ZLComboOptionEntry {

public:
	MainEntry(ScrollingOptionsPage &page) FB_DIALOG_SECTION;
	~MainEntry() FB_DIALOG_SECTION;

	const std::string &name() const FB_DIALOG_SECTION;
	const std::string &initialValue() const FB_DIALOG_SECTION;
	const std::vector<std::string> &values() const FB_DIALOG_SECTION;
	void onAccept(const std::string &text) const FB_DIALOG_SECTION;
	void onValueChange(const std::string &selectedValue) FB_DIALOG_SECTION;

private:
	ScrollingOptionsPage &myPage;
	std::string myName;
	std::vector<std::string> myValues;
};

class ScrollingModeEntry : public ZLComboOptionEntry {

private:
	static const std::string &nameByCode(int code);
	static TextView::ScrollingMode codeByName(const std::string &name);
	
public:
	ScrollingModeEntry(ScrollingOptionsPage::ScrollingEntries &entries, ZLIntegerOption &option) FB_DIALOG_SECTION;
	~ScrollingModeEntry() FB_DIALOG_SECTION;

	const std::string &name() const FB_DIALOG_SECTION;
	const std::string &initialValue() const FB_DIALOG_SECTION;
	const std::vector<std::string> &values() const FB_DIALOG_SECTION;
	void onAccept(const std::string &text) const FB_DIALOG_SECTION;
	void onValueChange(const std::string &selectedValue) FB_DIALOG_SECTION;
	void onMadeVisible() FB_DIALOG_SECTION;

private:
	ScrollingOptionsPage::ScrollingEntries &myEntries;
	ZLIntegerOption &myOption;
	std::string myName;
	std::vector<std::string> myValues;
	std::string myCurrentValue;
};

MainEntry::MainEntry(ScrollingOptionsPage &page) : myPage(page) {
	myValues.push_back(LARGE_SCROLLING);
	myValues.push_back(SMALL_SCROLLING);
	if (ZLDeviceInfo::isMousePresented()) {
		myValues.push_back(MOUSE_SCROLLING);
	}
}

MainEntry::~MainEntry() {
}

const std::string &MainEntry::name() const {
	static const std::string _name = "Options For";
	return _name;
}

const std::string &MainEntry::initialValue() const {
	return LARGE_SCROLLING;
}

const std::vector<std::string> &MainEntry::values() const {
	return myValues;
}

void MainEntry::onAccept(const std::string&) const {
}

void MainEntry::onValueChange(const std::string &selectedValue) {
	myPage.myLargeScrollingEntries.show(selectedValue == LARGE_SCROLLING);
	myPage.mySmallScrollingEntries.show(selectedValue == SMALL_SCROLLING);
	if (ZLDeviceInfo::isMousePresented()) {
		myPage.myMouseScrollingEntries.show(selectedValue == MOUSE_SCROLLING);
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

ScrollingModeEntry::ScrollingModeEntry(ScrollingOptionsPage::ScrollingEntries &page, ZLIntegerOption &option) : myEntries(page), myOption(option) {
	myValues.push_back(NO_OVERLAPPING);
	myValues.push_back(KEEP_LINES);
	myValues.push_back(SCROLL_LINES);
	myValues.push_back(SCROLL_PERCENTAGE);
}

ScrollingModeEntry::~ScrollingModeEntry() {
}

const std::string &ScrollingModeEntry::name() const {
	static const std::string _name = "Scrolling Mode";
	return _name;
}

const std::string &ScrollingModeEntry::initialValue() const {
	return nameByCode(myOption.value());
}

const std::vector<std::string> &ScrollingModeEntry::values() const {
	return myValues;
}

void ScrollingModeEntry::onAccept(const std::string &text) const {
	myOption.setValue(codeByName(text));
}

void ScrollingModeEntry::onMadeVisible() {
	onValueChange(myCurrentValue);
}

void ScrollingModeEntry::onValueChange(const std::string &selectedValue) {
	myCurrentValue = selectedValue;
	myEntries.myLinesToKeepEntry->setVisible(selectedValue == KEEP_LINES);
	myEntries.myLinesToScrollEntry->setVisible(selectedValue == SCROLL_LINES);
	myEntries.myPercentToScrollEntry->setVisible(selectedValue == SCROLL_PERCENTAGE);
}

void ScrollingOptionsPage::ScrollingEntries::init(FBReader::ScrollingOptions &options, ZLOptionsDialogTab *dialogTab) {
	myDelayEntry = new ZLSimpleSpinOptionEntry("Delay Between Scrollings, msecs", options.DelayOption, 0, 5000, 50);
	myModeEntry = new ScrollingModeEntry(*this, options.ModeOption);
	myLinesToKeepEntry = new ZLSimpleSpinOptionEntry("Lines To Keep", options.LinesToKeepOption, 1, 100, 1);
	myLinesToScrollEntry = new ZLSimpleSpinOptionEntry("Lines To Scroll", options.LinesToScrollOption, 1, 100, 1);
	myPercentToScrollEntry = new ZLSimpleSpinOptionEntry("Percent To Scroll", options.PercentToScrollOption, 1, 100, 5);
	myModeEntry->onValueChange(myModeEntry->initialValue());

	dialogTab->addOption(myDelayEntry);
	dialogTab->addOption(myModeEntry);
	dialogTab->addOption(myLinesToKeepEntry);
	dialogTab->addOption(myLinesToScrollEntry);
	dialogTab->addOption(myPercentToScrollEntry);
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

ScrollingOptionsPage::ScrollingOptionsPage(ZLOptionsDialogTab *dialogTab) {
	ZLComboOptionEntry *mainEntry = new MainEntry(*this);
	dialogTab->addOption(mainEntry);

	myLargeScrollingEntries.init(FBReader::LargeScrollingOptions, dialogTab);
	mySmallScrollingEntries.init(FBReader::SmallScrollingOptions, dialogTab);
	if (ZLDeviceInfo::isMousePresented()) {
		myMouseScrollingEntries.init(FBReader::MouseScrollingOptions, dialogTab);
	} else {
		myMouseScrollingEntries.myDelayEntry = 0;
	}

	mainEntry->onValueChange(mainEntry->initialValue());
}
