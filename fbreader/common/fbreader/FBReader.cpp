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

#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLOptionsDialog.h>

#include "FBReader.h"
#include "BookTextView.h"
#include "FootnoteView.h"
#include "ContentsView.h"
#include "CollectionView.h"
#include "OptionsDialog.h"

#include "../description/BookDescription.h"
#include "../bookmodel/BookModel.h"
#include "../view/PaintContext.h"
#include "../hyphenation/Hyphenator.h"
#include "../textview/TextStyle.h"

ZLBooleanOption FBReader::QuitOnCancelOption("Options", "QuitOnCancel", false);
ZLIntegerOption FBReader::ScrollingDelayOption("Options", "ScrollingDelay", 350);

ZLBooleanOption FBReader::SearchBackwardOption("Search", "Backward", false);
ZLBooleanOption FBReader::SearchIgnoreCaseOption("Search", "IgnoreCase", true);
ZLBooleanOption FBReader::SearchInWholeTextOption("Search", "WholeText", false);
ZLStringOption FBReader::SearchPatternOption("Search", "Pattern", std::string());

FBReader::FBReader(PaintContext *context) {
	myModel = 0;
	myContext = context;
	myBookTextView = new BookTextView(*this, *myContext);
	myFootnoteView = new FootnoteView(*myContext);
	myContentsView = new ContentsView(*this, *myContext);
	myCollectionView = new CollectionView(*this, *myContext);
	myMode = UNDEFINED_MODE;
	myPreviousMode = BOOK_TEXT_MODE;

	std::string howToStartString = HelpDirectory + "/HowToStart";
	ZLStringOption bookName("State", "Book", howToStartString);
	BookDescription *description = BookDescription::create(bookName.value());
	if (description == 0) {
		description = BookDescription::create(howToStartString);
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
		ZLStringOption bookName("State", "Book", std::string());
		bookName.setValue(myModel->fileName());
		delete myModel;
	}
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
			if (mode() == BOOK_TEXT_MODE) {
				myBookTextView->undoPageMove();
			}
			break;
		case ACTION_REDO:
			if (mode() == BOOK_TEXT_MODE) {
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
	}
}
