/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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
#include <ZLNetworkManager.h>

#include <optionEntries/ZLToggleBooleanOptionEntry.h>
#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLLanguageOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>

#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLTextStyleOptions.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "ScrollingOptionsPage.h"
#include "StyleOptionsPage.h"
#include "KeyBindingsPage.h"
#include "ConfigPage.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/BookTextView.h"
#include "../fbreader/CollectionView.h"

#include "../network/NetworkLink.h"
#include "../collection/BookCollection.h"
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
	ZLTextStyleCollection &collection = ZLTextStyleCollection::instance();
	ZLTextBaseStyle &baseStyle = collection.baseStyle();

	myDialog = ZLDialogManager::instance().createOptionsDialog(ZLResourceKey("OptionsDialog"), new OptionsApplyRunnable(fbreader), true);

	ZLDialogContent &libraryTab = myDialog->createTab(ZLResourceKey("Library"));
	CollectionView &collectionView = fbreader.collectionView();
	libraryTab.addOption(ZLResourceKey("bookPath"), collectionView.collection().PathOption);
	libraryTab.addOption(ZLResourceKey("lookInSubdirectories"), collectionView.collection().ScanSubdirsOption);
	libraryTab.addOption(ZLResourceKey("collectBooksWithoutMetaInfo"), collectionView.collection().CollectAllBooksOption);
	libraryTab.addOption(ZLResourceKey("downloadDirectory"), NetworkLinkCollection::instance().DirectoryOption);
	libraryTab.addOption(ZLResourceKey("showAllBooksTag"), collectionView.ShowAllBooksTagOption);

	ZLDialogContent &networkTab = myDialog->createTab(ZLResourceKey("NetworkLibrary"));
	NetworkLinkCollection &linkCollection = NetworkLinkCollection::instance();
	const size_t linkCollectionSize = linkCollection.size();
	for (size_t i = 0; i < linkCollectionSize; ++i) {
		NetworkLink &link = linkCollection.link(i);
		networkTab.addOption(link.SiteName, "", new ZLSimpleBooleanOptionEntry(link.OnOption));
	}
	ZLNetworkManager &networkManager = ZLNetworkManager::instance();
	if (!networkManager.providesProxyInfo()) {
		ZLToggleBooleanOptionEntry *useProxyEntry = new ZLToggleBooleanOptionEntry(networkManager.UseProxyOption());
		networkTab.addOption(ZLResourceKey("useProxy"), useProxyEntry);
		ZLSimpleStringOptionEntry *proxyHostEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyHostOption());
		networkTab.addOption(ZLResourceKey("proxyHost"), proxyHostEntry);
		ZLSimpleStringOptionEntry *proxyPortEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyPortOption());
		networkTab.addOption(ZLResourceKey("proxyPort"), proxyPortEntry);
		useProxyEntry->addDependentEntry(proxyHostEntry);
		useProxyEntry->addDependentEntry(proxyPortEntry);
		useProxyEntry->onStateChanged(useProxyEntry->initialState());
	}
	networkTab.addOption(ZLResourceKey("timeout"), new ZLSimpleSpinOptionEntry(networkManager.TimeoutOption(), 5));

	ZLDialogContent &encodingTab = myDialog->createTab(ZLResourceKey("Language"));
	encodingTab.addOption(ZLResourceKey("autoDetect"), new ZLSimpleBooleanOptionEntry(PluginCollection::instance().LanguageAutoDetectOption));
	encodingTab.addOption(ZLResourceKey("defaultLanguage"), new ZLLanguageOptionEntry(PluginCollection::instance().DefaultLanguageOption, ZLLanguageList::languageCodes()));
	EncodingEntry *encodingEntry = new EncodingEntry(PluginCollection::instance().DefaultEncodingOption);
	EncodingSetEntry *encodingSetEntry = new EncodingSetEntry(*encodingEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncodingSet"), encodingSetEntry);
	encodingTab.addOption(ZLResourceKey("defaultEncoding"), encodingEntry);
	encodingTab.addOption(ZLResourceKey("useWindows1252Hack"), new ZLSimpleBooleanOptionEntry(ZLEncodingCollection::useWindows1252HackOption()));

	myScrollingPage = new ScrollingOptionsPage(myDialog->createTab(ZLResourceKey("Scrolling")), fbreader);

	ZLDialogContent &selectionTab = myDialog->createTab(ZLResourceKey("Selection"));
	selectionTab.addOption(ZLResourceKey("enableSelection"), FBView::selectionOption());

	ZLDialogContent &cssTab = myDialog->createTab(ZLResourceKey("CSS"));
	cssTab.addOption(ZLResourceKey("overrideSpecifiedFonts"), ZLTextStyleCollection::instance().OverrideSpecifiedFontsOption);

	ZLDialogContent &marginTab = myDialog->createTab(ZLResourceKey("Margins"));
	FBMargins &margins = FBView::margins();
	marginTab.addOptions(
		ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(margins.LeftMarginOption, 1),
		ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(margins.RightMarginOption, 1)
	);
	marginTab.addOptions(
		ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(margins.TopMarginOption, 1),
		ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(margins.BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab(ZLResourceKey("Format")));
	myStylePage = new StyleOptionsPage(myDialog->createTab(ZLResourceKey("Styles")), *fbreader.context());

	createIndicatorTab(fbreader);

	ZLDialogContent &rotationTab = myDialog->createTab(ZLResourceKey("Rotation"));
	ZLResourceKey directionKey("direction");
	rotationTab.addOption(directionKey, new RotationTypeEntry(rotationTab.resource(directionKey), fbreader.RotationAngleOption));

	ZLDialogContent &colorsTab = myDialog->createTab(ZLResourceKey("Colors"));
	ZLResourceKey colorKey("colorFor");
	const ZLResource &resource = colorsTab.resource(colorKey);
	ZLColorOptionBuilder builder;
	const std::string BACKGROUND = resource["background"].value();
	builder.addOption(BACKGROUND, baseStyle.BackgroundColorOption);
	builder.addOption(resource["selectionBackground"].value(), baseStyle.SelectionBackgroundColorOption);
	builder.addOption(resource["text"].value(), baseStyle.RegularTextColorOption);
	builder.addOption(resource["internalLink"].value(), baseStyle.hyperlinkColorOption("internal"));
	builder.addOption(resource["externalLink"].value(), baseStyle.hyperlinkColorOption("external"));
	builder.addOption(resource["bookLink"].value(), baseStyle.hyperlinkColorOption("book"));
	builder.addOption(resource["highlighted"].value(), baseStyle.SelectedTextColorOption);
	builder.addOption(resource["treeLines"].value(), baseStyle.TreeLinesColorOption);
	builder.addOption(resource["indicator"].value(), (FBView::commonIndicatorInfo().ColorOption));
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

	myDialog->createPlatformDependentTabs();
}

void OptionsDialog::storeTemporaryOption(ZLOption *option) {
	myTemporaryOptions.push_back(option);
}

OptionsDialog::~OptionsDialog() {
}
