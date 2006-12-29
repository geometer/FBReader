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

#include <queue>

#include <ZLFile.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLDialog.h>
#include <ZLDir.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "FBReader.h"
#include "BookTextView.h"
#include "FootnoteView.h"
#include "ContentsView.h"
#include "CollectionView.h"
#include "RecentBooksView.h"
#include "BookInfoDialog.h"
#include "FBFileHandler.h"

#include "../FBOptions.h"
#include "../optionsDialog/OptionsDialog.h"
#include "../bookmodel/BookModel.h"
#include "../collection/BookList.h"
#include "../hyphenation/Hyphenator.h"
#include "../formats/FormatPlugin.h"

static const std::string OPTIONS = "Options";
static const std::string SEARCH = "Search";
static const std::string STATE = "State";
static const std::string BOOK = "Book";

const std::string LARGE_SCROLLING = "LargeScrolling";
const std::string SMALL_SCROLLING = "SmallScrolling";
const std::string MOUSE_SCROLLING = "MouseScrolling";
const std::string FINGER_TAP_SCROLLING = "FingerTapScrolling";

const std::string DELAY = "ScrollingDelay";
const std::string MODE = "Mode";
const std::string LINES_TO_KEEP = "LinesToKeep";
const std::string LINES_TO_SCROLL = "LinesToScroll";
const std::string PERCENT_TO_SCROLL = "PercentToScroll";

FBReader::ScrollingOptions::ScrollingOptions(
	const std::string &delayGroup, const std::string &delayName, long delayValue,
	const std::string &modeGroup, const std::string &modeName, long modeValue,
	const std::string &linesToKeepGroup, const std::string &linesToKeepName, long linesToKeepValue,
	const std::string &linesToScrollGroup, const std::string &linesToScrollName, long linesToScrollValue,
	const std::string &percentToScrollGroup, const std::string &percentToScrollName, long percentToScrollValue
) : DelayOption(ZLOption::CONFIG_CATEGORY, delayGroup, delayName, 0, 5000, delayValue),
		ModeOption(ZLOption::CONFIG_CATEGORY, modeGroup, modeName, modeValue),
		LinesToKeepOption(ZLOption::CONFIG_CATEGORY, linesToKeepGroup, linesToKeepName, 1, 100, linesToKeepValue),
		LinesToScrollOption(ZLOption::CONFIG_CATEGORY, linesToScrollGroup, linesToScrollName, 1, 100, linesToScrollValue),
		PercentToScrollOption(ZLOption::CONFIG_CATEGORY, percentToScrollGroup, percentToScrollName, 1, 100, percentToScrollValue) {}

