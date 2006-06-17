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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLDialogManager.h>
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLPaintContext.h>
#include <abstract/ZLDir.h>

#include "FBReader.h"
#include "BookTextView.h"
#include "FootnoteView.h"
#include "ContentsView.h"
#include "CollectionView.h"
#include "RecentBooksView.h"
#include "OptionsDialog.h"
#include "InfoDialog.h"
#include "FBFileHandler.h"

#include "../Files.h"
#include "../FBOptions.h"
#include "../collection/BookCollection.h"
#include "../collection/BookList.h"
#include "../bookmodel/BookModel.h"
#include "../hyphenation/Hyphenator.h"
#include "../textview/TextStyle.h"
#include "../textview/ParagraphCursor.h"
#include "../formats/FormatPlugin.h"

static const std::string OPTIONS = "Options";
static const std::string SEARCH = "Search";
static const std::string STATE = "State";
static const std::string BOOK = "Book";
static const std::string ANGLE = "Angle";

const std::string LARGE_SCROLLING = "LargeScrolling";
const std::string SMALL_SCROLLING = "SmallScrolling";
const std::string MOUSE_SCROLLING = "MouseScrolling";

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

FBReader::FBReader(ZLPaintContext *context, const std::string& bookToOpen, bool supportRotation) :
  QuitOnCancelOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "QuitOnCancel", false),
  StoreContentsPositionOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "StoreContentsPosition", false),
  KeyDelayOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "KeyDelay", 0, 5000, 750),
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
  SearchBackwardOption(FBOptions::SEARCH_CATEGORY, SEARCH, "Backward", false),
  SearchIgnoreCaseOption(FBOptions::SEARCH_CATEGORY, SEARCH, "IgnoreCase", true),
  SearchInWholeTextOption(FBOptions::SEARCH_CATEGORY, SEARCH, "WholeText", false),
  SearchThisSectionOnlyOption(FBOptions::SEARCH_CATEGORY, SEARCH, "ThisSectionOnly", false),
  SearchPatternOption(FBOptions::SEARCH_CATEGORY, SEARCH, "Pattern", ""),
  KeyboardControlOption(ZLOption::CONFIG_CATEGORY, "Keyboard", "FullControl", false),
  RotationAngleOption(ZLOption::CONFIG_CATEGORY, "Rotation", ANGLE, ZLViewWidget::DEGREES90),
  AngleStateOption(ZLOption::CONFIG_CATEGORY, STATE, ANGLE, ZLViewWidget::DEGREES0),
  myIsRotationSupported(supportRotation) {

  myModel = 0;
  myContext = context;
  myBookTextView = new BookTextView(*this, *myContext);
  myFootnoteView = new FootnoteView(*myContext);
  myContentsView = new ContentsView(*this, *myContext);
  myCollectionView = new CollectionView(*this, *myContext);
  myRecentBooksView = new RecentBooksView(*this, *myContext);
  myMode = UNDEFINED_MODE;
  myPreviousMode = BOOK_TEXT_MODE;

  BookDescriptionPtr description;

  if (!bookToOpen.empty()) {
    description = createDescription(bookToOpen);
  }

  if (description.isNull()) {
    std::string howToStartString = Files::PathPrefix + "help" + Files::PathDelimiter + "HowToStart.fb2";
    ZLStringOption bookName(ZLOption::STATE_CATEGORY, STATE, BOOK, howToStartString);
    description = BookDescription::create(bookName.value());

    if (description.isNull()) {
      description = BookDescription::create(howToStartString);
    }
  }
  openBook(description);

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
  if (isRotationSupported()) {
    toolbar().addButton(ACTION_ROTATE_SCREEN, "rotatescreen");
  }
}

FBReader::~FBReader() {
  delete myContext;
  delete myBookTextView;
  delete myFootnoteView;
  delete myContentsView;
  delete myCollectionView;
  delete myRecentBooksView;
  if (myModel != 0) {
    ZLStringOption bookName(ZLOption::STATE_CATEGORY, STATE, BOOK, std::string());
    bookName.setValue(myModel->fileName());
    delete myModel;
  }
  TextStyleCollection::deleteInstance();
  PluginCollection::deleteInstance();
  Hyphenator::deleteInstance();
}

