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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLPaintContext.h>
#include <ZLLanguageList.h>
#include <ZLEncodingConverter.h>

#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLLanguageOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLTextStyleCollection.h>
#include <ZLTextStyleOptions.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "StyleOptionsPage.h"
#include "KeyBindingsPage.h"
#include "ConfigPage.h"
#include "NetworkLibraryPage.h"

#include "../fbreader/FBReader.h"
#include "../options/FBOptions.h"
#include "../fbreader/BookTextView.h"

#include "../network/NetworkLinkCollection.h"
#include "../library/Library.h"
#include "../external/ProgramCollection.h"
#include "../formats/FormatPlugin.h"
#include "../encodingOption/EncodingOptionEntry.h"

class RotationTypeEntry : public ZLChoiceOptionEntry {

public:
	RotationTypeEntry(const ZLResource &resource, ZLIntegerOption &angleOption);
	const std::string &text(int index) const;
	int choiceNumber() const;
	int initialCheckedIndex() const;
	void onAccept(int index);

private:
	const ZLResource &myResource;
	ZLIntegerOption &myAngleOption;
};

RotationTypeEntry::RotationTypeEntry(const ZLResource &resource, ZLIntegerOption &angleOption) : myResource(resource), myAngleOption(angleOption) {
}

const std::string &RotationTypeEntry::text(int index) const {
	std::string keyName;
	switch (index) {
		case 1:
			keyName = "counterclockwise";
			break;
		case 2:
			keyName = "180";
			break;
		case 3:
			keyName = "clockwise";
			break;
		case 4:
			keyName = "cycle";
			break;
		default:
			keyName = "disabled";
			break;
	}
	return myResource[keyName].value();
}

int RotationTypeEntry::choiceNumber() const {
	return 5;
}

int RotationTypeEntry::initialCheckedIndex() const {
	switch (myAngleOption.value()) {
		default:
			return 0;
		case ZLView::DEGREES90:
			return 1;
		case ZLView::DEGREES180:
			return 2;
		case ZLView::DEGREES270:
			return 3;
		case -1:
			return 4;
	}
}

void RotationTypeEntry::onAccept(int index) {
	int angle = ZLView::DEGREES0;
	switch (index) {
		case 1:
			angle = ZLView::DEGREES90;
			break;
		case 2:
			angle = ZLView::DEGREES180;
			break;
		case 3:
			angle = ZLView::DEGREES270;
			break;
		case 4:
			angle = -1;
			break;
	}
	myAngleOption.setValue(angle);
}

class OptionsApplyRunnable : public ZLRunnable {

public:
	OptionsApplyRunnable(OptionsDialog &dialog);
	void run();

private:
	OptionsDialog &myDialog;
};

OptionsApplyRunnable::OptionsApplyRunnable(OptionsDialog &dialog) : myDialog(dialog) {
}

void OptionsApplyRunnable::run() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.grabAllKeys(fbreader.KeyboardControlOption.value());
	fbreader.clearTextCaches();
	fbreader.refreshWindow();
}