FBReader::FBReader(const std::string& bookToOpen) :
	ZLApplication("FBReader"),
	QuitOnCancelOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "QuitOnCancel", false),
	StoreContentsPositionOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "StoreContentsPosition", false),
	LargeScrollingOptions(
		OPTIONS, DELAY, 250,
		LARGE_SCROLLING, MODE, TextView::NO_OVERLAPPING,
		LARGE_SCROLLING, LINES_TO_KEEP, 1,
		LARGE_SCROLLING, LINES_TO_SCROLL, 1,
		LARGE_SCROLLING, PERCENT_TO_SCROLL, 50
	),
	SmallScrollingOptions(
		SMALL_SCROLLING, DELAY, 50,
		SMALL_SCROLLING, MODE, TextView::SCROLL_LINES,
		SMALL_SCROLLING, LINES_TO_KEEP, 1,
		SMALL_SCROLLING, LINES_TO_SCROLL, 1,
		SMALL_SCROLLING, PERCENT_TO_SCROLL, 50
	),
	MouseScrollingOptions(
		MOUSE_SCROLLING, DELAY, 0,
		MOUSE_SCROLLING, MODE, TextView::SCROLL_LINES,
		MOUSE_SCROLLING, LINES_TO_KEEP, 1,
		MOUSE_SCROLLING, LINES_TO_SCROLL, 1,
		MOUSE_SCROLLING, PERCENT_TO_SCROLL, 50
	),
	FingerTapScrollingOptions(
		FINGER_TAP_SCROLLING, DELAY, 0,
		FINGER_TAP_SCROLLING, MODE, TextView::NO_OVERLAPPING,
		FINGER_TAP_SCROLLING, LINES_TO_KEEP, 1,
		FINGER_TAP_SCROLLING, LINES_TO_SCROLL, 1,
		FINGER_TAP_SCROLLING, PERCENT_TO_SCROLL, 50
	),
	EnableFingerScrollingOption(ZLOption::CONFIG_CATEGORY, FINGER_TAP_SCROLLING, "Enabled", true),
	SearchBackwardOption(FBOptions::SEARCH_CATEGORY, SEARCH, "Backward", false),
	SearchIgnoreCaseOption(FBOptions::SEARCH_CATEGORY, SEARCH, "IgnoreCase", true),
	SearchInWholeTextOption(FBOptions::SEARCH_CATEGORY, SEARCH, "WholeText", false),
	SearchThisSectionOnlyOption(FBOptions::SEARCH_CATEGORY, SEARCH, "ThisSectionOnly", false),
	SearchPatternOption(FBOptions::SEARCH_CATEGORY, SEARCH, "Pattern", ""),
	UseSeparateBindingsOption(ZLOption::CONFIG_CATEGORY, "KeysOptions", "UseSeparateBindings", false),
	myBindings0("Keys"),
	myBindings90("Keys90"),
	myBindings180("Keys180"),
	myBindings270("Keys270") {

	myModel = 0;
	myBookTextView = new BookTextView(*this, context());
	myFootnoteView = new FootnoteView(*this, context());
	myContentsView = new ContentsView(*this, context());
	myCollectionView = new CollectionView(*this, context());
	myRecentBooksView = new RecentBooksView(*this, context());
	myMode = UNDEFINED_MODE;
	myPreviousMode = BOOK_TEXT_MODE;

	BookDescriptionPtr description;

	if (!bookToOpen.empty()) {
		description = createDescription(bookToOpen);
	}

	if (description.isNull()) {
		std::string howToStartString = ApplicationDirectory() + PathDelimiter + "help" + PathDelimiter + "HowToStart.fb2";
		ZLStringOption bookName(ZLOption::STATE_CATEGORY, STATE, BOOK, howToStartString);
		description = BookDescription::getDescription(bookName.value());

		if (description.isNull()) {
			description = BookDescription::getDescription(howToStartString);
		}
	}
	openBook(description);

	addAction(ACTION_SHOW_COLLECTION, new ShowCollectionAction(*this));
	addAction(ACTION_SHOW_LAST_BOOKS, new ShowRecentBooksListAction(*this));
	addAction(ACTION_SHOW_OPTIONS, new ShowOptionsDialogAction(*this));
	addAction(ACTION_SHOW_CONTENTS, new ShowContentsAction(*this));
	addAction(ACTION_SHOW_BOOK_INFO, new ShowBookInfoAction(*this));
	addAction(ACTION_ADD_BOOK, new AddBookAction(*this));
	addAction(ACTION_UNDO, new UndoAction(*this));
	addAction(ACTION_REDO, new RedoAction(*this));
	addAction(ACTION_SEARCH, new SearchAction(*this));
	addAction(ACTION_FIND_NEXT, new FindNextAction(*this));
	addAction(ACTION_FIND_PREVIOUS, new FindPreviousAction(*this));
	addAction(ACTION_SCROLL_TO_HOME, new ScrollToHomeAction(*this));
	addAction(ACTION_SCROLL_TO_START_OF_TEXT, new ScrollToStartOfTextAction(*this));
	addAction(ACTION_SCROLL_TO_END_OF_TEXT, new ScrollToEndOfTextAction(*this));
	addAction(ACTION_LARGE_SCROLL_FORWARD, new ScrollingAction(*this, LargeScrollingOptions, true));
	addAction(ACTION_LARGE_SCROLL_BACKWARD, new ScrollingAction(*this, LargeScrollingOptions, false));
	addAction(ACTION_SMALL_SCROLL_FORWARD, new ScrollingAction(*this, SmallScrollingOptions, true));
	addAction(ACTION_SMALL_SCROLL_BACKWARD, new ScrollingAction(*this, SmallScrollingOptions, false));
	addAction(ACTION_MOUSE_SCROLL_FORWARD, new ScrollingAction(*this, MouseScrollingOptions, true));
	addAction(ACTION_MOUSE_SCROLL_BACKWARD, new ScrollingAction(*this, MouseScrollingOptions, false));
	addAction(ACTION_FINGER_TAP_SCROLL_FORWARD, new ScrollingAction(*this, FingerTapScrollingOptions, true));
	addAction(ACTION_FINGER_TAP_SCROLL_BACKWARD, new ScrollingAction(*this, FingerTapScrollingOptions, false));
	addAction(ACTION_INCREASE_FONT, new ChangeFontSizeAction(*this, 2));
	addAction(ACTION_DECREASE_FONT, new ChangeFontSizeAction(*this, -2));
	addAction(ACTION_ROTATE_SCREEN, new RotationAction(*this));
	addAction(ACTION_TOGGLE_FULLSCREEN, new FullscreenAction(*this, true));
	addAction(ACTION_FULLSCREEN_ON, new FullscreenAction(*this, false));
	addAction(ACTION_CANCEL, new CancelAction(*this));
	addAction(ACTION_SHOW_HIDE_POSITION_INDICATOR, new ToggleIndicatorAction(*this));
	addAction(ACTION_QUIT, new QuitAction(*this));
	addAction(ACTION_OPEN_PREVIOUS_BOOK, new OpenPreviousBookAction(*this));

	toolbar().addButton(ACTION_SHOW_COLLECTION, "books");
	toolbar().addButton(ACTION_SHOW_LAST_BOOKS, "history");
	toolbar().addButton(ACTION_ADD_BOOK, "addbook");
	toolbar().addSeparator();
	toolbar().addButton(ACTION_SHOW_BOOK_INFO, "bookinfo");
	toolbar().addButton(ACTION_SHOW_OPTIONS, "settings");
	toolbar().addSeparator();
	toolbar().addButton(ACTION_SCROLL_TO_HOME, "home");
	toolbar().addButton(ACTION_UNDO, "leftarrow");
	toolbar().addButton(ACTION_REDO, "rightarrow");
	toolbar().addSeparator();
	toolbar().addButton(ACTION_SHOW_CONTENTS, "contents");
	toolbar().addSeparator();
	toolbar().addButton(ACTION_SEARCH, "find");
	toolbar().addButton(ACTION_FIND_NEXT, "findnext");
	toolbar().addButton(ACTION_FIND_PREVIOUS, "findprev");
	toolbar().addSeparator();
	toolbar().addButton(ACTION_ROTATE_SCREEN, "rotatescreen");

	menubar().addItem("Book Info...",	ACTION_SHOW_BOOK_INFO);
	menubar().addItem("Table Of Contents", ACTION_SHOW_CONTENTS);

	Menu &librarySubmenu = menubar().addSubmenu("Library");
	librarySubmenu.addItem("Open", ACTION_SHOW_COLLECTION);
	librarySubmenu.addItem("Open Previous", ACTION_OPEN_PREVIOUS_BOOK);
	librarySubmenu.addItem("Recent", ACTION_SHOW_LAST_BOOKS);
	librarySubmenu.addItem("Add Book...", ACTION_ADD_BOOK);

	Menu &findSubmenu = menubar().addSubmenu("Find");
	findSubmenu.addItem("Find Text...", ACTION_SEARCH);
	findSubmenu.addItem("Find Next", ACTION_FIND_NEXT);
	findSubmenu.addItem("Find Previous", ACTION_FIND_PREVIOUS);

	Menu &viewSubmenu = menubar().addSubmenu("View");
	// MSS: these three actions can have a checkbox next to them
	viewSubmenu.addItem("Rotate Screen", ACTION_ROTATE_SCREEN);
	viewSubmenu.addItem("Full Screen", ACTION_TOGGLE_FULLSCREEN);
	viewSubmenu.addItem("Toggle Indicator", ACTION_SHOW_HIDE_POSITION_INDICATOR);

	menubar().addItem("Preferences...", ACTION_SHOW_OPTIONS);
	menubar().addItem("Close", ACTION_QUIT);
}