BookDescriptionPtr FBReader::createDescription(const std::string& fileName) const {
  BookDescriptionPtr description;
  ZLFile aBook = ZLFile(fileName);

  if (aBook.isArchive()) {
    shared_ptr<ZLDir> myDir = aBook.directory();
    std::vector<std::string> names, archives;

    myDir->collectFiles(names, true);

    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
      const std::string& candidateName = myDir->itemName(*it);
      ZLFile candidate = ZLFile(candidateName);

      if (candidate.isArchive()) {
        archives.push_back(candidateName);
      } else if (!candidate.isDirectory()) {
        description = BookDescription::create(candidateName);

        if (!description.isNull()) {
          break;
        }
      }
    }

    if (description.isNull()) {
      for (std::vector<std::string>::const_iterator it = archives.begin(); it != archives.end(); it++) {
        description = createDescription(*it);

        if (!description.isNull()) {
          break;
        }
      }
    }
  } else if (!aBook.isDirectory()) {
    description = BookDescription::create(fileName);
  }

  return description;
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
      repaintView();
    } else {
      const TextModel *footnoteModel = myModel->footnoteModel(id);
      if (footnoteModel != 0) {
        myFootnoteView->setModel(footnoteModel, std::string());
        setMode(FOOTNOTE_MODE);
      }
    }
  }
}

void FBReader::repaintView() {
  if (myViewWidget != 0) {
    myViewWidget->repaintView();
  }
}

void FBReader::bookInfoSlot() {
  if ((myMode == BOOK_TEXT_MODE) || (myMode == CONTENTS_MODE) || (myMode == FOOTNOTE_MODE)) {
    runBookInfoDialog(myModel->fileName());
  }
}

void FBReader::optionsSlot() {
  OptionsDialog optionsDialog(*this, *myContext);
  optionsDialog.dialog().run("");
  grabAllKeys(KeyboardControlOption.value());
  clearTextCaches();
  repaintView();
}

void FBReader::addBookSlot() {
  FBFileHandler handler;
  ZLDialogManager::instance().openFileDialog("FBReader -- Add File To Library", handler);
  BookDescriptionPtr description = handler.description();
  if (!description.isNull() && runBookInfoDialog(description->fileName())) {
    BookList().addFileName(description->fileName());
    setMode(BOOK_TEXT_MODE);
  }
}

void FBReader::doScrolling(const ScrollingOptions &options, bool forward) {
  int delay = myLastScrollingTime.millisecondsTo(ZLTime());
  if ((delay < 0) || (delay >= options.DelayOption.value())) {
    TextView::ScrollingMode oType = (TextView::ScrollingMode)options.ModeOption.value();
    unsigned int oValue = 0;
    switch (oType) {
      case TextView::KEEP_LINES:
        oValue = options.LinesToKeepOption.value();
        break;
      case TextView::SCROLL_LINES:
        oValue = options.LinesToScrollOption.value();
        break;
      case TextView::SCROLL_PERCENTAGE:
        oValue = options.PercentToScrollOption.value();
        break;
      default:
        break;
    }
    ((TextView*)myViewWidget->view())->scrollPage(forward, oType, oValue);
    repaintView();
    myLastScrollingTime = ZLTime();
  }
}

bool FBReader::isScrollingAction(ActionCode code) {
  switch (code) {
    case ACTION_LARGE_SCROLL_FORWARD:
    case ACTION_LARGE_SCROLL_BACKWARD:
    case ACTION_SMALL_SCROLL_FORWARD:
    case ACTION_SMALL_SCROLL_BACKWARD:
    case ACTION_MOUSE_SCROLL_FORWARD:
    case ACTION_MOUSE_SCROLL_BACKWARD:
      return true;
    default:
      return false;
  }
}

