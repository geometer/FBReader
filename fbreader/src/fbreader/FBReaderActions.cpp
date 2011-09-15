/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLOptionsDialog.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <ZLibrary.h>

#include <ZLBlockTreeView.h>
#include <ZLTextView.h>
#include <ZLTextSelectionModel.h>

#include "FBReader.h"
#include "FBReaderActions.h"
#include "BookTextView.h"
#include "ContentsView.h"
#include "../optionsDialog/bookInfo/BookInfoDialog.h"
#include "../optionsDialogMobile/MobileOptionsDialog.h"
#include "../optionsDialog/library/LibraryOptionsDialog.h"
#include "../optionsDialog/network/NetworkOptionsDialog.h"
#include "../optionsDialog/system/SystemOptionsDialog.h"
#include "../optionsDialog/reading/ReadingOptionsDialog.h"
#include "../optionsDialog/lookAndFeel/OptionsPage.h"
#include "../optionsDialog/lookAndFeel/LookAndFeelOptionsDialog.h"

#include "../bookmodel/BookModel.h"
#include "../options/FBTextStyle.h"

#include "../database/booksdb/BooksDBUtil.h"
#include "../database/booksdb/BooksDB.h"
#include "../library/Library.h"
#include "../library/Book.h"

#include "../libraryTree/LibraryTreeNodes.h"
#include "../bookmodel/TOCTreeNodes.h"

ModeDependentAction::ModeDependentAction(int visibleInModes) : myVisibleInModes(visibleInModes) {
}

bool ModeDependentAction::isVisible() const {
	return (FBReader::Instance().mode() & myVisibleInModes) != 0;
}

SetModeAction::SetModeAction(FBReader::ViewMode modeToSet, int visibleInModes) : ModeDependentAction(visibleInModes), myModeToSet(modeToSet) {
}

void SetModeAction::run() {
	FBReader::Instance().setMode(myModeToSet);
}

void ShowHelpAction::run() {
	FBReader &fbreader = FBReader::Instance();
	shared_ptr<Book> book = BooksDBUtil::getBook(fbreader.helpFileName(ZLibrary::Language()));
	if (book.isNull()) {
		book = BooksDBUtil::getBook(fbreader.helpFileName("en"));
	}
	if (!book.isNull()) {
		fbreader.openBook(book);
		fbreader.setMode(FBReader::BOOK_TEXT_MODE);
		fbreader.refreshWindow();
	} else {
		ZLDialogManager::Instance().errorBox(ZLResourceKey("noHelpBox"));
	}
}

void ShowOptionsDialogAction::run() {
	std::string actionId = FBReader::Instance().LastOpenedPreferencesDialog.value();
	if (actionId.empty()) {
		return;
	}
	FBReader::Instance().doAction(actionId);
}

void ShowLibraryTreeAction::run() {
	shared_ptr<ZLTreeDialog> dialog = ZLDialogManager::Instance().createTreeDialog();
	size_t index = 0;
	dialog->rootNode().insert(new AuthorTreeNode, index++);
	dialog->rootNode().insert(new TagTreeNode, index++);
	dialog->rootNode().insert(new RecentBooksTreeNode, index++);
	dialog->run();
}

void ShowMobileOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_MOBILE_OPTIONS_DIALOG);
	MobileOptionsDialog().dialog().run();
}



void ShowLibraryOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_LIBRARY_OPTIONS_DIALOG);
	LibraryOptionsDialog().dialog().run();
}

void ShowNetworkOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_NETWORK_OPTIONS_DIALOG);
	NetworkOptionsDialog().dialog().run();
}

void ShowSystemOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_SYSTEM_OPTIONS_DIALOG);
	SystemOptionsDialog().dialog().run();
}

void ShowReadingOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_READING_OPTIONS_DIALOG);
	ReadingOptionsDialog().dialog().run();
}

void ShowLookAndFeelOptionsDialogAction::run() {
	FBReader::Instance().LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_LOOKANDFEEL_OPTIONS_DIALOG);
	LookAndFeelOptionsDialog().dialog().run();
}

ShowContentsAction::ShowContentsAction() : SetModeAction(FBReader::CONTENTS_MODE, FBReader::BOOK_TEXT_MODE) {
}

bool ShowContentsAction::isVisible() const {
	return ModeDependentAction::isVisible() && !((ContentsView&)*FBReader::Instance().myContentsView).isEmpty();
}

