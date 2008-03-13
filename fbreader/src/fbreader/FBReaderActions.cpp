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

#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLOptionsDialog.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <ZLibrary.h>

#include <ZLTextView.h>

#include "FBReader.h"
#include "FBReaderActions.h"
#include "BookTextView.h"
#include "ContentsView.h"
#include "RecentBooksView.h"
#include "FBFileHandler.h"

#include "../bookmodel/BookModel.h"
#include "../optionsDialog/OptionsDialog.h"

FBAction::FBAction(FBReader &fbreader) : myFBReader(fbreader) {
}

ModeDependentAction::ModeDependentAction(FBReader &fbreader, int visibleInModes) : FBAction(fbreader), myVisibleInModes(visibleInModes) {
}

bool ModeDependentAction::isVisible() {
	return fbreader().getMode() & myVisibleInModes;
}

SetModeAction::SetModeAction(FBReader &fbreader, FBReader::ViewMode modeToSet, int visibleInModes) : ModeDependentAction(fbreader, visibleInModes), myModeToSet(modeToSet) {
}

void SetModeAction::run() {
	fbreader().setMode(myModeToSet);
}

ShowHelpAction::ShowHelpAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowHelpAction::run() {
	BookDescriptionPtr description = BookDescription::getDescription(fbreader().helpFileName(ZLibrary::Language()));
	if (description.isNull()) {
		description = BookDescription::getDescription(fbreader().helpFileName("en"));
	}
	if (!description.isNull()) {
		fbreader().openBook(description);
		fbreader().setMode(FBReader::BOOK_TEXT_MODE);
		fbreader().refreshWindow();
	} else {
		ZLDialogManager::instance().errorBox(ZLResourceKey("noHelpBox"));
	}
}

ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowOptionsDialogAction::run() {
	FBReader &f = fbreader();
	OptionsDialog(f).dialog().run();
}

ShowContentsAction::ShowContentsAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ShowContentsAction::isVisible() {
	if (((ContentsView&)*fbreader().myContentsView).isEmpty()) {
		return false;
	}
	FBReader::ViewMode mode = fbreader().getMode();
	return (mode == FBReader::BOOK_TEXT_MODE) || (mode == FBReader::FOOTNOTE_MODE);
}

void ShowContentsAction::run() {
	fbreader().setMode(FBReader::CONTENTS_MODE);
}

AddBookAction::AddBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool AddBookAction::isVisible() {
	return fbreader().getMode() != FBReader::FOOTNOTE_MODE;
}

void AddBookAction::run() {
	FBFileHandler handler;
	if (ZLDialogManager::instance().selectionDialog(ZLResourceKey("addFileDialog"), handler)) {
		BookDescriptionPtr description = handler.description();
		const std::string &fileName = description->fileName();
		if (!description.isNull() && fbreader().runBookInfoDialog(fileName)) {
			fbreader().openFile(fileName);
			fbreader().setMode(FBReader::BOOK_TEXT_MODE);
		}
	}
}

ScrollToHomeAction::ScrollToHomeAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

void ScrollToHomeAction::run() {
	fbreader().bookTextView().scrollToHome();
}

ScrollToStartOfTextAction::ScrollToStartOfTextAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

void ScrollToStartOfTextAction::run() {
	fbreader().bookTextView().scrollToStartOfText();
}

ScrollToEndOfTextAction::ScrollToEndOfTextAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

void ScrollToEndOfTextAction::run() {
	fbreader().bookTextView().scrollToEndOfText();
}

ShowBookInfoAction::ShowBookInfoAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE | FBReader::CONTENTS_MODE | FBReader::FOOTNOTE_MODE) {
}

void ShowBookInfoAction::run() {
	const std::string &fileName = fbreader().myModel->fileName();
	if (fbreader().runBookInfoDialog(fileName)) {
		fbreader().openFile(fileName);
	}
}

UndoAction::UndoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool UndoAction::isEnabled() {
	return (fbreader().getMode() != FBReader::BOOK_TEXT_MODE) ||
					fbreader().bookTextView().canUndoPageMove();
}

void UndoAction::run() {
	if (fbreader().getMode() == FBReader::BOOK_TEXT_MODE) {
		fbreader().bookTextView().undoPageMove();
	} else {
		fbreader().restorePreviousMode();
	}
}

RedoAction::RedoAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool RedoAction::isEnabled() {
	return isVisible() && fbreader().bookTextView().canRedoPageMove();
}

void RedoAction::run() {
	fbreader().bookTextView().redoPageMove();
}

ScrollingAction::ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward) : FBAction(fbreader), myOptions(options), myForward(forward) {
}

bool ScrollingAction::isEnabled() {
	return
		(&myOptions != &fbreader().TapScrollingOptions) ||
		fbreader().EnableTapScrollingOption.value();
}

bool ScrollingAction::useKeyDelay() const {
	return false;
}