void FBReader::doAction(ActionCode code) {
  switch (code) {
    case NO_ACTION:
      break;
    case ACTION_SHOW_COLLECTION:
      setMode(BOOK_COLLECTION_MODE);
      break;
    case ACTION_SHOW_LAST_BOOKS:
      setMode(RECENT_BOOKS_MODE);
      break;
    case ACTION_SHOW_OPTIONS:
      optionsSlot();
      break;
    case ACTION_UNDO:
      if (myMode == BOOK_TEXT_MODE) {
        myBookTextView->undoPageMove();
      } else {
        restorePreviousMode();
      }
      break;
    case ACTION_REDO:
      if (myMode == BOOK_TEXT_MODE) {
        myBookTextView->redoPageMove();
      }
      break;
    case ACTION_SHOW_CONTENTS:
      if (((myMode == BOOK_TEXT_MODE) || (myMode == FOOTNOTE_MODE)) && !myContentsView->isEmpty()) {
        setMode(CONTENTS_MODE);
      }
      break;
    case ACTION_SEARCH:
      searchSlot();
      break;
    case ACTION_FIND_PREVIOUS:
      ((TextView*)myViewWidget->view())->findPrevious();
      break;
    case ACTION_FIND_NEXT:
      ((TextView*)myViewWidget->view())->findNext();
      break;
    case ACTION_LARGE_SCROLL_FORWARD:
    case ACTION_LARGE_SCROLL_BACKWARD:
      doScrolling(LargeScrollingOptions, code == ACTION_LARGE_SCROLL_FORWARD);
      break;
    case ACTION_SMALL_SCROLL_FORWARD:
    case ACTION_SMALL_SCROLL_BACKWARD:
      doScrolling(SmallScrollingOptions, code == ACTION_SMALL_SCROLL_FORWARD);
      break;
    case ACTION_MOUSE_SCROLL_FORWARD:
    case ACTION_MOUSE_SCROLL_BACKWARD:
      doScrolling(MouseScrollingOptions, code == ACTION_MOUSE_SCROLL_FORWARD);
      break;
    case ACTION_SCROLL_TO_HOME:
      if (myMode == BOOK_TEXT_MODE) {
        myBookTextView->scrollToHome();
      }
      break;
    case ACTION_SCROLL_TO_START_OF_TEXT:
      ((TextView*)myViewWidget->view())->scrollToStartOfText();
      break;
    case ACTION_SCROLL_TO_END_OF_TEXT:
      ((TextView*)myViewWidget->view())->scrollToEndOfText();
      break;
    case ACTION_CANCEL:
      if (myMode != BOOK_TEXT_MODE) {
        restorePreviousMode();
      } else if (isFullscreen()) {
        toggleFullscreenSlot();
      } else if (QuitOnCancelOption.value()) {
        quitSlot();
      }
      break;
    case ACTION_INCREASE_FONT:
      {
        ZLIntegerRangeOption &option =
          TextStyleCollection::instance().baseStyle().FontSizeOption;
        option.setValue(option.value() + 2);
        clearTextCaches();
        repaintView();
      }
      break;
    case ACTION_DECREASE_FONT:
      {
        ZLIntegerRangeOption &option =
          TextStyleCollection::instance().baseStyle().FontSizeOption;
        option.setValue(option.value() - 2);
        clearTextCaches();
        repaintView();
      }
      break;
    case ACTION_SHOW_HIDE_POSITION_INDICATOR:
			{
			  ZLBooleanOption &option = TextStyleCollection::instance().indicatorStyle().ShowOption;
        option.setValue(!option.value());
        repaintView();
		  }
      break;
    case ACTION_TOGGLE_FULLSCREEN:
      toggleFullscreenSlot();
      break;
    case ACTION_FULLSCREEN_ON:
      if (!isFullscreen()) {
        toggleFullscreenSlot();
      }
      break;
    case ACTION_ADD_BOOK:
      addBookSlot();
      break;
    case ACTION_SHOW_BOOK_INFO:
      bookInfoSlot();
      break;
    case ACTION_SHOW_HELP:
      break;
    case ACTION_ROTATE_SCREEN:
      if (isRotationSupported()) {
        int optionValue = RotationAngleOption.value();
        ZLViewWidget::Angle oldAngle = myViewWidget->rotation();
        ZLViewWidget::Angle newAngle = ZLViewWidget::DEGREES0;
        if (optionValue == -1) {
          switch (oldAngle) {
            case ZLViewWidget::DEGREES0:
              newAngle = ZLViewWidget::DEGREES90;
              break;
            case ZLViewWidget::DEGREES90:
              newAngle = ZLViewWidget::DEGREES180;
              break;
            case ZLViewWidget::DEGREES180:
              newAngle = ZLViewWidget::DEGREES270;
              break;
            case ZLViewWidget::DEGREES270:
              newAngle = ZLViewWidget::DEGREES0;
              break;
          }
        } else {
          newAngle = (oldAngle == ZLViewWidget::DEGREES0) ?
            (ZLViewWidget::Angle)optionValue : ZLViewWidget::DEGREES0;
        }
        myViewWidget->rotate(newAngle);
        AngleStateOption.setValue(newAngle);
        repaintView();
      }
      break;
    case ACTION_QUIT:
      if (myMode == BOOK_TEXT_MODE) {
        quitSlot();
      } else {
        restorePreviousMode();
      }
      break;
    case ACTION_OPEN_PREVIOUS_BOOK:
    {
      Books books = myRecentBooksView->lastBooks().books();
      if (books.size() > 1) {
        openBook(books[1]);
        repaintView();
        resetWindowCaption();
      }
      break;
    }
  }
}