void ShowTOCTreeAction::run() {
	shared_ptr<ZLTreeDialog> dialog = ZLDialogManager::Instance().createTreeDialog();
	shared_ptr<ZLTextModel> contentsModel = FBReader::Instance().myModel->contentsModel();
	const ZLTextTreeParagraph& rootParagraph = ((ContentsModel&)*contentsModel).getRootParagraph();
	const std::vector<ZLTextTreeParagraph*>& children = rootParagraph.children();
	for (size_t index=0; index<children.size(); ++index) {
		const ZLTextTreeParagraph& paragraph = *children.at(index);
		if (paragraph.children().size() == 0) {
			dialog->rootNode().insert(new ReferenceNode(paragraph), index);
		} else {
			dialog->rootNode().insert(new ReferenceTreeNode(paragraph), index);
		}
	}
	dialog->run();
}

bool ShowTOCTreeAction::isVisible() const {
	shared_ptr<BookModel> model = FBReader::Instance().myModel;
	return !(model.isNull() || model->contentsModel().isNull() || model->contentsModel()->paragraphsNumber() == 0);
}


ScrollToHomeAction::ScrollToHomeAction() : ModeDependentAction(FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToHomeAction::isEnabled() const {
	if (!isVisible()) {
		return false;
	}
	ZLTextWordCursor cursor = FBReader::Instance().bookTextView().textArea().startCursor();
	return cursor.isNull() || !cursor.isStartOfParagraph() || !cursor.paragraphCursor().isFirst();
}

void ScrollToHomeAction::run() {
	FBReader::Instance().bookTextView().scrollToHome();
}

ScrollToStartOfTextAction::ScrollToStartOfTextAction() : ModeDependentAction(FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToStartOfTextAction::isEnabled() const {
	if (!isVisible()) {
		return false;
	}
	ZLTextWordCursor cursor = FBReader::Instance().bookTextView().textArea().startCursor();
	return cursor.isNull() || !cursor.isStartOfParagraph() || !cursor.paragraphCursor().isFirst();
}

void ScrollToStartOfTextAction::run() {
	FBReader::Instance().bookTextView().scrollToStartOfText();
}

ScrollToEndOfTextAction::ScrollToEndOfTextAction() : ModeDependentAction(FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToEndOfTextAction::isEnabled() const {
	if (!isVisible()) {
		return false;
	}
	ZLTextWordCursor cursor = FBReader::Instance().bookTextView().textArea().endCursor();
	return cursor.isNull() || !cursor.isEndOfParagraph() || !cursor.paragraphCursor().isLast();
}

void ScrollToEndOfTextAction::run() {
	FBReader::Instance().bookTextView().scrollToEndOfText();
}

ShowBookInfoAction::ShowBookInfoAction() : ModeDependentAction(FBReader::BOOK_TEXT_MODE | FBReader::CONTENTS_MODE | FBReader::FOOTNOTE_MODE) {
}

void ShowBookInfoAction::run() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.LastOpenedPreferencesDialog.setValue(ActionCode::SHOW_BOOK_INFO_DIALOG);
	shared_ptr<Book> book = fbreader.myModel->book();
	if (BookInfoDialog(book).dialog().run()) {
		fbreader.openBook(book);
		fbreader.refreshWindow();
	}
}

UndoAction::UndoAction(int visibleInModes) : ModeDependentAction(visibleInModes) {
}

bool UndoAction::isEnabled() const {
	FBReader &fbreader = FBReader::Instance();
	return (fbreader.mode() != FBReader::BOOK_TEXT_MODE) ||
					fbreader.bookTextView().canUndoPageMove();
}

void UndoAction::run() {
	FBReader &fbreader = FBReader::Instance();
	if (fbreader.mode() == FBReader::BOOK_TEXT_MODE) {
		fbreader.bookTextView().undoPageMove();
	} else {
		fbreader.restorePreviousMode();
	}
}

RedoAction::RedoAction() : ModeDependentAction(FBReader::BOOK_TEXT_MODE) {
}

bool RedoAction::isEnabled() const {
	return isVisible() && FBReader::Instance().bookTextView().canRedoPageMove();
}

void RedoAction::run() {
	FBReader::Instance().bookTextView().redoPageMove();
}

ChangeFontSizeAction::ChangeFontSizeAction(int delta) : myDelta(delta) {
}

bool ChangeFontSizeAction::isEnabled() const {
	ZLIntegerRangeOption &option = FBTextStyle::Instance().FontSizeOption;
	if (myDelta < 0) {
		return option.value() > option.minValue();
	} else {
		return option.value() < option.maxValue();
	}
}

void ChangeFontSizeAction::run() {
	FBReader &fbreader = FBReader::Instance();
	ZLIntegerRangeOption &option = FBTextStyle::Instance().FontSizeOption;
	option.setValue(option.value() + myDelta);
	fbreader.clearTextCaches();
	fbreader.refreshWindow();
}

bool OpenPreviousBookAction::isVisible() const {
	const FBReader &fbreader = FBReader::Instance();
	if ((fbreader.mode() != FBReader::BOOK_TEXT_MODE) &&
			(fbreader.mode() != FBReader::CONTENTS_MODE)) {
		return false;
	}
	return Library::Instance().recentBooks().size() > 1;
}

void OpenPreviousBookAction::run() {
	FBReader &fbreader = FBReader::Instance();
	const BookList &books = Library::Instance().recentBooks();
	fbreader.openBook(books[1]);
	fbreader.refreshWindow();
	fbreader.resetWindowCaption();
}

void CancelAction::run() {
	FBReader &fbreader = FBReader::Instance();
	switch (fbreader.myActionOnCancel) {
		case FBReader::UNFULLSCREEN:
			if (fbreader.isFullscreen()) {
				fbreader.setFullscreen(false);
				return;
			} else if (fbreader.mode() != FBReader::BOOK_TEXT_MODE) {
				fbreader.restorePreviousMode();
				return;
			}
			break;
		case FBReader::RETURN_TO_TEXT_MODE:
			if (fbreader.mode() != FBReader::BOOK_TEXT_MODE) {
				fbreader.restorePreviousMode();
				return;
			} else if (fbreader.isFullscreen()) {
				fbreader.setFullscreen(false);
				return;
			}
			break;
	}
	if (fbreader.QuitOnCancelOption.value()) {
		fbreader.quit();
	}
}

bool ToggleIndicatorAction::isVisible() const {
	ZLIntegerRangeOption &option = FBView::commonIndicatorInfo().TypeOption;
	switch (option.value()) {
		case FBIndicatorStyle::FB_INDICATOR:
		case FBIndicatorStyle::NONE:
			return true;
	}
	return false;
}

void ToggleIndicatorAction::run() {
	ZLIntegerRangeOption &option = FBView::commonIndicatorInfo().TypeOption;
	switch (option.value()) {
		case FBIndicatorStyle::OS_SCROLLBAR:
			break;
		case FBIndicatorStyle::FB_INDICATOR:
			option.setValue(FBIndicatorStyle::NONE);
			FBReader::Instance().refreshWindow();
			break;
		case FBIndicatorStyle::NONE:
			option.setValue(FBIndicatorStyle::FB_INDICATOR);
			FBReader::Instance().refreshWindow();
			break;
	}
}

void QuitAction::run() {
	FBReader::Instance().closeView();
}

void ForceQuitAction::run() {
	FBReader::Instance().quit();
}

bool GotoNextTOCSectionAction::isVisible() const {
	FBReader &fbreader = FBReader::Instance();
	if (fbreader.mode() != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader.myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.textArea().model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoNextTOCSectionAction::isEnabled() const {
	FBReader &fbreader = FBReader::Instance();
	const ContentsView &contentsView = (const ContentsView&)*fbreader.myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.textArea().model();
	return !model.isNull() && ((int)contentsView.currentTextViewParagraph() < (int)model->paragraphsNumber() - 1);
}

void GotoNextTOCSectionAction::run() {
	FBReader &fbreader = FBReader::Instance();
	ContentsView &contentsView = (ContentsView&)*fbreader.myContentsView;
	size_t current = contentsView.currentTextViewParagraph();
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.textArea().model();
	int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current + 1]));
	if (reference != -1) {
		((ZLTextView&)*fbreader.myBookTextView).gotoParagraph(reference);
		fbreader.refreshWindow();
	}
}

bool GotoPreviousTOCSectionAction::isVisible() const {
	const FBReader &fbreader = FBReader::Instance();
	if (fbreader.mode() != FBReader::BOOK_TEXT_MODE) {
		return false;
	}
	const ContentsView &contentsView = (const ContentsView&)*fbreader.myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.textArea().model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoPreviousTOCSectionAction::isEnabled() const {
	const FBReader &fbreader = FBReader::Instance();
	const ContentsView &contentsView = (const ContentsView&)*fbreader.myContentsView;
	shared_ptr<ZLTextModel> model = contentsView.textArea().model();
	if (model.isNull()) {
		return false;
	}
	const ContentsModel &contentsModel = (const ContentsModel&)*model;
	int tocIndex = contentsView.currentTextViewParagraph(false);
	if (tocIndex > 0) {
		return true;
	}
	if (tocIndex == 0) {
		const ZLTextWordCursor &cursor = fbreader.bookTextView().textArea().startCursor();
		if (cursor.isNull()) {
			return false;
		}
		if (cursor.elementIndex() > 0) {
			return true;
		}
		return
			contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[0])) >
			(int)cursor.paragraphCursor().index();
	}
	return false;
}