void ScrollingAction::run() {
	int delay = fbreader().myLastScrollingTime.millisecondsTo(ZLTime());
	shared_ptr<ZLView> view = fbreader().currentView();
	if (!view.isNull() && ((delay < 0) || (delay >= myOptions.DelayOption.value()))) {
		ZLTextView::ScrollingMode oType = (ZLTextView::ScrollingMode)myOptions.ModeOption.value();
		unsigned int oValue = 0;
		switch (oType) {
			case ZLTextView::KEEP_LINES:
				oValue = myOptions.LinesToKeepOption.value();
				break;
			case ZLTextView::SCROLL_LINES:
				oValue = myOptions.LinesToScrollOption.value();
				break;
			case ZLTextView::SCROLL_PERCENTAGE:
				oValue = myOptions.PercentToScrollOption.value();
				break;
			default:
				break;
		}
		((ZLTextView&)*view).scrollPage(myForward, oType, oValue);
		fbreader().refreshWindow();
		fbreader().myLastScrollingTime = ZLTime();
	}
}

ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : FBAction(fbreader), myDelta(delta) {
}

void ChangeFontSizeAction::run() {
	ZLIntegerRangeOption &option = ZLTextStyleCollection::instance().baseStyle().FontSizeOption;
	option.setValue(option.value() + myDelta);
	fbreader().clearTextCaches();
	fbreader().refreshWindow();
}

OpenPreviousBookAction::OpenPreviousBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool OpenPreviousBookAction::isVisible() {
	if ((fbreader().getMode() != FBReader::BOOK_TEXT_MODE) && (fbreader().getMode() != FBReader::CONTENTS_MODE)) {
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
	if (fbreader().getMode() != FBReader::BOOK_TEXT_MODE) {
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
	ZLBooleanOption &option = FBView::commonIndicatorInfo().ShowOption;
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
	if (fbreader().getMode() != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoNextTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && ((int)contentsView.currentTextViewParagraph() < (int)model->paragraphsNumber() - 1);
}

void GotoNextTOCSectionAction::run() {
	ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();
	int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current + 1]));
	((ZLTextView&)*fbreader().myBookTextView).gotoParagraph(reference);
	fbreader().refreshWindow();
}

GotoPreviousTOCSectionAction::GotoPreviousTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoPreviousTOCSectionAction::isVisible() {
	if (fbreader().getMode() != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoPreviousTOCSectionAction::isEnabled() {
	const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.model();
	if (model.isNull()) {
		return false;
	}
	const ContentsModel &contentsModel = (const ContentsModel&)*model;
	int tocIndex = contentsView.currentTextViewParagraph(false);
	if (tocIndex > 0) {
		return true;
	}
	if (tocIndex == 0) {
		const ZLTextWordCursor &cursor = fbreader().bookTextView().startCursor();
		if (cursor.isNull()) {
			return false;
		}
		if (cursor.wordNumber() > 0) {
			return true;
		}
		return
			contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[tocIndex])) >
			(int)cursor.paragraphCursor().index();
	}
	return false;
}

void GotoPreviousTOCSectionAction::run() {
	ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
	size_t current = contentsView.currentTextViewParagraph(false);
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();

	int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current]));
	const ZLTextWordCursor &cursor = fbreader().bookTextView().startCursor();
	if (!cursor.isNull() &&
			(cursor.wordNumber() == 0) &&
			(reference == (int)cursor.paragraphCursor().index())) {
		reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current - 1]));
	}
	((ZLTextView&)*fbreader().myBookTextView).gotoParagraph(reference);
	fbreader().refreshWindow();
}

GotoPageNumber::GotoPageNumber(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool GotoPageNumber::isEnabled() {
	return ModeDependentAction::isEnabled() && (fbreader().bookTextView().pageNumber() > 1);
}

void GotoPageNumber::run() {
	shared_ptr<ZLDialog> gotoPageDialog = ZLDialogManager::instance().createDialog(ZLResourceKey("gotoPageDialog"));

	const int pageNumber = fbreader().bookTextView().pageNumber();
	ZLIntegerRangeOption pageNumberOption(ZLCategoryKey::CONFIG, "gotoPageDialog", "Number", 0, pageNumber, pageNumber);
	gotoPageDialog->addOption(ZLResourceKey("pageNumber"), new ZLSimpleSpinOptionEntry(pageNumberOption, 1));
	gotoPageDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	gotoPageDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (gotoPageDialog->run()) {
		gotoPageDialog->acceptValues();
		fbreader().bookTextView().gotoPage(pageNumberOption.value());
		fbreader().refreshWindow();
	}
}

SelectionAction::SelectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SelectionAction::isVisible() {
	return !fbreader().currentView().isNull();
}

bool SelectionAction::isEnabled() {
	return isVisible() && !textView().selectionModel().getText().empty();
}

ZLTextView &SelectionAction::textView() {
	return (ZLTextView&)*fbreader().currentView();
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
	return SelectionAction::isVisible() && fbreader().isDictionarySupported();
}

void OpenSelectedTextInDictionaryAction::run() {
	fbreader().openInDictionary(textView().selectionModel().getText());
}

ClearSelectionAction::ClearSelectionAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

void ClearSelectionAction::run() {
	textView().selectionModel().clear();
	fbreader().refreshWindow();
}
