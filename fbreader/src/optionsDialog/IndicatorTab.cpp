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

#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "OptionsDialog.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/BookTextView.h"

#include "../textview/TextStyleOptions.h"

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
	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	ZLToggleBooleanOptionEntry *showIndicatorEntry =
		new ZLToggleBooleanOptionEntry(indicatorStyle.ShowOption);
	indicatorTab.addOption("Show Position Indicator", "", showIndicatorEntry);

	ZLOptionEntry *heightEntry =
		new ZLSimpleSpinOptionEntry(indicatorStyle.HeightOption, 1);
	ZLOptionEntry *offsetEntry =
		new ZLSimpleSpinOptionEntry(indicatorStyle.OffsetOption, 1);
	indicatorTab.addOptions("Indicator Height", "", heightEntry, "Offset From Text", "", offsetEntry);
	showIndicatorEntry->addDependentEntry(heightEntry);
	showIndicatorEntry->addDependentEntry(offsetEntry);

	StateOptionEntry *showTextPositionEntry =
		new StateOptionEntry(indicatorStyle.ShowTextPositionOption);
	indicatorTab.addOption("Show Position As Numbers", "", showTextPositionEntry);
	showIndicatorEntry->addDependentEntry(showTextPositionEntry);

	StateOptionEntry *showTimeEntry =
		new StateOptionEntry(indicatorStyle.ShowTimeOption);
	indicatorTab.addOption("Show Time", "", showTimeEntry);
	showIndicatorEntry->addDependentEntry(showTimeEntry);

	SpecialFontSizeEntry *fontSizeEntry =
		new SpecialFontSizeEntry(indicatorStyle.FontSizeOption, 2, *showTextPositionEntry, *showTimeEntry);
	indicatorTab.addOption("Font Size", "", fontSizeEntry);
	showIndicatorEntry->addDependentEntry(fontSizeEntry);
	showTextPositionEntry->addDependentEntry(fontSizeEntry);
	showTimeEntry->addDependentEntry(fontSizeEntry);

	ZLOptionEntry *tocMarksEntry =
		new ZLSimpleBooleanOptionEntry(fbreader.bookTextView().ShowTOCMarksOption);
	indicatorTab.addOption("Show TOC Marks", "", tocMarksEntry);
	showIndicatorEntry->addDependentEntry(tocMarksEntry);

	ZLOptionEntry *navigationEntry =
		new ZLSimpleBooleanOptionEntry(indicatorStyle.IsSensitiveOption);
	indicatorTab.addOption("Enable Navigation", "", navigationEntry);
	showIndicatorEntry->addDependentEntry(navigationEntry);

	showIndicatorEntry->onStateChanged(showIndicatorEntry->initialState());
	showTextPositionEntry->onStateChanged(showTextPositionEntry->initialState());
	showTimeEntry->onStateChanged(showTimeEntry->initialState());
}
