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

#include <queue>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLDir.h>
#include <ZLStringUtil.h>
#include <ZLResource.h>
#include <ZLMessage.h>

#include <ZLTextHyphenator.h>

#include "FBReader.h"
#include "FBReaderActions.h"
#include "BookTextView.h"
#include "FootnoteView.h"
#include "ContentsView.h"
#include "CollectionView.h"
#include "RecentBooksView.h"
#include "BookInfoDialog.h"
#include "TimeUpdater.h"

#include "../migration/migrate.h"

#include "../options/FBOptions.h"
#include "../bookmodel/BookModel.h"
#include "../formats/FormatPlugin.h"
#include "../collection/BookList.h"

static const std::string OPTIONS = "Options";
static const std::string STATE = "State";
static const std::string BOOK = "Book";

const std::string LARGE_SCROLLING = "LargeScrolling";
const std::string SMALL_SCROLLING = "SmallScrolling";
const std::string MOUSE_SCROLLING = "MouseScrolling";
const std::string TAP_SCROLLING = "TapScrolling";

const std::string DELAY = "ScrollingDelay";
const std::string MODE = "Mode";
const std::string LINES_TO_KEEP = "LinesToKeep";
const std::string LINES_TO_SCROLL = "LinesToScroll";
const std::string PERCENT_TO_SCROLL = "PercentToScroll";

FBReader::ScrollingOptions::ScrollingOptions(const std::string &groupName, long delayValue, long modeValue, long linesToKeepValue, long linesToScrollValue, long percentToScrollValue) :
	DelayOption(ZLCategoryKey::CONFIG, groupName, DELAY, 0, 5000, delayValue),
	ModeOption(ZLCategoryKey::CONFIG, groupName, MODE, modeValue),
	LinesToKeepOption(ZLCategoryKey::CONFIG, groupName, LINES_TO_KEEP, 1, 100, linesToKeepValue),
	LinesToScrollOption(ZLCategoryKey::CONFIG, groupName, LINES_TO_SCROLL, 1, 100, linesToScrollValue),
	PercentToScrollOption(ZLCategoryKey::CONFIG, groupName, PERCENT_TO_SCROLL, 1, 100, percentToScrollValue) {
}

class OpenFileHandler : public ZLMessageHandler {

public:
	OpenFileHandler(FBReader &fbreader) : myFBReader(fbreader) {}
	void onMessageReceived(const std::vector<std::string> &arguments) {
		if (arguments.size() == 1) {
			myFBReader.myBookAlreadyOpen = true;
			myFBReader.presentWindow();
			myFBReader.openFile(arguments[0]);
		}
	}

private:
	FBReader &myFBReader;
};

