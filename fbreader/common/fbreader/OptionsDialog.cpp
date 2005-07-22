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

#include <abstract/ZLDialogManager.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLPaintContext.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "StyleOptionsPage.h"
#include "ColorOptionsPage.h"
#include "KeyBindingsPage.h"

#include "FBReader.h"

#include "../collection/BookCollection.h"
#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"

class ShowIndicatorEntry : public ZLSimpleBooleanOptionEntry {

public:
	ShowIndicatorEntry(IndicatorPage &page, const std::string &name, const ZLBooleanOption &option);
	~ShowIndicatorEntry();
	void onValueChange(bool value);

private:
	IndicatorPage &myPage;
};

ShowIndicatorEntry::ShowIndicatorEntry(IndicatorPage &page, const std::string &name, const ZLBooleanOption &option) : ZLSimpleBooleanOptionEntry(name, option), myPage(page) {
}

ShowIndicatorEntry::~ShowIndicatorEntry() {
}

void ShowIndicatorEntry::onValueChange(bool value) {
	myPage.HeightEntry->setVisible(value);
	myPage.OffsetEntry->setVisible(value);
	myPage.EnableNavigationEntry->setVisible(value);
}

OptionsDialog::OptionsDialog(ZLPaintContext &context) {
	myDialog = ZLDialogManager::instance().createOptionsDialog("OptionsDialog", "FBReader - Options");

	ZLOptionsDialogTab *generalTab = myDialog->createTab("General");
	generalTab->addOption(new ZLSimpleBooleanOptionEntry("Quit Application On Cancel", FBReader::QuitOnCancelOption));
	generalTab->addOption(new ZLSimpleSpinOptionEntry("Delay Between Page Scrollings, msecs", FBReader::ScrollingDelayOption, 0, 5000, 50));
	generalTab->addOption(new ZLSimpleStringOptionEntry("Book Path", BookCollection::PathOption));
	generalTab->addOption(new ZLSimpleBooleanOptionEntry("Look For Books In Subdirectories", BookCollection::ScanSubdirsOption));
	generalTab->addOption(new ZLSimpleSpinOptionEntry("Recent books list size", LastOpenedBooks::MaxListSizeOption, 0, 100, 1));

	ZLOptionsDialogTab *marginTab = myDialog->createTab("Margins");
	marginTab->addOptions(
		new ZLSimpleSpinOptionEntry("Left Margin", TextStyle::LeftMarginOption, 0, 100, 1),
		new ZLSimpleSpinOptionEntry("Right Margin", TextStyle::RightMarginOption, 0, 100, 1)
	);
	marginTab->addOptions(
		new ZLSimpleSpinOptionEntry("Top Margin", TextStyle::TopMarginOption, 0, 100, 1),
		new ZLSimpleSpinOptionEntry("Bottom Margin", TextStyle::BottomMarginOption, 0, 100, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab("Format"));
	myStylePage = new StyleOptionsPage(myDialog->createTab("Styles"), context);

	ZLOptionsDialogTab *indicatorTab = myDialog->createTab("Indicator");
	myIndicatorPage.ShowIndicatorEntry = new ShowIndicatorEntry(myIndicatorPage, "Show Position Indicator", TextView::ShowPositionIndicatorOption);
	myIndicatorPage.HeightEntry = new ZLSimpleSpinOptionEntry("Indicator Height", TextView::PositionIndicatorHeightOption, 1, 100, 1);
	myIndicatorPage.OffsetEntry = new ZLSimpleSpinOptionEntry("Offset From Text", TextView::PositionIndicatorOffsetOption, 0, 100, 1);
	myIndicatorPage.EnableNavigationEntry = new ZLSimpleBooleanOptionEntry("Enable Navigation", TextView::IsIndicatorSensitiveOption);
	indicatorTab->addOption(myIndicatorPage.ShowIndicatorEntry);
	indicatorTab->addOption(myIndicatorPage.HeightEntry);
	indicatorTab->addOption(myIndicatorPage.OffsetEntry);
	indicatorTab->addOption(myIndicatorPage.EnableNavigationEntry);
	myIndicatorPage.ShowIndicatorEntry->onValueChange(myIndicatorPage.ShowIndicatorEntry->initialState());

	myColorPage = new ColorOptionsPage(myDialog->createTab("Colors"));
	if (false) {
		myKeyBindingsPage = new KeyBindingsPage(myDialog->createTab("Keys"));
	} else {
		myKeyBindingsPage = 0;
	}
}

OptionsDialog::~OptionsDialog() {
	delete myFormatPage;
	delete myStylePage;
	delete myColorPage;
	if (myKeyBindingsPage != 0) {
		delete myKeyBindingsPage;
	}
	delete myDialog;
}
