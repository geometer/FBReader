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

void FBReader::nextPage() {
	// TODO: check, if current view is TextView
	((TextView*)myViewWidget->view())->scrollPageForward();
	repaintView();
}

void FBReader::previousPage() {
	// TODO: check, if current view is TextView
	((TextView*)myViewWidget->view())->scrollPageBackward();
	repaintView();
}

void FBReader::undoPage() {
	if (mode() == BOOK_TEXT_MODE) {
		myBookTextView->undoPageMove();
	}
}

void FBReader::redoPage() {
	if (mode() == BOOK_TEXT_MODE) {
		myBookTextView->redoPageMove();
	}
}

void FBReader::findNext() {
	// TODO: check, if current view is TextView
	((TextView*)myViewWidget->view())->findNext();
}

void FBReader::findPrevious() {
	// TODO: check, if current view is TextView
	((TextView*)myViewWidget->view())->findPrevious();
}

void FBReader::showOptionsDialog() {
	OptionsDialog *optionsDialog = new OptionsDialog(*myContext);
	optionsDialog->dialog().run("");
	delete optionsDialog;
}

void FBReader::decreaseFont() {
	ZLIntegerOption &option = TextStyleCollection::instance().baseStyle().fontSizeOption();
	int value = option.value() - 2;
	if (value >= 10) {
		option.setValue(value);
		repaintView();
	}
}

void FBReader::increaseFont() {
	ZLIntegerOption &option = TextStyleCollection::instance().baseStyle().fontSizeOption();
	int value = option.value() + 2;
	if (value <= 32) {
		option.setValue(value);
		repaintView();
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

void FBReader::showHidePositionIndicator() {
	TextView::ShowPositionIndicatorOption.setValue(!TextView::ShowPositionIndicatorOption.value());
	repaintView();
}

void FBReader::repaintView() {
	if (myViewWidget != 0) {
		myViewWidget->repaintView();
	}
}