void FBReader::enableMenuButtons() {
  TextView *textView = (TextView*)myViewWidget->view();
  setActionEnabled(ACTION_FIND_NEXT, textView->canFindNext());
  setActionEnabled(ACTION_FIND_PREVIOUS, textView->canFindPrevious());
  setActionEnabled(ACTION_SHOW_CONTENTS, !myContentsView->isEmpty());
  setActionEnabled(ACTION_UNDO, (myMode != BOOK_TEXT_MODE) || myBookTextView->canUndoPageMove());
  setActionEnabled(ACTION_REDO, myBookTextView->canRedoPageMove());
  if (isRotationSupported()) {
    setActionVisible(ACTION_ROTATE_SCREEN,
      (RotationAngleOption.value() != ZLViewWidget::DEGREES0) ||
      (myViewWidget->rotation() != ZLViewWidget::DEGREES0));
  }
	refresh();
}

class RebuildCollectionRunnable : public ZLRunnable {

public:
  RebuildCollectionRunnable(FBReader &reader) : myReader(reader) {}
  void run() { myReader.rebuildCollectionInternal(); }

private:
  FBReader &myReader;
};

void FBReader::rebuildCollectionInternal() {
  myCollectionView->rebuild();
}

void FBReader::setMode(ViewMode mode) {
  if (mode == myMode) {
    return;
  }

  myPreviousMode = myMode;
  myMode = mode;

  switch (myMode) {
    case BOOK_TEXT_MODE:
      setActionVisible(ACTION_SHOW_COLLECTION, true);
      setActionVisible(ACTION_SHOW_LAST_BOOKS, true);
      setActionVisible(ACTION_ADD_BOOK, true);
      setActionVisible(ACTION_SHOW_BOOK_INFO, true);
      setActionVisible(ACTION_SCROLL_TO_HOME, true);
      setActionVisible(ACTION_REDO, true);
      setActionVisible(ACTION_SHOW_CONTENTS, true);
      myViewWidget->setView(myBookTextView);
      break;
    case CONTENTS_MODE:
      setActionVisible(ACTION_SHOW_COLLECTION, true);
      setActionVisible(ACTION_SHOW_LAST_BOOKS, true);
      setActionVisible(ACTION_ADD_BOOK, true);
      setActionVisible(ACTION_SHOW_BOOK_INFO, true);
      setActionVisible(ACTION_SCROLL_TO_HOME, false);
      setActionVisible(ACTION_REDO, false);
      setActionVisible(ACTION_SHOW_CONTENTS, false);
      if (!StoreContentsPositionOption.value()) {
        myContentsView->gotoReference();
      }
      myViewWidget->setView(myContentsView);
      break;
    case FOOTNOTE_MODE:
      setActionVisible(ACTION_SHOW_COLLECTION, false);
      setActionVisible(ACTION_SHOW_LAST_BOOKS, false);
      setActionVisible(ACTION_ADD_BOOK, false);
      setActionVisible(ACTION_SHOW_BOOK_INFO, true);
      setActionVisible(ACTION_SCROLL_TO_HOME, false);
      setActionVisible(ACTION_REDO, false);
      setActionVisible(ACTION_SHOW_CONTENTS, true);
      myViewWidget->setView(myFootnoteView);
      break;
    case BOOK_COLLECTION_MODE:
      setActionVisible(ACTION_SHOW_COLLECTION, false);
      setActionVisible(ACTION_SHOW_LAST_BOOKS, false);
      setActionVisible(ACTION_ADD_BOOK, true);
      setActionVisible(ACTION_SHOW_BOOK_INFO, false);
      setActionVisible(ACTION_SCROLL_TO_HOME, false);
      setActionVisible(ACTION_REDO, false);
      setActionVisible(ACTION_SHOW_CONTENTS, false);
      {
        RebuildCollectionRunnable runnable(*this);
        ZLDialogManager::instance().wait(runnable, "Loading book list. Please, wait...");
      }
      myViewWidget->setView(myCollectionView);
      break;
    case RECENT_BOOKS_MODE:
      setActionVisible(ACTION_SHOW_COLLECTION, true);
      setActionVisible(ACTION_SHOW_LAST_BOOKS, false);
      setActionVisible(ACTION_ADD_BOOK, true);
      setActionVisible(ACTION_SHOW_BOOK_INFO, false);
      setActionVisible(ACTION_SCROLL_TO_HOME, false);
      setActionVisible(ACTION_REDO, false);
      setActionVisible(ACTION_SHOW_CONTENTS, false);
      myRecentBooksView->rebuild();
      myViewWidget->setView(myRecentBooksView);
      break;
    case BOOKMARKS_MODE:
      break;
    case UNDEFINED_MODE:
      break;
  }
  resetWindowCaption();
}