FBReader::FBReader(const std::string &bookToOpen) :
	ZLApplication("FBReader"),
	QuitOnCancelOption(ZLCategoryKey::CONFIG, OPTIONS, "QuitOnCancel", false),
	LargeScrollingOptions(LARGE_SCROLLING, 250, ZLTextView::NO_OVERLAPPING, 1, 1, 50),
	SmallScrollingOptions(SMALL_SCROLLING, 50, ZLTextView::SCROLL_LINES, 1, 1, 50),
	MouseScrollingOptions(MOUSE_SCROLLING, 0, ZLTextView::SCROLL_LINES, 1, 1, 50),
	TapScrollingOptions(TAP_SCROLLING, 0, ZLTextView::NO_OVERLAPPING, 1, 1, 50),
	EnableTapScrollingOption(ZLCategoryKey::CONFIG, TAP_SCROLLING, "Enabled", true),
	TapScrollingOnFingerOnlyOption(ZLCategoryKey::CONFIG, TAP_SCROLLING, "FingerOnly", true),
	UseSeparateBindingsOption(ZLCategoryKey::CONFIG, "KeysOptions", "UseSeparateBindings", false),
	EnableSingleClickDictionaryOption(ZLCategoryKey::CONFIG, "Dictionary", "SingleClick", false),
	myBindings0("Keys"),
	myBindings90("Keys90"),
	myBindings180("Keys180"),
	myBindings270("Keys270"),
	myBookToOpen(bookToOpen),
	myBookAlreadyOpen(false) {

	myModel = 0;
	myBookTextView = new BookTextView(*this, context());
	myFootnoteView = new FootnoteView(*this, context());
	myContentsView = new ContentsView(*this, context());
	myCollectionView = new CollectionView(*this, context());
	myRecentBooksView = new RecentBooksView(*this, context());
	myMode = UNDEFINED_MODE;
	myPreviousMode = BOOK_TEXT_MODE;
	setMode(BOOK_TEXT_MODE);

	addAction(ActionCode::SHOW_COLLECTION, new SetModeAction(*this, FBReader::BOOK_COLLECTION_MODE, FBReader::BOOK_TEXT_MODE | FBReader::CONTENTS_MODE | FBReader::RECENT_BOOKS_MODE));
	addAction(ActionCode::SHOW_LAST_BOOKS, new SetModeAction(*this, FBReader::RECENT_BOOKS_MODE, FBReader::BOOK_TEXT_MODE | FBReader::CONTENTS_MODE));
	addAction(ActionCode::SHOW_OPTIONS, new ShowOptionsDialogAction(*this));
	addAction(ActionCode::SHOW_CONTENTS, new ShowContentsAction(*this));
	addAction(ActionCode::SHOW_BOOK_INFO, new ShowBookInfoAction(*this));
	addAction(ActionCode::ADD_BOOK, new AddBookAction(*this));
	addAction(ActionCode::UNDO, new UndoAction(*this));
	addAction(ActionCode::REDO, new RedoAction(*this));
	addAction(ActionCode::SEARCH, new SearchAction(*this));
	addAction(ActionCode::FIND_NEXT, new FindNextAction(*this));
	addAction(ActionCode::FIND_PREVIOUS, new FindPreviousAction(*this));
	addAction(ActionCode::SCROLL_TO_HOME, new ScrollToHomeAction(*this));
	addAction(ActionCode::SCROLL_TO_START_OF_TEXT, new ScrollToStartOfTextAction(*this));
	addAction(ActionCode::SCROLL_TO_END_OF_TEXT, new ScrollToEndOfTextAction(*this));
	addAction(ActionCode::LARGE_SCROLL_FORWARD, new ScrollingAction(*this, LargeScrollingOptions, true));
	addAction(ActionCode::LARGE_SCROLL_BACKWARD, new ScrollingAction(*this, LargeScrollingOptions, false));
	addAction(ActionCode::SMALL_SCROLL_FORWARD, new ScrollingAction(*this, SmallScrollingOptions, true));
	addAction(ActionCode::SMALL_SCROLL_BACKWARD, new ScrollingAction(*this, SmallScrollingOptions, false));
	addAction(ActionCode::MOUSE_SCROLL_FORWARD, new ScrollingAction(*this, MouseScrollingOptions, true));
	addAction(ActionCode::MOUSE_SCROLL_BACKWARD, new ScrollingAction(*this, MouseScrollingOptions, false));
	addAction(ActionCode::TAP_SCROLL_FORWARD, new ScrollingAction(*this, TapScrollingOptions, true));
	addAction(ActionCode::TAP_SCROLL_BACKWARD, new ScrollingAction(*this, TapScrollingOptions, false));
	addAction(ActionCode::INCREASE_FONT, new ChangeFontSizeAction(*this, 2));
	addAction(ActionCode::DECREASE_FONT, new ChangeFontSizeAction(*this, -2));
	addAction(ActionCode::ROTATE_SCREEN, new RotationAction(*this));
	addAction(ActionCode::TOGGLE_FULLSCREEN, new FullscreenAction(*this, true));
	addAction(ActionCode::FULLSCREEN_ON, new FullscreenAction(*this, false));
	addAction(ActionCode::CANCEL, new CancelAction(*this));
	addAction(ActionCode::SHOW_HIDE_POSITION_INDICATOR, new ToggleIndicatorAction(*this));
	addAction(ActionCode::QUIT, new QuitAction(*this));
	addAction(ActionCode::OPEN_PREVIOUS_BOOK, new OpenPreviousBookAction(*this));
	addAction(ActionCode::SHOW_HELP, new ShowHelpAction(*this));
	addAction(ActionCode::GOTO_NEXT_TOC_SECTION, new GotoNextTOCSectionAction(*this));
	addAction(ActionCode::GOTO_PREVIOUS_TOC_SECTION, new GotoPreviousTOCSectionAction(*this));
	addAction(ActionCode::COPY_SELECTED_TEXT_TO_CLIPBOARD, new CopySelectedTextAction(*this));
	addAction(ActionCode::OPEN_SELECTED_TEXT_IN_DICTIONARY, new OpenSelectedTextInDictionaryAction(*this));
	addAction(ActionCode::CLEAR_SELECTION, new ClearSelectionAction(*this));
	addAction(ActionCode::GOTO_PAGE_NUMBER, new GotoPageNumber(*this));

	myOpenFileHandler = new OpenFileHandler(*this);
	ZLCommunicationManager::instance().registerHandler("openFile", myOpenFileHandler);
}

