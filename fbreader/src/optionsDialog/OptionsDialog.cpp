/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include <ZLLanguageList.h>
#include <ZLStringUtil.h>
#include <ZLNetworkManager.h>

#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLLanguageOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLStringInfoEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include <ZLTextStyleCollection.h>

#include "OptionsDialog.h"
#include "AuthorDisplayNameEntry.h"
#include "BookTagEntry.h"
#include "FormatOptionsPage.h"
#include "RotationTypeEntry.h"
#include "SeriesTitleEntry.h"
#include "SeriesIndexEntry.h"
#include "StyleOptionsPage.h"

#include "../encodingOption/EncodingOptionEntry.h"
#include "../options/FBOptions.h"
#include "../fbreader/FBReader.h"
#include "../fbreader/FBView.h"
#include "../library/Library.h"
#include "../library/Book.h"
#include "../library/Tag.h"
#include "../library/Author.h"
#include "../network/NetworkLinkCollection.h"

static const std::size_t AUTHOR_ENTRIES_POOL_SIZE = 64;
static const std::size_t TAG_ENTRIES_POOL_SIZE = 64;

class BookTitleEntry : public ZLStringOptionEntry {

public:
	BookTitleEntry(OptionsDialog &dialog);

	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	OptionsDialog &myInfoDialog;
};

BookTitleEntry::BookTitleEntry(OptionsDialog &dialog) : myInfoDialog(dialog) {
}

const std::string &BookTitleEntry::initialValue() const {
	return myInfoDialog.myBook->title();
}

void BookTitleEntry::onAccept(const std::string &value) {
	myInfoDialog.myBook->setTitle(value);
}






class BookEncodingEntry : public AbstractEncodingEntry {

public:
	BookEncodingEntry(OptionsDialog &dialog);

	void onAcceptValue(const std::string &value);

private:
	OptionsDialog &myInfoDialog;
};

BookEncodingEntry::BookEncodingEntry(OptionsDialog &dialog) :
	AbstractEncodingEntry(dialog.myBook->encoding()),
	myInfoDialog(dialog) {
}

void BookEncodingEntry::onAcceptValue(const std::string &value) {
	myInfoDialog.myBook->setEncoding(value);
}



class BookLanguageEntry : public ZLAbstractLanguageOptionEntry {

public:
	BookLanguageEntry(OptionsDialog &dialog, const std::vector<std::string> &languageCodes);

	void onAcceptCode(const std::string &code);

private:
	OptionsDialog &myInfoDialog;
};

BookLanguageEntry::BookLanguageEntry(OptionsDialog &dialog, const std::vector<std::string> &languageCodes) :
	ZLAbstractLanguageOptionEntry(dialog.myBook->language(), languageCodes),
	myInfoDialog(dialog) {
}

void BookLanguageEntry::onAcceptCode(const std::string &code) {
	myInfoDialog.myBook->setLanguage(code);
}


OptionsDialog::ApplyAction::ApplyAction(OptionsDialog &dialog) : myInfoDialog(dialog) {}

void OptionsDialog::ApplyAction::run() {
	Book &book = *myInfoDialog.myBook;

	AuthorList authors;
	for (std::size_t i = 0; i < myInfoDialog.myAuthorEntries.size(); ++i) {
		shared_ptr<Author> a = myInfoDialog.myAuthorEntries[i]->myCurrentAuthor;
		if (!a.isNull() &&
				std::find(authors.begin(), authors.end(), a) == authors.end()) {
			authors.push_back(a);
		}
	}

	book.removeAllAuthors();
	for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		book.addAuthor(*it);
	}

	book.removeAllTags();
	for (std::size_t i = 0; i < myInfoDialog.myNewTags.size(); ++i) {
		book.addTag(myInfoDialog.myNewTags[i]);
	}

	Library::Instance().updateBook(myInfoDialog.myBook);

	FBReader &fbreader = FBReader::Instance();
	fbreader.grabAllKeys(fbreader.KeyboardControlOption.value());
	fbreader.clearTextCaches();
	fbreader.refreshWindow();
}

