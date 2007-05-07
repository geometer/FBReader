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
	fbreader().setMode(FBReader::BOOK_COLLECTION_MODE);
}

bool ShowCollectionAction::isVisible() {
	FBReader::ViewMode mode = fbreader().myMode;
	return (mode != FBReader::FOOTNOTE_MODE) && (mode != FBReader::BOOK_COLLECTION_MODE);
}

ShowHelpAction::ShowHelpAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowHelpAction::run() {
	fbreader().openFile(fbreader().helpFileName());
	fbreader().setMode(FBReader::BOOK_TEXT_MODE);
}

ShowRecentBooksListAction::ShowRecentBooksListAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowRecentBooksListAction::run() {
	fbreader().setMode(FBReader::RECENT_BOOKS_MODE);
}

bool ShowRecentBooksListAction::isVisible() {
	FBReader::ViewMode mode = fbreader().myMode;
	return
		(mode != FBReader::FOOTNOTE_MODE) &&
		(mode != FBReader::BOOK_COLLECTION_MODE) &&
		(mode != FBReader::RECENT_BOOKS_MODE);
}

ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowOptionsDialogAction::run() {
	FBReader &f = fbreader();
	OptionsDialog(f).dialog().run("");
}

ShowContentsAction::ShowContentsAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ShowContentsAction::isVisible() {
	FBReader::ViewMode mode = fbreader().myMode;
	return (mode == FBReader::BOOK_TEXT_MODE) || (mode == FBReader::FOOTNOTE_MODE);
}

bool ShowContentsAction::isEnabled() {
	return isVisible() && !((ContentsView&)*fbreader().myContentsView).isEmpty();
}

void ShowContentsAction::run() {
	fbreader().setMode(FBReader::CONTENTS_MODE);
}

AddBookAction::AddBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool AddBookAction::isVisible() {
	return fbreader().myMode != FBReader::FOOTNOTE_MODE;
}

void AddBookAction::run() {
	fbreader().addBookSlot();
}

ScrollToHomeAction::ScrollToHomeAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToHomeAction::isVisible() {
	return fbreader().myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToHomeAction::run() {
	fbreader().bookTextView().scrollToHome();
}

ScrollToStartOfTextAction::ScrollToStartOfTextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToStartOfTextAction::isVisible() {
	return fbreader().myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToStartOfTextAction::run() {
	fbreader().bookTextView().scrollToStartOfText();
}

ScrollToEndOfTextAction::ScrollToEndOfTextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ScrollToEndOfTextAction::isVisible() {
	return fbreader().myMode == FBReader::BOOK_TEXT_MODE;
}

void ScrollToEndOfTextAction::run() {
	fbreader().bookTextView().scrollToEndOfText();
}

ShowBookInfoAction::ShowBookInfoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ShowBookInfoAction::isVisible() {
	return
		(fbreader().myMode == FBReader::BOOK_TEXT_MODE) ||
		(fbreader().myMode == FBReader::CONTENTS_MODE) ||
		(fbreader().myMode == FBReader::FOOTNOTE_MODE);
}

void ShowBookInfoAction::run() {
	fbreader().bookInfoSlot();
}

UndoAction::UndoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool UndoAction::isEnabled() {
	return (fbreader().myMode != FBReader::BOOK_TEXT_MODE) ||
					fbreader().bookTextView().canUndoPageMove();
}

void UndoAction::run() {
	if (fbreader().myMode == FBReader::BOOK_TEXT_MODE) {
		fbreader().bookTextView().undoPageMove();
	} else {
		fbreader().restorePreviousMode();
	}
}

RedoAction::RedoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool RedoAction::isVisible() {
	return fbreader().myMode == FBReader::BOOK_TEXT_MODE;
}

bool RedoAction::isEnabled() {
	return isVisible() && fbreader().bookTextView().canRedoPageMove();
}

void RedoAction::run() {
	fbreader().bookTextView().redoPageMove();
}

SearchAction::SearchAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SearchAction::isVisible() {
	return fbreader().currentView() != 0;
}

void SearchAction::run() {
	fbreader().searchSlot();
}

FindNextAction::FindNextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindNextAction::isEnabled() {
	shared_ptr<ZLView> view = fbreader().currentView();
	return (!view.isNull()) && ((TextView&)*view).canFindNext();
}

void FindNextAction::run() {
	((TextView&)*fbreader().currentView()).findNext();
}

FindPreviousAction::FindPreviousAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindPreviousAction::isEnabled() {
	shared_ptr<ZLView> view = fbreader().currentView();
	return (!view.isNull()) && ((TextView&)*view).canFindPrevious();
}

void FindPreviousAction::run() {
	((TextView&)*fbreader().currentView()).findPrevious();
}

ScrollingAction::ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward) : FBAction(fbreader), myOptions(options), myForward(forward) {
}

bool ScrollingAction::isEnabled() {
	return
		(&myOptions != &fbreader().FingerTapScrollingOptions) ||
		fbreader().EnableFingerScrollingOption.value();
}

bool ScrollingAction::useKeyDelay() const {
	return false;
}

void ScrollingAction::run() {
	int delay = fbreader().myLastScrollingTime.millisecondsTo(ZLTime());
	shared_ptr<ZLView> view = fbreader().currentView();
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
		fbreader().refreshWindow();
		fbreader().myLastScrollingTime = ZLTime();
	}
}

ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : FBAction(fbreader), myDelta(delta) {
}

