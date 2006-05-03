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

#include <abstract/ZLDialogManager.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLPaintContext.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "ScrollingOptionsPage.h"
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
	ShowIndicatorEntry(IndicatorPage &page, const std::string &name, ZLBooleanOption &option);
	~ShowIndicatorEntry();
	void onValueChange(bool value);

private:
	IndicatorPage &myPage;
};

ShowIndicatorEntry::ShowIndicatorEntry(IndicatorPage &page, const std::string &name, ZLBooleanOption &option) : ZLSimpleBooleanOptionEntry(name, option), myPage(page) {
}

ShowIndicatorEntry::~ShowIndicatorEntry() {
}

void ShowIndicatorEntry::onValueChange(bool value) {
	myPage.HeightEntry->setVisible(value);
	myPage.OffsetEntry->setVisible(value);
	myPage.EnableNavigationEntry->setVisible(value);
}

class RotationTypeEntry : public ZLChoiceOptionEntry {

public:
	RotationTypeEntry() {}
	const std::string &name() const;
	const std::string &text(int index) const;
	int choiceNumber() const;
	int initialCheckedIndex() const;
	void onAccept(int index);
};

static const std::string ROTATION_TYPE = "Rotation Type";
static const std::string ROTATION_0 = "Disabled";
static const std::string ROTATION_90 = "90 Degrees Counterclockwise";
static const std::string ROTATION_180 = "180 Degrees";
static const std::string ROTATION_270 = "90 Degrees Clockwise";
static const std::string ROTATION_USE_4_DIRECTIONS = "Cycle Through All 4 Directions";

const std::string &RotationTypeEntry::name() const {
	return ROTATION_TYPE;
}

const std::string &RotationTypeEntry::text(int index) const {
	switch (index) {
		case 1:
			return ROTATION_90;
		case 2:
			return ROTATION_180;
		case 3:
			return ROTATION_270;
		case 4:
			return ROTATION_USE_4_DIRECTIONS;
		default:
			return ROTATION_0;
	}
}

int RotationTypeEntry::choiceNumber() const {
	return 5;
}

int RotationTypeEntry::initialCheckedIndex() const {
	switch (FBReader::RotationAngleOption.value()) {
		default:
			return 0;
		case ZLViewWidget::DEGREES90:
			return 1;
		case ZLViewWidget::DEGREES180:
			return 2;
		case ZLViewWidget::DEGREES270:
			return 3;
		case -1:
			return 4;
	}
}

void RotationTypeEntry::onAccept(int index) {
	int angle = ZLViewWidget::DEGREES0;
	switch (index) {
		case 1:
			angle = ZLViewWidget::DEGREES90;
			break;
		case 2:
			angle = ZLViewWidget::DEGREES180;
			break;
		case 3:
			angle = ZLViewWidget::DEGREES270;
			break;
		case 4:
			angle = -1;
			break;
	}
	FBReader::RotationAngleOption.setValue(angle);
}


OptionsDialog::OptionsDialog(FBReader &fbreader, ZLPaintContext &context) {
	myDialog = ZLDialogManager::instance().createOptionsDialog("OptionsDialog", "FBReader - Options");

	ZLOptionsDialogTab *generalTab = myDialog->createTab("General");
	generalTab->addOption(new ZLSimpleBooleanOptionEntry("Quit Application On Cancel", FBReader::QuitOnCancelOption));
	generalTab->addOption(new ZLSimpleStringOptionEntry("Book Path", BookCollection::PathOption));
	generalTab->addOption(new ZLSimpleBooleanOptionEntry("Look For Books In Subdirectories", BookCollection::ScanSubdirsOption));
	generalTab->addOption(new ZLSimpleSpinOptionEntry("Recent Books List Size", LastOpenedBooks::MaxListSizeOption, 1));
	generalTab->addOption(new ZLSimpleBooleanOptionEntry("Keep Position In Contents View", FBReader::StoreContentsPositionOption));
	generalTab->addOption(new ZLSimpleSpinOptionEntry("Delay Between Accepted Key Pressings", FBReader::KeyDelayOption, 50));

	myScrollingPage = new ScrollingOptionsPage(myDialog->createTab("Scrolling"));

	ZLOptionsDialogTab *marginTab = myDialog->createTab("Margins");
	marginTab->addOptions(
		new ZLSimpleSpinOptionEntry("Left Margin", TextStyle::LeftMarginOption, 1),
		new ZLSimpleSpinOptionEntry("Right Margin", TextStyle::RightMarginOption, 1)
	);
	marginTab->addOptions(
		new ZLSimpleSpinOptionEntry("Top Margin", TextStyle::TopMarginOption, 1),
		new ZLSimpleSpinOptionEntry("Bottom Margin", TextStyle::BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab("Format"));
	myStylePage = new StyleOptionsPage(myDialog->createTab("Styles"), context);

	ZLOptionsDialogTab *indicatorTab = myDialog->createTab("Indicator");
	myIndicatorPage.ShowIndicatorEntry = new ShowIndicatorEntry(myIndicatorPage, "Show Position Indicator", TextView::ShowPositionIndicatorOption);
	myIndicatorPage.HeightEntry = new ZLSimpleSpinOptionEntry("Indicator Height", TextView::PositionIndicatorHeightOption, 1);
	myIndicatorPage.OffsetEntry = new ZLSimpleSpinOptionEntry("Offset From Text", TextView::PositionIndicatorOffsetOption, 1);
	myIndicatorPage.EnableNavigationEntry = new ZLSimpleBooleanOptionEntry("Enable Navigation", TextView::IsIndicatorSensitiveOption);
	indicatorTab->addOption(myIndicatorPage.ShowIndicatorEntry);
	indicatorTab->addOption(myIndicatorPage.HeightEntry);
	indicatorTab->addOption(myIndicatorPage.OffsetEntry);
	indicatorTab->addOption(myIndicatorPage.EnableNavigationEntry);
	myIndicatorPage.ShowIndicatorEntry->onValueChange(myIndicatorPage.ShowIndicatorEntry->initialState());

	if (fbreader.isRotationSupported()) {
		ZLOptionsDialogTab *rotationTab = myDialog->createTab("Rotation");
		rotationTab->addOption(new RotationTypeEntry());
	}

	myColorPage = new ColorOptionsPage(myDialog->createTab("Colors"));
	myKeyBindingsPage = new KeyBindingsPage(fbreader, myDialog->createTab("Keys"));
}

OptionsDialog::~OptionsDialog() {
	delete myScrollingPage;
	delete myFormatPage;
	delete myStylePage;
	delete myColorPage;
	delete myKeyBindingsPage;
	delete myDialog;
}