void GotoPreviousTOCSectionAction::run() {
	FBReader &fbreader = FBReader::Instance();
	ContentsView &contentsView = (ContentsView&)*fbreader.myContentsView;
	size_t current = contentsView.currentTextViewParagraph(false);
	const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.textArea().model();

	int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current]));
	const ZLTextWordCursor &cursor = fbreader.bookTextView().textArea().startCursor();
	if (!cursor.isNull() &&
			(cursor.elementIndex() == 0)) {
		int paragraphIndex = cursor.paragraphCursor().index();
		if (reference == paragraphIndex) {
			reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current - 1]));
		} else if (reference == paragraphIndex - 1) {
			const ZLTextModel &textModel = *fbreader.bookTextView().textArea().model();
			const ZLTextParagraph *para = textModel[paragraphIndex];
			if ((para != 0) && (para->kind() == ZLTextParagraph::END_OF_SECTION_PARAGRAPH)) {
				reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current - 1]));
			}
		}
	}
	if (reference != -1) {
		((ZLTextView&)*fbreader.myBookTextView).gotoParagraph(reference);
		fbreader.refreshWindow();
	}
}

GotoPageNumberAction::GotoPageNumberAction(const std::string &parameter) : ModeDependentAction(FBReader::BOOK_TEXT_MODE), myParameter(parameter) {
}

