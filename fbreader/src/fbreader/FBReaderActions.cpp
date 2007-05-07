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

#include "FBReader.h"
#include "FBReaderActions.h"
#include "BookTextView.h"
#include "ContentsView.h"
#include "RecentBooksView.h"

#include "../bookmodel/BookModel.h"
#include "../textview/TextView.h"
#include "../optionsDialog/OptionsDialog.h"

FBAction::FBAction(FBReader &fbreader) : myFBReader(fbreader) {
}

ShowCollectionAction::ShowCollectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowCollectionAction::run() {
	myFBReader.setMode(FBReader::BOOK_COLLECTION_MODE);
}

bool ShowCollectionAction::isVisible() {
	FBReader::ViewMode mode = myFBReader.myMode;
	return (mode != FBReader::FOOTNOTE_MODE) && (mode != FBReader::BOOK_COLLECTION_MODE);
}

ShowHelpAction::ShowHelpAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowHelpAction::run() {
	myFBReader.openFile(myFBReader.helpFileName());
	myFBReader.setMode(FBReader::BOOK_TEXT_MODE);
}

ShowRecentBooksListAction::ShowRecentBooksListAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowRecentBooksListAction::run() {
	myFBReader.setMode(FBReader::RECENT_BOOKS_MODE);
}

bool ShowRecentBooksListAction::isVisible() {
	FBReader::ViewMode mode = myFBReader.myMode;
	return
		(mode != FBReader::FOOTNOTE_MODE) &&
		(mode != FBReader::BOOK_COLLECTION_MODE) &&
		(mode != FBReader::RECENT_BOOKS_MODE);
}

ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowOptionsDialogAction::run() {
	OptionsDialog(myFBReader).dialog().run("");
}

ShowContentsAction::ShowContentsAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ShowContentsAction::isVisible() {
	FBReader::ViewMode mode = myFBReader.myMode;
	return (mode == FBReader::BOOK_TEXT_MODE) || (mode == FBReader::FOOTNOTE_MODE);
}

bool ShowContentsAction::isEnabled() {
	return isVisible() && !((ContentsView&)*myFBReader.myContentsView).isEmpty();
}

void ShowContentsAction::run() {
	myFBReader.setMode(FBReader::CONTENTS_MODE);
}

AddBookAction::AddBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool AddBookAction::isVisible() {
	return myFBReader.myMode != FBReader::FOOTNOTE_MODE;
}

void AddBookAction::run() {
	myFBReader.addBookSlot();
}