void FBReader::initWindow() {
	ZLApplication::initWindow();
	setMode(BOOK_TEXT_MODE);
	refreshWindow();
}

FBReader::~FBReader() {
	delete myBookTextView;
	delete myFootnoteView;
	delete myContentsView;
	delete myCollectionView;
	delete myRecentBooksView;
	if (myModel != 0) {
		delete myModel;
	}
	TextStyleCollection::deleteInstance();
	PluginCollection::deleteInstance();
	Hyphenator::deleteInstance();
}

BookDescriptionPtr FBReader::createDescription(const std::string& fileName) const {
	ZLFile bookFile = ZLFile(fileName);

	if (!bookFile.isArchive()) {
		return BookDescription::getDescription(fileName);
	}

	std::queue<std::string> archiveNames;
	archiveNames.push(fileName);

	std::vector<std::string> items;

	while (!archiveNames.empty()) {
		shared_ptr<ZLDir> archiveDir = ZLFile(archiveNames.front()).directory();
		archiveNames.pop();
		if (archiveDir.isNull()) {
			continue;
		}
		archiveDir->collectFiles(items, true);
		for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
			const std::string fileName = archiveDir->itemName(*it);
			ZLFile subFile(fileName);
			if (subFile.isArchive()) {
				archiveNames.push(fileName);
			} else if (!subFile.isDirectory()) {
				BookDescriptionPtr description = BookDescription::getDescription(fileName);
				if (!description.isNull()) {
					return description;
				}
			}
		}
		items.clear();
	}

	return 0;
}