bool GotoPageNumberAction::isVisible() const {
	return
		ModeDependentAction::isVisible() &&
		!FBReader::Instance().bookTextView().hasMultiSectionModel();
}

bool GotoPageNumberAction::isEnabled() const {
	return ModeDependentAction::isEnabled() && (FBReader::Instance().bookTextView().pageNumber() > 1);
}

void GotoPageNumberAction::run() {
	FBReader &fbreader = FBReader::Instance();
	int pageIndex = 0;
	const int pageNumber = fbreader.bookTextView().pageNumber();

	if (!myParameter.empty()) {
		const std::string value = fbreader.visualParameter(myParameter);
		if (value.empty()) {
			return;
		}
		pageIndex = atoi(value.c_str());
	} else {
		shared_ptr<ZLDialog> gotoPageDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("gotoPageDialog"));

		ZLIntegerRangeOption pageIndexOption(ZLCategoryKey::CONFIG, "gotoPageDialog", "Index", 1, pageNumber, pageIndex);
		gotoPageDialog->addOption(ZLResourceKey("pageNumber"), new ZLSimpleSpinOptionEntry(pageIndexOption, 1));
		gotoPageDialog->addButton(ZLDialogManager::OK_BUTTON, true);
		gotoPageDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
		if (gotoPageDialog->run()) {
			gotoPageDialog->acceptValues();
			pageIndex = pageIndexOption.value();
		} else {
			return;
		}
	}

	fbreader.bookTextView().gotoPage(std::max(1, std::min(pageIndex, pageNumber)));
	fbreader.refreshWindow();
}

bool SelectionAction::isVisible() const {
	shared_ptr<ZLView> view = FBReader::Instance().currentView();
	return !view.isNull() && view->isInstanceOf(ZLTextView::TYPE_ID);
}

bool SelectionAction::isEnabled() const {
	if (!isVisible()) {
		return false;
	}
	const ZLTextSelectionModel &selectionModel = textView().selectionModel();
	return !selectionModel.text().empty() || !selectionModel.image().isNull();
}

ZLTextView &SelectionAction::textView() const {
	return (ZLTextView&)*FBReader::Instance().currentView();
}

bool CopySelectedTextAction::isVisible() const {
	return SelectionAction::isVisible() && ZLDialogManager::Instance().isClipboardSupported(ZLDialogManager::CLIPBOARD_MAIN);
}

void CopySelectedTextAction::run() {
	textView().selectionModel().copySelectionToClipboard(ZLDialogManager::CLIPBOARD_MAIN);
}

bool OpenSelectedTextInDictionaryAction::isVisible() const {
	return SelectionAction::isVisible() && FBReader::Instance().isDictionarySupported();
}

void OpenSelectedTextInDictionaryAction::run() {
	FBReader::Instance().openInDictionary(textView().selectionModel().text());
}

void ClearSelectionAction::run() {
	textView().selectionModel().clear();
	FBReader::Instance().refreshWindow();
}

void FBFullscreenAction::run() {
	FBReader &fbreader = FBReader::Instance();
	if (!fbreader.isFullscreen()) {
		fbreader.myActionOnCancel = FBReader::UNFULLSCREEN;
	}
	FullscreenAction::run();
}

FilterLibraryAction::FilterLibraryAction() : ModeDependentAction(FBReader::LIBRARY_MODE) {
}

void FilterLibraryAction::run() {
}
