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
static const std::string SET_NUMBER_TO_OVERLAP = "Keep Lines";
static const std::string SET_NUMBER_TO_SCROLL = "Scroll Lines";
static const std::string SET_PERCENT_TO_SCROLL = "Scroll Percentage";

class OverlappingTypeEntry : public ZLComboOptionEntry {

private:
	static const std::string &nameByCode(int code);
	static TextView::OverlappingType codeByName(const std::string &name);
	
public:
	OverlappingTypeEntry(ScrollingOptionsPage &page) FB_DIALOG_SECTION;
	~OverlappingTypeEntry() FB_DIALOG_SECTION;

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

const std::string &OverlappingTypeEntry::nameByCode(int code) {
	switch (code) {
		case TextView::NUMBER_OF_OVERLAPPED_LINES:
			return SET_NUMBER_TO_OVERLAP;
		case TextView::NUMBER_OF_SCROLLED_LINES:
			return SET_NUMBER_TO_SCROLL;
		case TextView::PERCENT_OF_SCROLLED:
			return SET_PERCENT_TO_SCROLL;
		default:
			return NO_OVERLAPPING;
	}
}

TextView::OverlappingType OverlappingTypeEntry::codeByName(const std::string &name) {
	if (name == SET_NUMBER_TO_OVERLAP) {
		return TextView::NUMBER_OF_OVERLAPPED_LINES;
	}
	if (name == SET_NUMBER_TO_SCROLL) {
		return TextView::NUMBER_OF_SCROLLED_LINES;
	}
	if (name == SET_PERCENT_TO_SCROLL) {
		return TextView::PERCENT_OF_SCROLLED;
	}
	return TextView::NO_OVERLAPPING;
}

OverlappingTypeEntry::OverlappingTypeEntry(ScrollingOptionsPage &page) : myPage(page) {
	myValues.push_back(NO_OVERLAPPING);
	myValues.push_back(SET_NUMBER_TO_OVERLAP);
	myValues.push_back(SET_NUMBER_TO_SCROLL);
	myValues.push_back(SET_PERCENT_TO_SCROLL);
}

OverlappingTypeEntry::~OverlappingTypeEntry() {
}

const std::string &OverlappingTypeEntry::name() const {
	static const std::string _name = "Scrolling Mode";
	return _name;
}

const std::string &OverlappingTypeEntry::initialValue() const {
	return nameByCode(TextView::OverlappingTypeOption.value());
}

const std::vector<std::string> &OverlappingTypeEntry::values() const {
	return myValues;
}

void OverlappingTypeEntry::onAccept(const std::string &text) const {
	TextView::OverlappingTypeOption.setValue(codeByName(text));
}

void OverlappingTypeEntry::onValueChange(const std::string &selectedValue) {
	myPage.myLinesToOverlapEntry->setVisible(selectedValue == SET_NUMBER_TO_OVERLAP);
	myPage.myLinesToScrollEntry->setVisible(selectedValue == SET_NUMBER_TO_SCROLL);
	myPage.myPercentToScrollEntry->setVisible(selectedValue == SET_PERCENT_TO_SCROLL);
}

ScrollingOptionsPage::ScrollingOptionsPage(ZLOptionsDialogTab *dialogTab) {
	dialogTab->addOption(new ZLSimpleSpinOptionEntry(
		"Delay Between Scrollings, msecs", FBReader::ScrollingDelayOption, 0, 5000, 50
	));
	ZLComboOptionEntry *typeEntry = new OverlappingTypeEntry(*this);
	myLinesToOverlapEntry =
		new ZLSimpleSpinOptionEntry("Lines To Keep", TextView::LinesToOverlapOption, 1, 100, 1);
	myLinesToScrollEntry =
		new ZLSimpleSpinOptionEntry("Lines To Scroll", TextView::LinesToScrollOption, 1, 100, 1);
	myPercentToScrollEntry =
		new ZLSimpleSpinOptionEntry("Percent To Scroll", TextView::PercentToScrollOption, 1, 100, 5);
	typeEntry->onValueChange(typeEntry->initialValue());

	dialogTab->addOption(typeEntry);
	dialogTab->addOption(myLinesToOverlapEntry);
	dialogTab->addOption(myLinesToScrollEntry);
	dialogTab->addOption(myPercentToScrollEntry);
}