class OpenBookRunnable : public ZLRunnable {

public:
	OpenBookRunnable(FBReader &reader, BookDescriptionPtr description) : myReader(reader), myDescription(description) {}
	void run() { myReader.openBookInternal(myDescription); }

private:
	FBReader &myReader;
	BookDescriptionPtr myDescription;
};

void FBReader::openBook(BookDescriptionPtr description) {
	OpenBookRunnable runnable(*this, description);
	ZLDialogManager::instance().wait(runnable, "Loading book. Please, wait...");
}

void FBReader::openBookInternal(BookDescriptionPtr description) {
	if (!description.isNull()) {
		myBookTextView->saveState();
		myContentsView->saveState();
		if (myModel != 0) {
			delete myModel;
		}
		myModel = new BookModel(description);
		ZLStringOption(ZLOption::STATE_CATEGORY, STATE, BOOK, std::string()).setValue(myModel->fileName());
		Hyphenator::instance().load(description->language());
		myBookTextView->setModel(&myModel->bookTextModel(), description->fileName());
		myBookTextView->setCaption(description->title());
		myFootnoteView->setModel(0, std::string());
		myFootnoteView->setCaption(description->title());
		myContentsView->setModel(&myModel->contentsModel(), description->fileName());
		myContentsView->setCaption(description->title());

		myRecentBooksView->lastBooks().addBook(description->fileName());
	}
}

void FBReader::tryShowFootnoteView(const std::string &id) {
	if ((myMode == BOOK_TEXT_MODE) && (myModel != 0)) {
		int linkedParagraphNumber = myModel->paragraphNumberById(id);
		if (linkedParagraphNumber >= 0) {
			myBookTextView->gotoParagraph(linkedParagraphNumber);
			refreshWindow();
		} else {
			const TextModel *footnoteModel = myModel->footnoteModel(id);
			if (footnoteModel != 0) {
				myFootnoteView->setModel(footnoteModel, std::string());
				setMode(FOOTNOTE_MODE);
			}
		}
	}
}

void FBReader::bookInfoSlot() {
	if ((myMode == BOOK_TEXT_MODE) || (myMode == CONTENTS_MODE) || (myMode == FOOTNOTE_MODE)) {
		runBookInfoDialog(myModel->fileName());
	}
}

void FBReader::optionsSlot() {
	OptionsDialog optionsDialog(*this, context());
	optionsDialog.dialog().run("");
	grabAllKeys(KeyboardControlOption.value());
	clearTextCaches();
	refreshWindow();
}

void FBReader::addBookSlot() {
	FBFileHandler handler;
	if (ZLDialogManager::instance().selectionDialog("FBReader - Add File To Library", handler)) {
		BookDescriptionPtr description = handler.description();
		if (!description.isNull() && runBookInfoDialog(description->fileName())) {
			BookList().addFileName(description->fileName());
			setMode(BOOK_TEXT_MODE);
		}
	}
}

class RebuildCollectionRunnable : public ZLRunnable {

public:
	RebuildCollectionRunnable(FBReader &reader) : myReader(reader) {}
	void run() { myReader.rebuildCollectionInternal(); }

private:
	FBReader &myReader;
};

void FBReader::rebuildCollectionInternal() {
	myCollectionView->updateModel();
	myCollectionView->collection().authors();
}

void FBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}

	myPreviousMode = myMode;
	myMode = mode;

	switch (myMode) {
		case BOOK_TEXT_MODE:
			setView(myBookTextView);
			break;
		case CONTENTS_MODE:
			if (!StoreContentsPositionOption.value()) {
				myContentsView->gotoReference();
			}
			setView(myContentsView);
			break;
		case FOOTNOTE_MODE:
			setView(myFootnoteView);
			break;
		case BOOK_COLLECTION_MODE:
			{
				RebuildCollectionRunnable runnable(*this);
				ZLDialogManager::instance().wait(runnable, "Loading book list. Please, wait...");
			}
			if (myModel != 0) {
				myCollectionView->selectBook(myModel->description());
			}
			setView(myCollectionView);
			break;
		case RECENT_BOOKS_MODE:
			myRecentBooksView->rebuild();
			setView(myRecentBooksView);
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
}

bool FBReader::runBookInfoDialog(const std::string &fileName) {
	BookCollection &collection = myCollectionView->collection();
	if (BookInfoDialog(collection, fileName).dialog().run("")) {
		openFile(fileName);
		collection.rebuild(false);
		return true;
	}
	return false;
}

BookTextView &FBReader::bookTextView() const {
	return *myBookTextView;
}

void FBReader::showBookTextView() {
	setMode(BOOK_TEXT_MODE);
}

void FBReader::restorePreviousMode() {
	setMode(myPreviousMode);
	myPreviousMode = BOOK_TEXT_MODE;
}

bool FBReader::closeView() {
	if (myMode == BOOK_TEXT_MODE) {
		quit();
		return true;
	} else {
		restorePreviousMode();
		return false;
	}
}

void FBReader::openFile(const std::string &fileName) {
	BookDescriptionPtr description = BookDescription::getDescription(fileName);
	if (!description.isNull()) {
		openBook(description);
		resetWindowCaption();
		refreshWindow();
	}
}

void FBReader::clearTextCaches() {
	myBookTextView->clearCaches();
	myFootnoteView->clearCaches();
	myContentsView->clearCaches();
	myCollectionView->clearCaches();
	myRecentBooksView->clearCaches();
}

void FBReader::searchSlot() {
	shared_ptr<ZLDialog> searchDialog = ZLDialogManager::instance().createDialog("Text search");

	searchDialog->addOption(new ZLSimpleStringOptionEntry("", SearchPatternOption));
	searchDialog->addOption(new ZLSimpleBooleanOptionEntry("&Ignore case", SearchIgnoreCaseOption));
	searchDialog->addOption(new ZLSimpleBooleanOptionEntry("In w&hole text", SearchInWholeTextOption));
	searchDialog->addOption(new ZLSimpleBooleanOptionEntry("&Backward", SearchBackwardOption));
	if (((TextView*)currentView())->hasMultiSectionModel()) {
		searchDialog->addOption(new ZLSimpleBooleanOptionEntry("&This section only", SearchThisSectionOnlyOption));
	}
	searchDialog->addButton("&Go!", true);

	if (searchDialog->run()) {
		searchDialog->acceptValues();
		std::string pattern = SearchPatternOption.value();
		while (!pattern.empty() && isspace(pattern[0])) {
			pattern.erase(0, 1);
		}
		while (!pattern.empty() && isspace(pattern[pattern.length() - 1])) {
			pattern.erase(pattern.length() - 1, 1);
		}
		SearchPatternOption.setValue(pattern);
		((TextView*)currentView())->search(
			SearchPatternOption.value(),
			SearchIgnoreCaseOption.value(),
			SearchInWholeTextOption.value(),
			SearchBackwardOption.value(),
			SearchThisSectionOnlyOption.value()
		);
	}
}

ZLKeyBindings &FBReader::keyBindings() {
	return UseSeparateBindingsOption.value() ?
		keyBindings(myViewWidget->rotation()) : myBindings0;
}

ZLKeyBindings &FBReader::keyBindings(ZLViewWidget::Angle angle) {
	switch (angle) {
		case ZLViewWidget::DEGREES0:
		default:
			return myBindings0;
		case ZLViewWidget::DEGREES90:
			return myBindings90;
		case ZLViewWidget::DEGREES180:
			return myBindings180;
		case ZLViewWidget::DEGREES270:
			return myBindings270;
	}
}
