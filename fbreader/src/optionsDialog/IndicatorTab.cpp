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

#include <ZLOptionsDialog.h>

#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include <ZLTextStyleOptions.h>

#include "OptionsDialog.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/FBView.h"
#include "../fbreader/BookTextView.h"

class StateOptionEntry : public ZLToggleBooleanOptionEntry {

public:
	StateOptionEntry(ZLBooleanOption &option);
	void onStateChanged(bool state);

private:
	bool myState;

friend class SpecialFontSizeEntry;
};

class SpecialFontSizeEntry : public ZLSimpleSpinOptionEntry {

public:
	SpecialFontSizeEntry(ZLIntegerRangeOption &option, int step, StateOptionEntry &first, StateOptionEntry &second);
	void setVisible(bool state);

private:
	StateOptionEntry &myFirst;
	StateOptionEntry &mySecond;
};

StateOptionEntry::StateOptionEntry(ZLBooleanOption &option) : ZLToggleBooleanOptionEntry(option) {
	myState = option.value();
}

void StateOptionEntry::onStateChanged(bool state) {
	myState = state;
	ZLToggleBooleanOptionEntry::onStateChanged(state);
}

SpecialFontSizeEntry::SpecialFontSizeEntry(ZLIntegerRangeOption &option, int step, StateOptionEntry &first, StateOptionEntry &second) : ZLSimpleSpinOptionEntry(option, step), myFirst(first), mySecond(second) {
}

void SpecialFontSizeEntry::setVisible(bool) {
	ZLSimpleSpinOptionEntry::setVisible(
		(myFirst.isVisible() && myFirst.myState) ||
		(mySecond.isVisible() && mySecond.myState)
	);
}

class IndicatorTypeEntry : public ZLComboOptionEntry {

public:
	IndicatorTypeEntry(const ZLResource &resource, ZLIntegerRangeOption &typeOption);
	void addDependentEntry(ZLOptionEntry *entry);
	const std::string &initialValue() const;

private:
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

private:
	ZLIntegerRangeOption &myOption; 
	std::vector<std::string> myValues;
	std::vector<ZLOptionEntry*> myDependentEntries;
};

IndicatorTypeEntry::IndicatorTypeEntry(const ZLResource &resource, ZLIntegerRangeOption &typeOption) : myOption(typeOption) {
	myValues.push_back(resource["osScrollbar"].value());
	myValues.push_back(resource["fbIndicator"].value());
	myValues.push_back(resource["none"].value());
}

const std::string &IndicatorTypeEntry::initialValue() const {
	return myValues[myOption.value()];
}

const std::vector<std::string> &IndicatorTypeEntry::values() const {
	return myValues;
}

void IndicatorTypeEntry::addDependentEntry(ZLOptionEntry *entry) {
	myDependentEntries.push_back(entry);
}

void IndicatorTypeEntry::onAccept(const std::string &value) {
	for (size_t index = 0; index != myValues.size(); ++index) {
		if (myValues[index] == value) {
			myOption.setValue(index);
			break;
		}
	}
}

void IndicatorTypeEntry::onValueSelected(int index) {
	for (std::vector<ZLOptionEntry*>::iterator it = myDependentEntries.begin(); it != myDependentEntries.end(); ++it) {
		(*it)->setVisible(index == FBIndicatorStyle::FB_INDICATOR);
	}
}

void OptionsDialog::createIndicatorTab() {
	ZLDialogContent &indicatorTab = myDialog->createTab(ZLResourceKey("Indicator"));
	FBIndicatorStyle &indicatorInfo = FBView::commonIndicatorInfo();
	static ZLResourceKey typeKey("type");
	IndicatorTypeEntry *indicatorTypeEntry =
		new IndicatorTypeEntry(indicatorTab.resource(typeKey), indicatorInfo.TypeOption);
	indicatorTab.addOption(typeKey, indicatorTypeEntry);

	ZLOptionEntry *heightEntry =
		new ZLSimpleSpinOptionEntry(indicatorInfo.HeightOption, 1);
	ZLOptionEntry *offsetEntry =
		new ZLSimpleSpinOptionEntry(indicatorInfo.OffsetOption, 1);
	indicatorTab.addOptions(ZLResourceKey("height"), heightEntry, ZLResourceKey("offset"), offsetEntry);
	indicatorTypeEntry->addDependentEntry(heightEntry);
	indicatorTypeEntry->addDependentEntry(offsetEntry);

	StateOptionEntry *showTextPositionEntry =
		new StateOptionEntry(indicatorInfo.ShowTextPositionOption);
	indicatorTab.addOption(ZLResourceKey("pageNumber"), showTextPositionEntry);
	indicatorTypeEntry->addDependentEntry(showTextPositionEntry);

	StateOptionEntry *showTimeEntry =
		new StateOptionEntry(indicatorInfo.ShowTimeOption);
	indicatorTab.addOption(ZLResourceKey("time"), showTimeEntry);
	indicatorTypeEntry->addDependentEntry(showTimeEntry);

	SpecialFontSizeEntry *fontSizeEntry =
		new SpecialFontSizeEntry(indicatorInfo.FontSizeOption, 2, *showTextPositionEntry, *showTimeEntry);
	indicatorTab.addOption(ZLResourceKey("fontSize"), fontSizeEntry);
	indicatorTypeEntry->addDependentEntry(fontSizeEntry);
	showTextPositionEntry->addDependentEntry(fontSizeEntry);
	showTimeEntry->addDependentEntry(fontSizeEntry);

	ZLOptionEntry *tocMarksEntry =
		new ZLSimpleBooleanOptionEntry(FBReader::Instance().bookTextView().ShowTOCMarksOption);
	indicatorTab.addOption(ZLResourceKey("tocMarks"), tocMarksEntry);
	indicatorTypeEntry->addDependentEntry(tocMarksEntry);

	ZLOptionEntry *navigationEntry =
		new ZLSimpleBooleanOptionEntry(indicatorInfo.IsSensitiveOption);
	indicatorTab.addOption(ZLResourceKey("navigation"), navigationEntry);
	indicatorTypeEntry->addDependentEntry(navigationEntry);

	indicatorTypeEntry->onStringValueSelected(indicatorTypeEntry->initialValue());
	showTextPositionEntry->onStateChanged(showTextPositionEntry->initialState());
	showTimeEntry->onStateChanged(showTimeEntry->initialState());
}
