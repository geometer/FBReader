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
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLOptionsDialog.h>

#include "FBReader.h"
#include "BookTextView.h"
#include "FootnoteView.h"
#include "ContentsView.h"
#include "CollectionView.h"
#include "OptionsDialog.h"
#include "FBFileHandler.h"
#include "InfoOptions.h"

#include "../collection/BookList.h"
#include "../description/BookDescription.h"
#include "../bookmodel/BookModel.h"
#include "../view/PaintContext.h"
#include "../hyphenation/Hyphenator.h"
#include "../textview/TextStyle.h"
#include "../textview/ParagraphCursor.h"
#include "../formats/FormatPlugin.h"

static const std::string OPTIONS = "Options";
static const std::string SEARCH = "Search";
static const std::string STATE = "State";
static const std::string BOOK = "Book";

ZLBooleanOption FBReader::QuitOnCancelOption(OPTIONS, "QuitOnCancel", false);
ZLIntegerOption FBReader::ScrollingDelayOption(OPTIONS, "ScrollingDelay", 250);

ZLBooleanOption FBReader::SearchBackwardOption(SEARCH, "Backward", false);
ZLBooleanOption FBReader::SearchIgnoreCaseOption(SEARCH, "IgnoreCase", true);
ZLBooleanOption FBReader::SearchInWholeTextOption(SEARCH, "WholeText", false);
ZLStringOption FBReader::SearchPatternOption(SEARCH, "Pattern", std::string());

FBReader::FBReader(PaintContext *context) {
	myModel = 0;
	myContext = context;
	myBookTextView = new BookTextView(*this, *myContext);
	myFootnoteView = new FootnoteView(*myContext);
	myContentsView = new ContentsView(*this, *myContext);
	myCollectionView = new CollectionView(*this, *myContext);
	myMode = UNDEFINED_MODE;
	myPreviousMode = BOOK_TEXT_MODE;

#ifndef PALM_TEMPORARY
	std::string howToStartString = HelpDirectory + "/HowToStart.fb2";
#else // PALM_TEMPORARY
	std::string howToStartString = "/Help.txt";
#endif // PALM_TEMPORARY
	ZLStringOption bookName(STATE, BOOK, howToStartString);
	BookDescription *description = BookDescription::create(bookName.value());
	if (description == 0) {
#ifndef PALM_TEMPORARY
		description = BookDescription::create(howToStartString);
#else // PALM_TEMPORARY
		description = BookDescription::create("$$TEST");
#endif // PALM_TEMPORARY
	}
	openBook(description);
}

FBReader::~FBReader() {
	delete myContext;
	myBookTextView->saveState();
	delete myBookTextView;
	delete myFootnoteView;
	myContentsView->saveState();
	delete myContentsView;
	delete myCollectionView;
	if (myModel != 0) {
		ZLStringOption bookName(STATE, BOOK, std::string());
		bookName.setValue(myModel->fileName());
		delete myModel;
	}
	TextStyleCollection::deleteInstance();
	PluginCollection::deleteInstance();
	Hyphenator::deleteInstance();
	ParagraphCursor::ParagraphProcessor::clean();
}