FBReader::~FBReader() {
	if (myModel != 0) {
		delete myModel;
	}
	ZLTextStyleCollection::deleteInstance();
	PluginCollection::deleteInstance();
	ZLTextHyphenator::deleteInstance();
}

void FBReader::initWindow() {
	ZLApplication::initWindow();
	trackStylus(true);

	MigrationRunnable migration;
	if (migration.shouldMigrate()) {
		ZLDialogManager::instance().wait(ZLResourceKey("migrate"), migration);
	}

	if (!myBookAlreadyOpen) {
		BookDescriptionPtr description;
		if (!myBookToOpen.empty()) {
			createDescription(myBookToOpen, description);
		}
		if (description.isNull()) {
			ZLStringOption bookName(ZLCategoryKey::STATE, STATE, BOOK, "");
			description = BookDescription::getDescription(bookName.value());
		}
		if (description.isNull()) {
			description = BookDescription::getDescription(helpFileName(ZLibrary::Language()));
		}
		if (description.isNull()) {
			description = BookDescription::getDescription(helpFileName("en"));
		}
		openBook(description);
	}
	refreshWindow();

	ZLTimeManager::instance().addTask(new TimeUpdater(*this), 1000);
}

bool FBReader::createDescription(const std::string& fileName, BookDescriptionPtr &description) {
	ZLFile bookFile = ZLFile(fileName);

	if (!bookFile.isArchive()) {
		FormatPlugin *plugin = PluginCollection::instance().plugin(ZLFile(fileName), false);
		if (plugin == 0) {
			return false;
		}
		std::string error = plugin->tryOpen(fileName);
		if (!error.empty()) {
			ZLResourceKey boxKey("openBookErrorBox");
			ZLDialogManager::instance().errorBox(
				boxKey,
				ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), error)
			);
		} else {
			BookList().addFileName(bookFile.path());
			description = BookDescription::getDescription(bookFile.path());
		}
		return true;
	}

	std::queue<std::string> archiveNames;
	archiveNames.push(bookFile.path());

	std::vector<std::string> items;

	while (!archiveNames.empty()) {
		shared_ptr<ZLDir> archiveDir = ZLFile(archiveNames.front()).directory();
		archiveNames.pop();
		if (archiveDir.isNull()) {
			continue;
		}
		archiveDir->collectFiles(items, true);
		for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
			const std::string itemName = archiveDir->itemPath(*it);
			ZLFile subFile(itemName);
			if (subFile.isArchive()) {
				archiveNames.push(itemName);
			} else if (createDescription(itemName, description)) {
				return true;
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
	ZLDialogManager::instance().wait(ZLResourceKey("loadingBook"), runnable);
	resetWindowCaption();
}

void FBReader::openBookInternal(BookDescriptionPtr description) {
	if (!description.isNull()) {
		BookTextView &bookTextView = (BookTextView&)*myBookTextView;
		ContentsView &contentsView = (ContentsView&)*myContentsView;
		FootnoteView &footnoteView = (FootnoteView&)*myFootnoteView;
		RecentBooksView &recentBooksView = (RecentBooksView&)*myRecentBooksView;

		bookTextView.saveState();
		bookTextView.setModel(0, "");
		bookTextView.setContentsModel(0);
		contentsView.setModel(0);
		if (myModel != 0) {
			delete myModel;
		}
		myModel = new BookModel(description);
		ZLStringOption(ZLCategoryKey::STATE, STATE, BOOK, std::string()).setValue(myModel->fileName());
		ZLTextHyphenator::instance().load(description->language());
		bookTextView.setModel(myModel->bookTextModel(), description->fileName());
		bookTextView.setCaption(description->title());
		bookTextView.setContentsModel(myModel->contentsModel());
		footnoteView.setModel(0);
		footnoteView.setCaption(description->title());
		contentsView.setModel(myModel->contentsModel());
		contentsView.setCaption(description->title());

		recentBooksView.lastBooks().addBook(description->fileName());
	}
}

void FBReader::tryShowFootnoteView(const std::string &id, bool external) {
	if (external) {
		shared_ptr<ProgramCollection> collection = webBrowserCollection();
		if (!collection.isNull()) {
			shared_ptr<Program> program = collection->currentProgram();
			if (!program.isNull()) {
				program->run("openLink", id);
			}
		}
	} else {
		if ((myMode == BOOK_TEXT_MODE) && (myModel != 0)) {
			BookModel::Label label = myModel->label(id);
			if (!label.Model.isNull()) {
				if (label.Model == myModel->bookTextModel()) {
					bookTextView().gotoParagraph(label.ParagraphNumber);
				} else {
					FootnoteView &view = ((FootnoteView&)*myFootnoteView);
					view.setModel(label.Model);
					setMode(FOOTNOTE_MODE);
					view.gotoParagraph(label.ParagraphNumber);
				}
				setHyperlinkCursor(false);
				refreshWindow();
			}
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
	CollectionView &collectionView = (CollectionView&)*myCollectionView;
	collectionView.updateModel();
	collectionView.collection().authors();
}

FBReader::ViewMode FBReader::getMode() const {
	return myMode;
}

void FBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}

	myPreviousMode = myMode;
	myMode = mode;

	setViewFinal(myMode == BOOK_TEXT_MODE);

	switch (myMode) {
		case BOOK_TEXT_MODE:
			setView(myBookTextView);
			break;
		case CONTENTS_MODE:
			((ContentsView&)*myContentsView).gotoReference();
			setView(myContentsView);
			break;
		case FOOTNOTE_MODE:
			setView(myFootnoteView);
			break;
		case BOOK_COLLECTION_MODE:
			{
				RebuildCollectionRunnable runnable(*this);
				ZLDialogManager::instance().wait(ZLResourceKey("loadingBookList"), runnable);
			}
			if (myModel != 0) {
				((CollectionView&)*myCollectionView).selectBook(myModel->description());
			}
			setView(myCollectionView);
			break;
		case RECENT_BOOKS_MODE:
			((RecentBooksView&)*myRecentBooksView).rebuild();
			setView(myRecentBooksView);
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
}

bool FBReader::runBookInfoDialog(const std::string &fileName) {
	BookCollection &collection = ((CollectionView&)*myCollectionView).collection();
	if (BookInfoDialog(collection, fileName).dialog().run()) {
		collection.rebuild(false);
		return true;
	}
	return false;
}

BookTextView &FBReader::bookTextView() const {
	return (BookTextView&)*myBookTextView;
}

CollectionView &FBReader::collectionView() const {
	return (CollectionView&)*myCollectionView;
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

std::string FBReader::helpFileName(const std::string &language) const {
	return ZLibrary::ApplicationDirectory() + ZLibrary::FileNameDelimiter + "help" + ZLibrary::FileNameDelimiter + "MiniHelp." + language + ".fb2";
}

void FBReader::openFile(const std::string &fileName) {
	BookDescriptionPtr description;
	createDescription(fileName, description);
	if (!description.isNull()) {
		openBook(description);
		refreshWindow();
	}
}

void FBReader::clearTextCaches() {
	((ZLTextView&)*myBookTextView).clearCaches();
	((ZLTextView&)*myFootnoteView).clearCaches();
	((ZLTextView&)*myContentsView).clearCaches();
	((ZLTextView&)*myCollectionView).clearCaches();
	((ZLTextView&)*myRecentBooksView).clearCaches();
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

shared_ptr<ProgramCollection> FBReader::dictionaryCollection() const {
	return myProgramCollectionMap.collection("Dictionary");
}

bool FBReader::isDictionarySupported() const {
	shared_ptr<ProgramCollection> collection = dictionaryCollection();
	return !collection.isNull() && !collection->currentProgram().isNull();
}

void FBReader::openInDictionary(const std::string &word) {
	shared_ptr<Program> dictionary = dictionaryCollection()->currentProgram();
	dictionary->run("present", ZLibrary::ApplicationName());
	dictionary->run("showWord", word);
}

shared_ptr<ProgramCollection> FBReader::webBrowserCollection() const {
	return myProgramCollectionMap.collection("Web Browser");
}
