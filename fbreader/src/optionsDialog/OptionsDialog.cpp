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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLPaintContext.h>

#include <optionEntries/ZLToggleBooleanOptionEntry.h>
#include <optionEntries/ZLColorOptionBuilder.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "ScrollingOptionsPage.h"
#include "StyleOptionsPage.h"
#include "KeyBindingsPage.h"
#include "ConfigPage.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/BookTextView.h"
#include "../fbreader/CollectionView.h"
#include "../fbreader/RecentBooksView.h"

#include "../collection/BookCollection.h"
#include "../external/ProgramCollection.h"
#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"
#include "../formats/FormatPlugin.h"

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
	return myResource[ZLResourceKey(keyName)].value();
}

int RotationTypeEntry::choiceNumber() const {
	return 5;
}

int RotationTypeEntry::initialCheckedIndex() const {
	switch (myAngleOption.value()) {
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
	myAngleOption.setValue(angle);
}

class DefaultLanguageEntry : public ZLComboOptionEntry {

public:
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
};

static std::string LANGUAGE_RUSSIAN = "Russian";
static std::string LANGUAGE_CHINESE = "Chinese";
static std::string LANGUAGE_CZECH = "Czech";
static std::string LANGUAGE_OTHER = "Other";

const std::string &DefaultLanguageEntry::initialValue() const {
	switch (PluginCollection::instance().DefaultLanguageOption.value()) {
		case EncodingDetector::RUSSIAN:
			return LANGUAGE_RUSSIAN;
		case EncodingDetector::CHINESE:
			return LANGUAGE_CHINESE;
		case EncodingDetector::CZECH:
			return LANGUAGE_CZECH;
		default:
			return LANGUAGE_OTHER;
	}
}

const std::vector<std::string> &DefaultLanguageEntry::values() const {
	static std::vector<std::string> _values;
	if (_values.empty()) {
		_values.push_back(LANGUAGE_CHINESE);
		_values.push_back(LANGUAGE_CZECH);
		_values.push_back(LANGUAGE_RUSSIAN);
		_values.push_back(LANGUAGE_OTHER);
	}
	return _values;
}

void DefaultLanguageEntry::onAccept(const std::string &value) {
	EncodingDetector::Language language = EncodingDetector::OTHER;
	if (value == LANGUAGE_RUSSIAN) {
		language = EncodingDetector::RUSSIAN;
	} else if (value == LANGUAGE_CHINESE) {
		language = EncodingDetector::CHINESE;
	} else if (value == LANGUAGE_CZECH) {
		language = EncodingDetector::CZECH;
	}
	PluginCollection::instance().DefaultLanguageOption.setValue(language);
}

class OptionsApplyRunnable : public ZLRunnable {

public:
	OptionsApplyRunnable(FBReader &fbreader);
	void run();

private:
	FBReader &myFBReader;
};

OptionsApplyRunnable::OptionsApplyRunnable(FBReader &fbreader) : myFBReader(fbreader) {
}

void OptionsApplyRunnable::run() {
	myFBReader.grabAllKeys(myFBReader.KeyboardControlOption.value());
	myFBReader.clearTextCaches();
	((CollectionView&)*myFBReader.myCollectionView).synchronizeModel();
	myFBReader.refreshWindow();
}

OptionsDialog::OptionsDialog(FBReader &fbreader) {
	myDialog = ZLDialogManager::instance().createOptionsDialog(ZLResourceKey("OptionsDialog"), new OptionsApplyRunnable(fbreader), true);

	ZLDialogContent &generalTab = myDialog->createTab(ZLResourceKey("General"));
	generalTab.addOption(ZLResourceKey("quitOnCancel"), fbreader.QuitOnCancelOption);
	CollectionView &collectionView = (CollectionView&)*fbreader.myCollectionView;
	generalTab.addOption(ZLResourceKey("bookPath"), collectionView.collection().PathOption);
	generalTab.addOption(ZLResourceKey("lookInSubdirectories"), collectionView.collection().ScanSubdirsOption);
	RecentBooksView &recentBooksView = (RecentBooksView&)*fbreader.myRecentBooksView;
	generalTab.addOption(ZLResourceKey("recentListSize"), new ZLSimpleSpinOptionEntry(recentBooksView.lastBooks().MaxListSizeOption, 1));
	generalTab.addOption(ZLResourceKey("keyDelay"), new ZLSimpleSpinOptionEntry(fbreader.KeyDelayOption, 50));
	generalTab.addOption(ZLResourceKey("defaultLanguage"), new DefaultLanguageEntry());

	myScrollingPage = new ScrollingOptionsPage(myDialog->createTab(ZLResourceKey("Scrolling")), fbreader);

	ZLDialogContent &marginTab = myDialog->createTab(ZLResourceKey("Margins"));
	marginTab.addOptions(
		ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(TextStyleCollection::instance().baseStyle().LeftMarginOption, 1),
		ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(TextStyleCollection::instance().baseStyle().RightMarginOption, 1)
	);
	marginTab.addOptions(
		ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(TextStyleCollection::instance().baseStyle().TopMarginOption, 1),
		ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(TextStyleCollection::instance().baseStyle().BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab(ZLResourceKey("Format")));
	myStylePage = new StyleOptionsPage(myDialog->createTab(ZLResourceKey("Styles")), fbreader.context());

	createIndicatorTab(fbreader);

	ZLDialogContent &rotationTab = myDialog->createTab(ZLResourceKey("Rotation"));
	ZLResourceKey directionKey("direction");
	rotationTab.addOption(directionKey, new RotationTypeEntry(rotationTab.resource(directionKey), fbreader.RotationAngleOption));

	ZLDialogContent &colorsTab = myDialog->createTab(ZLResourceKey("Colors"));
	ZLResourceKey colorKey("colorFor");
	const ZLResource &resource = colorsTab.resource(colorKey);
	ZLColorOptionBuilder builder;
	const std::string BACKGROUND = resource[ZLResourceKey("background")].value();
	BaseTextStyle &baseStyle = TextStyleCollection::instance().baseStyle();
	builder.addOption(BACKGROUND, baseStyle.BackgroundColorOption);
	builder.addOption(resource[ZLResourceKey("selectionBackground")].value(), baseStyle.SelectionBackgroundColorOption);
	builder.addOption(resource[ZLResourceKey("text")].value(), baseStyle.RegularTextColorOption);
	builder.addOption(resource[ZLResourceKey("internalLink")].value(), baseStyle.InternalHyperlinkTextColorOption);
	builder.addOption(resource[ZLResourceKey("externalLink")].value(), baseStyle.ExternalHyperlinkTextColorOption);
	builder.addOption(resource[ZLResourceKey("highlighted")].value(), baseStyle.SelectedTextColorOption);
	builder.addOption(resource[ZLResourceKey("treeLines")].value(), baseStyle.TreeLinesColorOption);
	builder.addOption(resource[ZLResourceKey("indicator")].value(), TextStyleCollection::instance().indicatorStyle().ColorOption);
	builder.setInitial(BACKGROUND);
	colorsTab.addOption(colorKey, builder.comboEntry());
	colorsTab.addOption("", "", builder.colorEntry());

	myKeyBindingsPage = new KeyBindingsPage(fbreader, myDialog->createTab(ZLResourceKey("Keys")));
	if (ZLOption::isAutoSavingSupported()) {
		myConfigPage = new ConfigPage(fbreader, myDialog->createTab(ZLResourceKey("Config")));
	}

	std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	additional.push_back(std::pair<ZLResourceKey,ZLOptionEntry*>(
		ZLResourceKey("singleClickOpen"),
		new ZLSimpleBooleanOptionEntry(fbreader.EnableSingleClickDictionaryOption)
	));
	createIntegrationTab(fbreader.dictionaryCollection(), ZLResourceKey("Dictionary"), additional);
	additional.clear();
	createIntegrationTab(fbreader.webBrowserCollection(), ZLResourceKey("Web"), additional);
}

void OptionsDialog::storeTemporaryOption(ZLOption *option) {
	myTemporaryOptions.push_back(option);
}

OptionsDialog::~OptionsDialog() {
}