void FBReader::openBook(BookDescription *description) {
	if (description != 0) {
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
		myContext->removeCaches();
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

void FBReader::doAction(ActionCode code) {
	switch (code) {
		case ACTION_SHOW_COLLECTION:
			setMode(BOOK_COLLECTION_MODE);
			break;
		case ACTION_SHOW_OPTIONS:
			{
				OptionsDialog optionsDialog(*myContext);
				optionsDialog.dialog().run("");
				repaintView();
			}
			break;
		case ACTION_UNDO:
			if (myMode == BOOK_TEXT_MODE) {
				myBookTextView->undoPageMove();
			}
			break;
		case ACTION_REDO:
			if (myMode == BOOK_TEXT_MODE) {
				myBookTextView->redoPageMove();
			}
			break;
		case ACTION_SHOW_CONTENTS:
			if (!myContentsView->isEmpty()) {
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
		case ACTION_SCROLL_FORWARD:
			if (myLastScrollingTime.millisecondsTo(ZLTime()) >= ScrollingDelayOption.value()) {
				((TextView*)myViewWidget->view())->scrollPageForward();
				repaintView();
				myLastScrollingTime = ZLTime();
			}
			break;
		case ACTION_SCROLL_BACKWARD:
			if (myLastScrollingTime.millisecondsTo(ZLTime()) >= ScrollingDelayOption.value()) {
				((TextView*)myViewWidget->view())->scrollPageBackward();
				repaintView();
				myLastScrollingTime = ZLTime();
			}
			break;
		case ACTION_CANCEL:
			cancelSlot();
			break;
		case ACTION_INCREASE_FONT:
			{
				ZLIntegerOption &option = TextStyleCollection::instance().baseStyle().fontSizeOption();
				int value = option.value() + 2;
				if (value <= 32) {
					option.setValue(value);
					repaintView();
				}
			}
			break;
		case ACTION_DECREASE_FONT:
			{
				ZLIntegerOption &option = TextStyleCollection::instance().baseStyle().fontSizeOption();
				int value = option.value() - 2;
				if (value >= 10) {
					option.setValue(value);
					repaintView();
				}
			}
			break;
		case ACTION_SHOW_HIDE_POSITION_INDICATOR:
			TextView::ShowPositionIndicatorOption.setValue(!TextView::ShowPositionIndicatorOption.value());
			repaintView();
			break;
		case ACTION_FULLSCREEN:
			fullscreenSlot();
			break;
		case ACTION_ADD_BOOK:
			{
				FBFileHandler handler;
				ZLDialogManager::instance().openFileDialog("FBReader -- Add File To Library", handler);
				BookDescription *description = handler.description();
				if (description != 0) {
					if (runBookInfoDialog(description->fileName())) {
						BookList().addFileName(description->fileName());
						setMode(BOOK_TEXT_MODE);
					}
					delete description;
				}
			}
			break;
		case ACTION_SHOW_BOOK_INFO:
			if ((myMode == BOOK_TEXT_MODE) || (myMode == CONTENTS_MODE) || (myMode == FOOTNOTE_MODE)) {
				runBookInfoDialog(myModel->fileName());
			}
			break;
		case ACTION_SHOW_HELP:
			break;
		case ACTION_ROTATE_SCREEN:
			myViewWidget->rotate();
			repaintView();
			break;
	}
}

void FBReader::enableMenuButtons() {
	TextView *textView = (TextView*)myViewWidget->view();
	setButtonEnabled(ACTION_FIND_NEXT, textView->canFindNext());
	setButtonEnabled(ACTION_FIND_PREVIOUS, textView->canFindPrevious());
	setButtonEnabled(ACTION_SHOW_CONTENTS, !myContentsView->isEmpty());
	setButtonEnabled(ACTION_UNDO, myBookTextView->canUndoPageMove());
	setButtonEnabled(ACTION_REDO, myBookTextView->canRedoPageMove());
}

void FBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}

	myPreviousMode = myMode;
	myMode = mode;

	switch (myMode) {
		case BOOK_TEXT_MODE:
			setButtonVisible(ACTION_SHOW_COLLECTION, true);
			setButtonVisible(ACTION_ADD_BOOK, true);
			setButtonVisible(ACTION_SHOW_BOOK_INFO, true);
			setButtonVisible(ACTION_UNDO, true);
			setButtonVisible(ACTION_REDO, true);
			setButtonVisible(ACTION_SHOW_CONTENTS, true);
			myViewWidget->setView(myBookTextView);
			break;
		case CONTENTS_MODE:
			setButtonVisible(ACTION_SHOW_COLLECTION, true);
			setButtonVisible(ACTION_ADD_BOOK, true);
			setButtonVisible(ACTION_SHOW_BOOK_INFO, true);
			setButtonVisible(ACTION_UNDO, false);
			setButtonVisible(ACTION_REDO, false);
			setButtonVisible(ACTION_SHOW_CONTENTS, false);
			myViewWidget->setView(myContentsView);
			break;
		case FOOTNOTE_MODE:
			setButtonVisible(ACTION_SHOW_COLLECTION, false);
			setButtonVisible(ACTION_ADD_BOOK, false);
			setButtonVisible(ACTION_SHOW_BOOK_INFO, true);
			setButtonVisible(ACTION_UNDO, false);
			setButtonVisible(ACTION_REDO, false);
			setButtonVisible(ACTION_SHOW_CONTENTS, true);
			myViewWidget->setView(myFootnoteView);
			break;
		case BOOK_COLLECTION_MODE:
			setButtonVisible(ACTION_SHOW_COLLECTION, false);
			setButtonVisible(ACTION_ADD_BOOK, true);
			setButtonVisible(ACTION_SHOW_BOOK_INFO, false);
			setButtonVisible(ACTION_UNDO, false);
			setButtonVisible(ACTION_REDO, false);
			setButtonVisible(ACTION_SHOW_CONTENTS, false);
			myCollectionView->fill();
			myViewWidget->setView(myCollectionView);
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
	setWindowCaption("FBReader - " + myViewWidget->view()->caption());
}

void FBReader::createToolbar() {
	addButton(ACTION_SHOW_COLLECTION, "books");
	addButton(ACTION_ADD_BOOK, "addbook");
	addButton(ACTION_SHOW_BOOK_INFO, "bookinfo");
	addButton(ACTION_SHOW_OPTIONS, "settings");
	addButton(ACTION_UNDO, "leftarrow");
	addButton(ACTION_REDO, "rightarrow");
	addButton(ACTION_SHOW_CONTENTS, "contents");
	addButton(ACTION_SEARCH, "find");
	addButton(ACTION_FIND_NEXT, "findnext");
	addButton(ACTION_FIND_PREVIOUS, "findprev");
	if (isRotationSupported()) {
		addButton(ACTION_ROTATE_SCREEN, "rotatescreen");
	}
}

bool FBReader::runBookInfoDialog(const std::string &fileName) {
	bool code;
	{
		BookInfo info(fileName);

		ZLOptionsDialog *infoDialog = ZLDialogManager::instance().createOptionsDialog("InfoDialog", "FBReader - Book Info");
		ZLOptionsDialogTab *infoTab = infoDialog->createTab("Info");
		infoTab->addOption(new ZLSimpleStringOptionEntry("Title", info.TitleOption));
		infoTab->addOption(new ZLSimpleStringOptionEntry("Author (display name)", info.AuthorDisplayNameOption));
		infoTab->addOption(new ZLSimpleStringOptionEntry("Author (sort name)", info.AuthorSortKeyOption));
		infoTab->addOption(new LanguageEntry("Language", info.LanguageOption));
		infoTab->addOption(new EncodingEntry("Encoding", info.EncodingOption));
		code = infoDialog->run("");
		delete infoDialog;
	}
	if (code) {
		BookDescription *newDescription = BookDescription::create(fileName);
		if (newDescription != 0) {
			openBook(newDescription);
			setWindowCaption("FBReader - " + myViewWidget->view()->caption());
			repaintView();
		} else {
			// TODO: show information message
		}
	}
	return code;
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