OptionsDialog::OptionsDialog() {
	FBReader &fbreader = FBReader::Instance();

	myDialog = ZLDialogManager::Instance().createOptionsDialog(ZLResourceKey("OptionsDialog"), new OptionsApplyRunnable(*this), true);

	ZLDialogContent &libraryTab = myDialog->createTab(ZLResourceKey("Library"));
	Library &library = Library::Instance();
	libraryTab.addOption(ZLResourceKey("bookPath"), library.PathOption);
	libraryTab.addOption(ZLResourceKey("lookInSubdirectories"), library.ScanSubdirsOption);
	libraryTab.addOption(ZLResourceKey("collectBooksWithoutMetaInfo"), library.CollectAllBooksOption);
	libraryTab.addOption(ZLResourceKey("downloadDirectory"), NetworkLinkCollection::Instance().DirectoryOption);

	myNetworkLibraryPage = new NetworkLibraryPage(myDialog->createTab(ZLResourceKey("NetworkLibrary")));

	ZLDialogContent &encodingTab = myDialog->createTab(ZLResourceKey("Language"));
	encodingTab.addOption(ZLResourceKey("autoDetect"), new ZLSimpleBooleanOptionEntry(PluginCollection::Instance().LanguageAutoDetectOption));
	encodingTab.addOption(ZLResourceKey("defaultLanguage"), new ZLLanguageOptionEntry(PluginCollection::Instance().DefaultLanguageOption, ZLLanguageList::languageCodes()));
	EncodingEntry *encodingEntry = new EncodingEntry(PluginCollection::Instance().DefaultEncodingOption);
	EncodingSetEntry *encodingSetEntry = new EncodingSetEntry(*encodingEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncodingSet"), encodingSetEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncoding"), encodingEntry);

	ZLDialogContent &scrollingTab = myDialog->createTab(ZLResourceKey("Scrolling"));
	scrollingTab.addOption(ZLResourceKey("keyLinesToScroll"), new ZLSimpleSpinOptionEntry(fbreader.LinesToScrollOption, 1));
	scrollingTab.addOption(ZLResourceKey("keyLinesToKeep"), new ZLSimpleSpinOptionEntry(fbreader.LinesToKeepOption, 1));
	scrollingTab.addOption(ZLResourceKey("keyScrollDelay"), new ZLSimpleSpinOptionEntry(fbreader.KeyScrollingDelayOption, 50));
	const bool hasTouchScreen = 
		ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::TOUCHSCREEN_PRESENTED, false).value();
	if (hasTouchScreen) {
		ZLToggleBooleanOptionEntry *enableTapScrollingEntry =
			new ZLToggleBooleanOptionEntry(fbreader.EnableTapScrollingOption);
		scrollingTab.addOption(ZLResourceKey("enableTapScrolling"), enableTapScrollingEntry);
		const bool isFingerTapDetectionSupported = 
			ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::FINGER_TAP_DETECTABLE, false).value();
		if (isFingerTapDetectionSupported) {
			ZLOptionEntry *fingerOnlyEntry =
				new ZLSimpleBooleanOptionEntry(fbreader.TapScrollingOnFingerOnlyOption);
			scrollingTab.addOption(ZLResourceKey("fingerOnly"), fingerOnlyEntry);
			enableTapScrollingEntry->addDependentEntry(fingerOnlyEntry);
			enableTapScrollingEntry->onStateChanged(enableTapScrollingEntry->initialState());
		}
	}

	ZLDialogContent &selectionTab = myDialog->createTab(ZLResourceKey("Selection"));
	selectionTab.addOption(ZLResourceKey("enableSelection"), FBView::selectionOption());

	ZLDialogContent &cssTab = myDialog->createTab(ZLResourceKey("CSS"));
	cssTab.addOption(ZLResourceKey("overrideSpecifiedFonts"), ZLTextStyleCollection::Instance().OverrideSpecifiedFontsOption);

	ZLDialogContent &marginTab = myDialog->createTab(ZLResourceKey("Margins"));
	FBOptions &options = FBOptions::Instance();
	marginTab.addOptions(
		ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(options.LeftMarginOption, 1),
		ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(options.RightMarginOption, 1)
	);
	marginTab.addOptions(
		ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(options.TopMarginOption, 1),
		ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(options.BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab(ZLResourceKey("Format")));
	myStylePage = new StyleOptionsPage(myDialog->createTab(ZLResourceKey("Styles")), *fbreader.context());

	createIndicatorTab();

	ZLDialogContent &rotationTab = myDialog->createTab(ZLResourceKey("Rotation"));
	ZLResourceKey directionKey("direction");
	rotationTab.addOption(directionKey, new RotationTypeEntry(rotationTab.resource(directionKey), fbreader.RotationAngleOption));

	ZLDialogContent &colorsTab = myDialog->createTab(ZLResourceKey("Colors"));
	ZLResourceKey colorKey("colorFor");
	const ZLResource &resource = colorsTab.resource(colorKey);
	ZLColorOptionBuilder builder;
	const std::string BACKGROUND = resource["background"].value();
	builder.addOption(BACKGROUND, options.BackgroundColorOption);
	builder.addOption(resource["selectionBackground"].value(), options.colorOption(ZLTextStyle::SELECTION_BACKGROUND));
	builder.addOption(resource["text"].value(), options.RegularTextColorOption);
	builder.addOption(resource["internalLink"].value(), options.colorOption("internal"));
	builder.addOption(resource["externalLink"].value(), options.colorOption("external"));
	builder.addOption(resource["bookLink"].value(), options.colorOption("book"));
	builder.addOption(resource["highlighted"].value(), options.colorOption(ZLTextStyle::HIGHLIGHTED_TEXT));
	builder.addOption(resource["treeLines"].value(), options.colorOption(ZLTextStyle::TREE_LINES));
	builder.addOption(resource["indicator"].value(), (FBView::commonIndicatorInfo().ColorOption));
	builder.setInitial(BACKGROUND);
	colorsTab.addOption(colorKey, builder.comboEntry());
	colorsTab.addOption("", "", builder.colorEntry());

	myKeyBindingsPage = new KeyBindingsPage(myDialog->createTab(ZLResourceKey("Keys")));
	if (ZLOption::isAutoSavingSupported()) {
		myConfigPage = new ConfigPage(myDialog->createTab(ZLResourceKey("Config")));
	}

	std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	additional.push_back(std::pair<ZLResourceKey,ZLOptionEntry*>(
		ZLResourceKey("singleClickOpen"),
		new ZLSimpleBooleanOptionEntry(fbreader.EnableSingleClickDictionaryOption)
	));
	createIntegrationTab(fbreader.dictionaryCollection(), ZLResourceKey("Dictionary"), additional);
	additional.clear();
	createIntegrationTab(fbreader.webBrowserCollection(), ZLResourceKey("Web"), additional);

	myDialog->createPlatformDependentTabs();
}

void OptionsDialog::storeTemporaryOption(ZLOption *option) {
	myTemporaryOptions.push_back(option);
}

OptionsDialog::~OptionsDialog() {
}
