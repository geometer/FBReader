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

#include "ScrollingOptionsPage.h"
#include "FBReader.h"
#include "../textview/TextView.h"

static const std::string NO_OVERLAPPING = "No Overlapping";
static const std::string KEEP_LINES = "Keep Lines";
static const std::string SCROLL_LINES = "Scroll Lines";
static const std::string SCROLL_PERCENTAGE = "Scroll Percentage";

class ScrollingModeEntry : public ZLComboOptionEntry {

private:
	static const std::string &nameByCode(int code);
	static TextView::ScrollingMode codeByName(const std::string &name);
	
public:
	ScrollingModeEntry(ScrollingOptionsPage &page) FB_DIALOG_SECTION;
	~ScrollingModeEntry() FB_DIALOG_SECTION;

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

ScrollingModeEntry::ScrollingModeEntry(ScrollingOptionsPage &page) : myPage(page) {
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
	return nameByCode(TextView::ScrollingModeOption.value());
}

const std::vector<std::string> &ScrollingModeEntry::values() const {
	return myValues;
}

void ScrollingModeEntry::onAccept(const std::string &text) const {
	TextView::ScrollingModeOption.setValue(codeByName(text));
}

void ScrollingModeEntry::onValueChange(const std::string &selectedValue) {
	myPage.myLinesToKeepEntry->setVisible(selectedValue == KEEP_LINES);
	myPage.myLinesToScrollEntry->setVisible(selectedValue == SCROLL_LINES);
	myPage.myPercentToScrollEntry->setVisible(selectedValue == SCROLL_PERCENTAGE);
}

ScrollingOptionsPage::ScrollingOptionsPage(ZLOptionsDialogTab *dialogTab) {
	dialogTab->addOption(new ZLSimpleSpinOptionEntry(
		"Delay Between Scrollings, msecs", FBReader::ScrollingDelayOption, 0, 5000, 50
	));
	ZLComboOptionEntry *typeEntry = new ScrollingModeEntry(*this);
	myLinesToKeepEntry =
		new ZLSimpleSpinOptionEntry("Lines To Keep", TextView::LinesToKeepOption, 1, 100, 1);
	myLinesToScrollEntry =
		new ZLSimpleSpinOptionEntry("Lines To Scroll", TextView::LinesToScrollOption, 1, 100, 1);
	myPercentToScrollEntry =
		new ZLSimpleSpinOptionEntry("Percent To Scroll", TextView::PercentToScrollOption, 1, 100, 5);
	typeEntry->onValueChange(typeEntry->initialValue());

	dialogTab->addOption(typeEntry);
	dialogTab->addOption(myLinesToKeepEntry);
	dialogTab->addOption(myLinesToScrollEntry);
	dialogTab->addOption(myPercentToScrollEntry);
}