ScrollToHomeAction::ScrollToHomeAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToHomeAction::isVisible() {
	return myFBReader.myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToHomeAction::run() {
	myFBReader.bookTextView().scrollToHome();
}

ScrollToStartOfTextAction::ScrollToStartOfTextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToStartOfTextAction::isVisible() {
	return myFBReader.myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToStartOfTextAction::run() {
	myFBReader.bookTextView().scrollToStartOfText();
}

ScrollToEndOfTextAction::ScrollToEndOfTextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToEndOfTextAction::isVisible() {
	return myFBReader.myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToEndOfTextAction::run() {
	myFBReader.bookTextView().scrollToEndOfText();
}

ShowBookInfoAction::ShowBookInfoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ShowBookInfoAction::isVisible() {
	return
		(myFBReader.myMode == FBReader::BOOK_TEXT_MODE) ||
		(myFBReader.myMode == FBReader::CONTENTS_MODE) ||
		(myFBReader.myMode == FBReader::FOOTNOTE_MODE);
}

void ShowBookInfoAction::run() {
	myFBReader.bookInfoSlot();
}

UndoAction::UndoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool UndoAction::isEnabled() {
	return (myFBReader.myMode != FBReader::BOOK_TEXT_MODE) ||
					myFBReader.bookTextView().canUndoPageMove();
}

void UndoAction::run() {
	if (myFBReader.myMode == FBReader::BOOK_TEXT_MODE) {
		myFBReader.bookTextView().undoPageMove();
	} else {
		myFBReader.restorePreviousMode();
	}
}

RedoAction::RedoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool RedoAction::isVisible() {
	return myFBReader.myMode == FBReader::BOOK_TEXT_MODE;
}

bool RedoAction::isEnabled() {
	return isVisible() && myFBReader.bookTextView().canRedoPageMove();
}

void RedoAction::run() {
	myFBReader.bookTextView().redoPageMove();
}

SearchAction::SearchAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SearchAction::isVisible() {
	return myFBReader.currentView() != 0;
}

void SearchAction::run() {
	myFBReader.searchSlot();
}

FindNextAction::FindNextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindNextAction::isEnabled() {
	shared_ptr<ZLView> view = myFBReader.currentView();
	return (!view.isNull()) && ((TextView&)*view).canFindNext();
}

void FindNextAction::run() {
	((TextView&)*myFBReader.currentView()).findNext();
}

FindPreviousAction::FindPreviousAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindPreviousAction::isEnabled() {
	shared_ptr<ZLView> view = myFBReader.currentView();
	return (!view.isNull()) && ((TextView&)*view).canFindPrevious();
}

void FindPreviousAction::run() {
	((TextView&)*myFBReader.currentView()).findPrevious();
}

ScrollingAction::ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward) : FBAction(fbreader), myOptions(options), myForward(forward) {
}

bool ScrollingAction::isEnabled() {
	return
		(&myOptions != &myFBReader.FingerTapScrollingOptions) ||
		myFBReader.EnableFingerScrollingOption.value();
}

bool ScrollingAction::useKeyDelay() const {
	return false;
}

void ScrollingAction::run() {
	int delay = myFBReader.myLastScrollingTime.millisecondsTo(ZLTime());
	shared_ptr<ZLView> view = myFBReader.currentView();
	if (!view.isNull() && ((delay < 0) || (delay >= myOptions.DelayOption.value()))) {
		TextView::ScrollingMode oType = (TextView::ScrollingMode)myOptions.ModeOption.value();
		unsigned int oValue = 0;
		switch (oType) {
			case TextView::KEEP_LINES:
				oValue = myOptions.LinesToKeepOption.value();
				break;
			case TextView::SCROLL_LINES:
				oValue = myOptions.LinesToScrollOption.value();
				break;
			case TextView::SCROLL_PERCENTAGE:
				oValue = myOptions.PercentToScrollOption.value();
				break;
			default:
				break;
		}
		((TextView&)*view).scrollPage(myForward, oType, oValue);
		myFBReader.refreshWindow();
		myFBReader.myLastScrollingTime = ZLTime();
	}
}

ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : FBAction(fbreader), myDelta(delta) {
}

void ChangeFontSizeAction::run() {
	ZLIntegerRangeOption &option = TextStyleCollection::instance().baseStyle().FontSizeOption;
	option.setValue(option.value() + myDelta);
	myFBReader.clearTextCaches();
	myFBReader.refreshWindow();
}

OpenPreviousBookAction::OpenPreviousBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool OpenPreviousBookAction::isVisible() {
	if ((myFBReader.myMode != FBReader::BOOK_TEXT_MODE) && (myFBReader.myMode != FBReader::CONTENTS_MODE)) {
		return false;
	}
	return ((RecentBooksView&)*myFBReader.myRecentBooksView).lastBooks().books().size() > 1;
}

void OpenPreviousBookAction::run() {
	Books books = ((RecentBooksView&)*myFBReader.myRecentBooksView).lastBooks().books();
	myFBReader.openBook(books[1]);
	myFBReader.refreshWindow();
	myFBReader.resetWindowCaption();
}

CancelAction::CancelAction(FBReader &fbreader) : FBAction(fbreader) {
}

void CancelAction::run() {
	if (myFBReader.myMode != FBReader::BOOK_TEXT_MODE) {
		myFBReader.restorePreviousMode();
	} else if (myFBReader.isFullscreen()) {
		myFBReader.setFullscreen(false);
	} else if (myFBReader.QuitOnCancelOption.value()) {
		myFBReader.quit();
	}
}

ToggleIndicatorAction::ToggleIndicatorAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ToggleIndicatorAction::run() {
	ZLBooleanOption &option = TextStyleCollection::instance().indicatorStyle().ShowOption;
	option.setValue(!option.value());
	myFBReader.refreshWindow();
}

QuitAction::QuitAction(FBReader &fbreader) : FBAction(fbreader) {
}

void QuitAction::run() {
	myFBReader.closeView();
}

GotoNextTOCSectionAction::GotoNextTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoNextTOCSectionAction::isVisible() {
	if (myFBReader.myMode != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*myFBReader.myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoNextTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*myFBReader.myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && ((int)contentsView.currentTextViewParagraph() < (int)model->paragraphsNumber() - 1);
}

void GotoNextTOCSectionAction::run() {
	ContentsView &contentsView = (ContentsView&)*myFBReader.myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();
	int reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current + 1]));
	((TextView&)*myFBReader.myBookTextView).gotoParagraph(reference);
	myFBReader.refreshWindow();
}

GotoPreviousTOCSectionAction::GotoPreviousTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoPreviousTOCSectionAction::isVisible() {
	if (myFBReader.myMode != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*myFBReader.myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoPreviousTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*myFBReader.myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	if (model.isNull()) {
		return false;
	}
	const ContentsModel &contentsModel = (const ContentsModel&)*model;
	int tocIndex = contentsView.currentTextViewParagraph();
	if (tocIndex > 0) {
		return true;
	}
	if (tocIndex == 0) {
		const WordCursor &cursor = myFBReader.bookTextView().startCursor();
		if (cursor.isNull()) {
			return false;
		}
		if (cursor.wordNumber() > 0) {
			return true;
		}
		return
			contentsModel.reference(((const TreeParagraph*)contentsModel[tocIndex])) >
			(int)cursor.paragraphCursor().index();
	}
	return false;
}

void GotoPreviousTOCSectionAction::run() {
	ContentsView &contentsView = (ContentsView&)*myFBReader.myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();

	int reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current]));
	const WordCursor &cursor = myFBReader.bookTextView().startCursor();
	if (!cursor.isNull() &&
			(cursor.wordNumber() == 0) &&
			(reference == (int)cursor.paragraphCursor().index())) {
		reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current - 1]));
	}
	((TextView&)*myFBReader.myBookTextView).gotoParagraph(reference);
	myFBReader.refreshWindow();
}

CopySelectedTextAction::CopySelectedTextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool CopySelectedTextAction::isVisible() {
	return ZLDialogManager::instance().isClipboardSupported(ZLDialogManager::CLIPBOARD_MAIN);
}

bool CopySelectedTextAction::isEnabled() {
	return isVisible() && ((const TextView&)*myFBReader.currentView()).hasSelectedText();
}

void CopySelectedTextAction::run() {
	((const TextView&)*myFBReader.currentView()).copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_MAIN);
}

ClearSelectionAction::ClearSelectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ClearSelectionAction::isVisible() {
	return true;
}

bool ClearSelectionAction::isEnabled() {
	return isVisible() && ((const TextView&)*myFBReader.currentView()).hasSelectedText();
}

void ClearSelectionAction::run() {
	((TextView&)*myFBReader.currentView()).clearSelection();
}