OptionsDialog::OptionsDialog(shared_ptr<Book> book) : myBook(book) {
	myDialog = ZLDialogManager::Instance().createOptionsDialog(ZLResourceKey("Preferences"), new ApplyAction(*this));

	FBReader &fbreader = FBReader::Instance();
	FBOptions &options = FBOptions::Instance();
	Library &library = Library::Instance();
	PluginCollection &pluginCollection = PluginCollection::Instance();

	// ++++ directories page ++++
	ZLDialogContent &libraryTab = myDialog->createTab(ZLResourceKey("directories"), ZLResourceKey("directoriesLibrary"));
	libraryTab.addOption(ZLResourceKey("bookPath"), library.PathOption);
	libraryTab.addOption(ZLResourceKey("lookInSubdirectories"), library.ScanSubdirsOption);
	libraryTab.addOption(ZLResourceKey("collectBooksWithNoMetainfo"), library.CollectAllBooksOption);
	libraryTab.addOption(ZLResourceKey("downloadDirectory"), NetworkLinkCollection::Instance().DirectoryOption);
	// ---- directories page ----

	// ++++ text properties page ++++
	ZLDialogContent &marginTab = myDialog->createTab(ZLResourceKey("text"), ZLResourceKey("margins"));
	marginTab.addOptions(
		ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(options.LeftMarginOption, 1),
		ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(options.RightMarginOption, 1)
	);
	marginTab.addOptions(
		ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(options.TopMarginOption, 1),
		ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(options.BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(myDialog->createTab(ZLResourceKey("text"), ZLResourceKey("Format")));
	myStylePage = new StyleOptionsPage(myDialog->createTab(ZLResourceKey("text"), ZLResourceKey("Styles")), *fbreader.context());

	ZLDialogContent &colorsTab = myDialog->createTab(ZLResourceKey("text"), ZLResourceKey("Colors"));
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
	// ---- text properties page ----

	// ++++ look & feel page ++++
	ZLDialogContent &scrollingTab = myDialog->createTab(ZLResourceKey("lookAndFeel"), ZLResourceKey("scrolling"));
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

	ZLDialogContent &selectionTab = myDialog->createTab(ZLResourceKey("lookAndFeel"), ZLResourceKey("Selection"));
	selectionTab.addOption(ZLResourceKey("enableSelection"), FBView::selectionOption());

	createIndicatorTab();

/*
	ZLDialogContent &rotationTab = myDialog->createTab(ZLResourceKey("lookAndFeel"), ZLResourceKey("Rotation"));
	ZLResourceKey directionKey("direction");
	rotationTab.addOption(directionKey, new RotationTypeEntry(rotationTab.resource(directionKey), fbreader.RotationAngleOption));
*/

	createKeyBindingsTab();

	ZLDialogContent &cssTab = myDialog->createTab(ZLResourceKey("lookAndFeel"), ZLResourceKey("CSS"));
	cssTab.addOption(ZLResourceKey("overrideSpecifiedFonts"), ZLTextStyleCollection::Instance().OverrideSpecifiedFontsOption);
	// ---- look & feel page ----

	// ++++ network page ++++
	ZLDialogContent &connectionTab = myDialog->createTab(ZLResourceKey("network"), ZLResourceKey("connection"));
	ZLNetworkManager &networkManager = ZLNetworkManager::Instance();
	connectionTab.addOption(ZLResourceKey("timeout"), new ZLSimpleSpinOptionEntry(networkManager.TimeoutOption(), 5));
	if (!networkManager.providesProxyInfo()) {
		ZLToggleBooleanOptionEntry *useProxyEntry = new ZLToggleBooleanOptionEntry(networkManager.UseProxyOption());
		connectionTab.addOption(ZLResourceKey("useProxy"), useProxyEntry);
		ZLSimpleStringOptionEntry *proxyHostEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyHostOption());
		connectionTab.addOption(ZLResourceKey("proxyHost"), proxyHostEntry);
		ZLSimpleStringOptionEntry *proxyPortEntry = new ZLSimpleStringOptionEntry(networkManager.ProxyPortOption());
		connectionTab.addOption(ZLResourceKey("proxyPort"), proxyPortEntry);
		useProxyEntry->addDependentEntry(proxyHostEntry);
		useProxyEntry->addDependentEntry(proxyPortEntry);
		useProxyEntry->onStateChanged(useProxyEntry->initialState());
	}

	//std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	//createIntegrationTab(fbreader.webBrowserCollection(), ZLResourceKey("Web"), additional);

	// ---- network page ----

	// ++++ languages page ++++
	ZLDialogContent &encodingTab = myDialog->createTab(ZLResourceKey("language"), ZLResourceKey("defaultLanguageForBook"));
	encodingTab.addOption(ZLResourceKey("language"), new ZLLanguageOptionEntry(pluginCollection.DefaultLanguageOption, ZLLanguageList::languageCodes()));
	EncodingEntry *encodingEntry = new EncodingEntry(pluginCollection.DefaultEncodingOption);
	EncodingSetEntry *encodingSetEntry = new EncodingSetEntry(*encodingEntry);
	encodingTab.addOption(ZLResourceKey("encodingSet"), encodingSetEntry);
	encodingTab.addOption(ZLResourceKey("encoding"), encodingEntry);

	std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > additional;
	ZLOptionEntry *entry =
		new ZLSimpleBooleanOptionEntry(fbreader.EnableSingleClickDictionaryOption);
	additional.push_back(std::make_pair(ZLResourceKey("singleClickOpen"), entry));
	//createIntegrationTab(fbreader.dictionaryCollection(), ZLResourceKey("Dictionary"), additional);

	//myDialog.createPlatformDependentTabs();
	// ---- languages page ----

	// ++++ current book page ++++
	ZLDialogContent &commonTab = myDialog->createTab(ZLResourceKey("book"), ZLResourceKey("bookGeneral"));
	commonTab.addOption(
		ZLResourceKey("file"), new ZLStringInfoEntry(ZLFile::fileNameToUtf8(book->file().path()))
	);
	commonTab.addOption(ZLResourceKey("title"), new BookTitleEntry(*this));

	myEncodingEntry = new BookEncodingEntry(*this);
	myEncodingSetEntry =
		(myEncodingEntry->initialValue() != Book::AutoEncoding) ?
		new EncodingSetEntry(*(EncodingEntry*)myEncodingEntry) : 0;
	std::vector<std::string> languageCodes = ZLLanguageList::languageCodes();
	languageCodes.push_back("de-traditional");
	myLanguageEntry = new BookLanguageEntry(*this, languageCodes);
	mySeriesTitleEntry = new SeriesTitleEntry(*this);
	mySeriesIndexEntry = new SeriesIndexEntry(*this);

	commonTab.addOption(ZLResourceKey("language"), myLanguageEntry);
	if (myEncodingSetEntry != 0) {
		commonTab.addOption(ZLResourceKey("encodingSet"), myEncodingSetEntry);
	}
	commonTab.addOption(ZLResourceKey("encoding"), myEncodingEntry);

	initAuthorEntries();

	ZLDialogContent &seriesTab = myDialog->createTab(ZLResourceKey("book"), ZLResourceKey("series"));
	seriesTab.addOption(ZLResourceKey("seriesTitle"), mySeriesTitleEntry);
	seriesTab.addOption(ZLResourceKey("bookIndex"), mySeriesIndexEntry);

	mySeriesTitleEntry->onValueEdited(mySeriesTitleEntry->initialValue());
	/*
	ZLOrderOptionEntry *orderEntry = new ZLOrderOptionEntry();
	orderEntry->values().push_back("First");
	orderEntry->values().push_back("Second");
	orderEntry->values().push_back("Third");
	orderEntry->values().push_back("Fourth");
	orderEntry->values().push_back("Fifth");
	seriesTab.addOption(orderEntry);
	*/

	initTagEntries();

	shared_ptr<FormatPlugin> plugin = pluginCollection.plugin(*book);
	if (!plugin.isNull()) {
		myFormatInfoPage = plugin->createInfoPage(*myDialog, book->file());
	}
	// ---- current book page ----
}

OptionsDialog::~OptionsDialog() {
}

void OptionsDialog::initTagEntries() {
	bool visible = true;
	const TagList &tags = myBook->tags();
	myTagsDone = false;
	myTagsTab = &myDialog->createTab(ZLResourceKey("book"), ZLResourceKey("tags"));
	for (std::size_t i = 0; i < TAG_ENTRIES_POOL_SIZE; ++i) {
		std::string tag = (i < tags.size()) ? tags[i]->fullName() : "";
		BookTagEntry *entry = new BookTagEntry(*this, tag, visible);
		myTagEntries.push_back(entry);
		myTagsTab->addOption(ZLResourceKey("tags"), entry);
	}
	myTagsDone = true;
}

void OptionsDialog::initAuthorEntries() {
	bool visible = true;
	const AuthorList &authors = myBook->authors();
	myAuthorsDone = false;
	myAuthorsTab = &myDialog->createTab(ZLResourceKey("book"), ZLResourceKey("authors"));
	for (std::size_t i = 0; i < AUTHOR_ENTRIES_POOL_SIZE; ++i) {
		shared_ptr<Author> author = (i < authors.size()) ? authors[i] : 0;
		AuthorDisplayNameEntry *entry = new AuthorDisplayNameEntry(*this, author, visible);
		myAuthorEntries.push_back(entry);
		myAuthorsTab->addOption(ZLResourceKey("authorDisplayName"), entry);
	}
	myAuthorsDone = true;
}
