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

void OptionsDialog::createIndicatorTab(FBReader &fbreader) {
	ZLDialogContent &indicatorTab = myDialog->createTab(ZLResourceKey("Indicator"));
	FBIndicatorStyle &indicatorInfo = FBView::commonIndicatorInfo();
	ZLToggleBooleanOptionEntry *showIndicatorEntry =
		new ZLToggleBooleanOptionEntry(indicatorInfo.ShowOption);
	indicatorTab.addOption(ZLResourceKey("show"), showIndicatorEntry);

	ZLOptionEntry *heightEntry =
		new ZLSimpleSpinOptionEntry(indicatorInfo.HeightOption, 1);
	ZLOptionEntry *offsetEntry =
		new ZLSimpleSpinOptionEntry(indicatorInfo.OffsetOption, 1);
	indicatorTab.addOptions(ZLResourceKey("height"), heightEntry, ZLResourceKey("offset"), offsetEntry);
	showIndicatorEntry->addDependentEntry(heightEntry);
	showIndicatorEntry->addDependentEntry(offsetEntry);

	StateOptionEntry *showTextPositionEntry =
		new StateOptionEntry(indicatorInfo.ShowTextPositionOption);
	indicatorTab.addOption(ZLResourceKey("pageNumber"), showTextPositionEntry);
	showIndicatorEntry->addDependentEntry(showTextPositionEntry);

	StateOptionEntry *showTimeEntry =
		new StateOptionEntry(indicatorInfo.ShowTimeOption);
	indicatorTab.addOption(ZLResourceKey("time"), showTimeEntry);
	showIndicatorEntry->addDependentEntry(showTimeEntry);

	SpecialFontSizeEntry *fontSizeEntry =
		new SpecialFontSizeEntry(indicatorInfo.FontSizeOption, 2, *showTextPositionEntry, *showTimeEntry);
	indicatorTab.addOption(ZLResourceKey("fontSize"), fontSizeEntry);
	showIndicatorEntry->addDependentEntry(fontSizeEntry);
	showTextPositionEntry->addDependentEntry(fontSizeEntry);
	showTimeEntry->addDependentEntry(fontSizeEntry);

	ZLOptionEntry *tocMarksEntry =
		new ZLSimpleBooleanOptionEntry(fbreader.bookTextView().ShowTOCMarksOption);
	indicatorTab.addOption(ZLResourceKey("tocMarks"), tocMarksEntry);
	showIndicatorEntry->addDependentEntry(tocMarksEntry);

	ZLOptionEntry *navigationEntry =
		new ZLSimpleBooleanOptionEntry(indicatorInfo.IsSensitiveOption);
	indicatorTab.addOption(ZLResourceKey("navigation"), navigationEntry);
	showIndicatorEntry->addDependentEntry(navigationEntry);

	showIndicatorEntry->onStateChanged(showIndicatorEntry->initialState());
	showTextPositionEntry->onStateChanged(showTextPositionEntry->initialState());
	showTimeEntry->onStateChanged(showTimeEntry->initialState());
}