void FBReader::resetWindowCaption() {
  setWindowCaption("FBReader - " + myViewWidget->view()->caption());
}

bool FBReader::runBookInfoDialog(const std::string &fileName) {
  if (InfoDialog(fileName).dialog().run("")) {
    BookDescriptionPtr newDescription = BookDescription::create(fileName);
    if (!newDescription.isNull()) {
      openBook(newDescription);
      setWindowCaption("FBReader - " + myViewWidget->view()->caption());
      repaintView();
    } else {
      // TODO: show information message
    }
		return true;
  }
  return false;
}

BookTextView &FBReader::textView() const {
  return *myBookTextView;
}

void FBReader::showBookTextView() {
  setMode(BOOK_TEXT_MODE);
}

void FBReader::restorePreviousMode() {
  setMode(myPreviousMode);
  myPreviousMode = BOOK_TEXT_MODE;
}

void FBReader::clearTextCaches() {
  myBookTextView->clearCaches();
  myFootnoteView->clearCaches();
  myContentsView->clearCaches();
  myCollectionView->clearCaches();
  myRecentBooksView->clearCaches();
}

void FBReader::doAction(const std::string &key) {
  ActionCode code = keyBindings(myViewWidget->rotation()).getBinding(key);
  if (code != NO_ACTION) {
    if (isScrollingAction(code) || (myLastKeyActionTime.millisecondsTo(ZLTime()) >= KeyDelayOption.value())) {
      doAction(code);
      myLastKeyActionTime = ZLTime();
    }
  }
}

bool FBReader::isFullKeyboardControlSupported() const {
  return false;
}

void FBReader::grabAllKeys(bool) {
}

// vim:ts=2:sw=2:noet