void ChangeFontSizeAction::run() {
	ZLIntegerRangeOption &option = TextStyleCollection::instance().baseStyle().FontSizeOption;
	option.setValue(option.value() + myDelta);
	fbreader().clearTextCaches();
	fbreader().refreshWindow();
}

OpenPreviousBookAction::OpenPreviousBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool OpenPreviousBookAction::isVisible() {
	if ((fbreader().myMode != FBReader::BOOK_TEXT_MODE) && (fbreader().myMode != FBReader::CONTENTS_MODE)) {
		return false;
	}
	return ((RecentBooksView&)*fbreader().myRecentBooksView).lastBooks().books().size() > 1;
}

void OpenPreviousBookAction::run() {
	Books books = ((RecentBooksView&)*fbreader().myRecentBooksView).lastBooks().books();
	fbreader().openBook(books[1]);
	fbreader().refreshWindow();
	fbreader().resetWindowCaption();
}

CancelAction::CancelAction(FBReader &fbreader) : FBAction(fbreader) {
}

void CancelAction::run() {
	if (fbreader().myMode != FBReader::BOOK_TEXT_MODE) {
		fbreader().restorePreviousMode();
	} else if (fbreader().isFullscreen()) {
		fbreader().setFullscreen(false);
	} else if (fbreader().QuitOnCancelOption.value()) {
		fbreader().quit();
	}
}

ToggleIndicatorAction::ToggleIndicatorAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ToggleIndicatorAction::run() {
	ZLBooleanOption &option = TextStyleCollection::instance().indicatorStyle().ShowOption;
	option.setValue(!option.value());
	fbreader().refreshWindow();
}

QuitAction::QuitAction(FBReader &fbreader) : FBAction(fbreader) {
}

void QuitAction::run() {
	fbreader().closeView();
}

GotoNextTOCSectionAction::GotoNextTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoNextTOCSectionAction::isVisible() {
	if (fbreader().myMode != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoNextTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && ((int)contentsView.currentTextViewParagraph() < (int)model->paragraphsNumber() - 1);
}

void GotoNextTOCSectionAction::run() {
	ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();
	int reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current + 1]));
	((TextView&)*fbreader().myBookTextView).gotoParagraph(reference);
	fbreader().refreshWindow();
}

GotoPreviousTOCSectionAction::GotoPreviousTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoPreviousTOCSectionAction::isVisible() {
	if (fbreader().myMode != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<TextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoPreviousTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
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
		const WordCursor &cursor = fbreader().bookTextView().startCursor();
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
	ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();

	int reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current]));
	const WordCursor &cursor = fbreader().bookTextView().startCursor();
	if (!cursor.isNull() &&
			(cursor.wordNumber() == 0) &&
			(reference == (int)cursor.paragraphCursor().index())) {
		reference = contentsModel.reference(((const TreeParagraph*)contentsModel[current - 1]));
	}
	((TextView&)*fbreader().myBookTextView).gotoParagraph(reference);
	fbreader().refreshWindow();
}

SelectionAction::SelectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SelectionAction::isVisible() {
	return fbreader().currentView() != 0;
}

bool SelectionAction::isEnabled() {
	return isVisible() && !textView().selectionModel().getText().empty();
}

TextView &SelectionAction::textView() {
	return (TextView&)*fbreader().currentView();
}

CopySelectedTextAction::CopySelectedTextAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

bool CopySelectedTextAction::isVisible() {
	return SelectionAction::isVisible() && ZLDialogManager::instance().isClipboardSupported(ZLDialogManager::CLIPBOARD_MAIN);
}

void CopySelectedTextAction::run() {
	textView().copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_MAIN);
}

OpenSelectedTextInDictionaryAction::OpenSelectedTextInDictionaryAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

bool OpenSelectedTextInDictionaryAction::isVisible() {
	if (!SelectionAction::isVisible()) {
		return false;
	}
	shared_ptr<ProgramCollection> dictionaryCollection = fbreader().dictionaryCollection();
	return !dictionaryCollection.isNull() && !dictionaryCollection->currentProgram().isNull();
}

void OpenSelectedTextInDictionaryAction::run() {
	fbreader().dictionaryCollection()->currentProgram()->run(
		"showWord", textView().selectionModel().getText()
	);
}

ClearSelectionAction::ClearSelectionAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

void ClearSelectionAction::run() {
	textView().selectionModel().clear();
	textView().repaintView();
}
