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

#include "FBReader.h"
#include "BookTextView.h"
#include "ContentsView.h"

#include "../textview/TextView.h"

FBReader::FBAction::FBAction(FBReader &fbreader) : myFBReader(fbreader) {
}

FBReader::ShowCollectionAction::ShowCollectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

void FBReader::ShowCollectionAction::run() {
	myFBReader.setMode(BOOK_COLLECTION_MODE);
}

bool FBReader::ShowCollectionAction::isVisible() {
	ViewMode mode = myFBReader.myMode;
	return (mode != FOOTNOTE_MODE) && (mode != BOOK_COLLECTION_MODE);
}

FBReader::ShowRecentBooksListAction::ShowRecentBooksListAction(FBReader &fbreader) : FBAction(fbreader) {
}

void FBReader::ShowRecentBooksListAction::run() {
	myFBReader.setMode(RECENT_BOOKS_MODE);
}

bool FBReader::ShowRecentBooksListAction::isVisible() {
	ViewMode mode = myFBReader.myMode;
	return (mode != FOOTNOTE_MODE) && (mode != BOOK_COLLECTION_MODE) && (mode != RECENT_BOOKS_MODE);
}

FBReader::ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : FBAction(fbreader) {
}

FBReader::ShowContentsAction::ShowContentsAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::ShowContentsAction::isVisible() {
	ViewMode mode = myFBReader.myMode;
	return (mode == BOOK_TEXT_MODE) || (mode == FOOTNOTE_MODE);
}

bool FBReader::ShowContentsAction::isEnabled() {
	return isVisible() && !myFBReader.myContentsView->isEmpty();
}

void FBReader::ShowContentsAction::run() {
	myFBReader.setMode(CONTENTS_MODE);
}

FBReader::AddBookAction::AddBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::AddBookAction::isVisible() {
	return myFBReader.myMode != FOOTNOTE_MODE;
}

void FBReader::AddBookAction::run() {
	myFBReader.addBookSlot();
}

FBReader::ScrollToHomeAction::ScrollToHomeAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::ScrollToHomeAction::isVisible() {
	return myFBReader.myMode == BOOK_TEXT_MODE;
}

void FBReader::ScrollToHomeAction::run() {
	myFBReader.myBookTextView->scrollToHome();
}

FBReader::ShowBookInfoAction::ShowBookInfoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::ShowBookInfoAction::isVisible() {
	return
		(myFBReader.myMode == BOOK_TEXT_MODE) ||
		(myFBReader.myMode == CONTENTS_MODE) ||
		(myFBReader.myMode == FOOTNOTE_MODE);
}

void FBReader::ShowBookInfoAction::run() {
	myFBReader.bookInfoSlot();
}

FBReader::UndoAction::UndoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::UndoAction::isEnabled() {
	return (myFBReader.myMode != BOOK_TEXT_MODE) ||
					myFBReader.myBookTextView->canUndoPageMove();
}

void FBReader::UndoAction::run() {
	if (myFBReader.myMode == BOOK_TEXT_MODE) {
		myFBReader.myBookTextView->undoPageMove();
	} else {
		myFBReader.restorePreviousMode();
	}
}

FBReader::RedoAction::RedoAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::RedoAction::isVisible() {
	return myFBReader.myMode == BOOK_TEXT_MODE;
}

bool FBReader::RedoAction::isEnabled() {
	return isVisible() && myFBReader.myBookTextView->canRedoPageMove();
}

void FBReader::RedoAction::run() {
	myFBReader.myBookTextView->redoPageMove();
}

void FBReader::ShowOptionsDialogAction::run() {
	myFBReader.optionsSlot();
}

FBReader::SearchAction::SearchAction(FBReader &fbreader) : FBAction(fbreader) {
}

void FBReader::SearchAction::run() {
	myFBReader.searchSlot();
}

FBReader::FindNextAction::FindNextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::FindNextAction::isEnabled() {
	TextView *textView = (TextView*)myFBReader.myViewWidget->view();
	return (textView != 0) && textView->canFindNext();
}

void FBReader::FindNextAction::run() {
	((TextView*)myFBReader.myViewWidget->view())->findNext();
}

FBReader::FindPreviousAction::FindPreviousAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::FindPreviousAction::isEnabled() {
	TextView *textView = (TextView*)myFBReader.myViewWidget->view();
	return (textView != 0) && textView->canFindPrevious();
}

void FBReader::FindPreviousAction::run() {
	((TextView*)myFBReader.myViewWidget->view())->findPrevious();
}

FBReader::ScrollingAction::ScrollingAction(FBReader &fbreader, const ScrollingOptions &options, bool forward) : FBAction(fbreader), myOptions(options), myForward(forward) {
}

void FBReader::ScrollingAction::run() {
	int delay = myFBReader.myLastScrollingTime.millisecondsTo(ZLTime());
	TextView *textView = (TextView*)myFBReader.myViewWidget->view();
	if ((textView != 0) && ((delay < 0) || (delay >= myOptions.DelayOption.value()))) {
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
		((TextView*)myFBReader.myViewWidget->view())->scrollPage(myForward, oType, oValue);
		myFBReader.repaintView();
		myFBReader.myLastScrollingTime = ZLTime();
	}
}

FBReader::ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : FBAction(fbreader), myDelta(delta) {
}

void FBReader::ChangeFontSizeAction::run() {
	ZLIntegerRangeOption &option = TextStyleCollection::instance().baseStyle().FontSizeOption;
	option.setValue(option.value() + myDelta);
	myFBReader.clearTextCaches();
	myFBReader.repaintView();
}

FBReader::RotationAction::RotationAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FBReader::RotationAction::isVisible() {
	return myFBReader.isRotationSupported() &&
				 ((myFBReader.RotationAngleOption.value() != ZLViewWidget::DEGREES0) ||
					(myFBReader.myViewWidget->rotation() != ZLViewWidget::DEGREES0));
}

void FBReader::RotationAction::run() {
	int optionValue = myFBReader.RotationAngleOption.value();
	ZLViewWidget::Angle oldAngle = myFBReader.myViewWidget->rotation();
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
	myFBReader.myViewWidget->rotate(newAngle);
	myFBReader.AngleStateOption.setValue(newAngle);
	myFBReader.repaintView();
}
