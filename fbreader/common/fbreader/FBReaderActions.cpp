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

#include "../textview/TextView.h"

FBReader::ShowCollectionAction::ShowCollectionAction(FBReader &fbreader) : Action(fbreader) {
}

void FBReader::ShowCollectionAction::run() {
	((FBReader&)myApplication).setMode(BOOK_COLLECTION_MODE);
}

bool FBReader::ShowCollectionAction::isVisible() {
	ViewMode mode = ((FBReader&)myApplication).myMode;
	return (mode != FOOTNOTE_MODE) && (mode != BOOK_COLLECTION_MODE);
}

FBReader::ShowRecentBooksListAction::ShowRecentBooksListAction(FBReader &fbreader) : Action(fbreader) {
}

void FBReader::ShowRecentBooksListAction::run() {
	((FBReader&)myApplication).setMode(RECENT_BOOKS_MODE);
}

bool FBReader::ShowRecentBooksListAction::isVisible() {
	ViewMode mode = ((FBReader&)myApplication).myMode;
	return (mode != FOOTNOTE_MODE) && (mode != BOOK_COLLECTION_MODE) && (mode != RECENT_BOOKS_MODE);
}

FBReader::ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : Action(fbreader) {
}

FBReader::UndoAction::UndoAction(FBReader &fbreader) : Action(fbreader) {
}

bool FBReader::UndoAction::isEnabled() {
  return (((FBReader&)myApplication).myMode != BOOK_TEXT_MODE) ||
					((FBReader&)myApplication).myBookTextView->canUndoPageMove();
}

void FBReader::UndoAction::run() {
  if (((FBReader&)myApplication).myMode == BOOK_TEXT_MODE) {
    ((FBReader&)myApplication).myBookTextView->undoPageMove();
  } else {
    ((FBReader&)myApplication).restorePreviousMode();
  }
}

FBReader::RedoAction::RedoAction(FBReader &fbreader) : Action(fbreader) {
}

bool FBReader::RedoAction::isVisible() {
	return ((FBReader&)myApplication).myMode == BOOK_TEXT_MODE;
}

bool FBReader::RedoAction::isEnabled() {
  return isVisible() && ((FBReader&)myApplication).myBookTextView->canRedoPageMove();
}

void FBReader::RedoAction::run() {
	((FBReader&)myApplication).myBookTextView->redoPageMove();
}

void FBReader::ShowOptionsDialogAction::run() {
	((FBReader&)myApplication).optionsSlot();
}

FBReader::SearchAction::SearchAction(FBReader &fbreader) : Action(fbreader) {
}

void FBReader::SearchAction::run() {
	((FBReader&)myApplication).searchSlot();
}

FBReader::FindNextAction::FindNextAction(FBReader &fbreader) : Action(fbreader) {
}

bool FBReader::FindNextAction::isEnabled() {
	TextView *textView = (TextView*)((FBReader&)myApplication).myViewWidget->view();
	return (textView != 0) && textView->canFindNext();
}

void FBReader::FindNextAction::run() {
	((TextView*)((FBReader&)myApplication).myViewWidget->view())->findNext();
}

FBReader::FindPreviousAction::FindPreviousAction(FBReader &fbreader) : Action(fbreader) {
}

bool FBReader::FindPreviousAction::isEnabled() {
	TextView *textView = (TextView*)((FBReader&)myApplication).myViewWidget->view();
	return (textView != 0) && textView->canFindPrevious();
}

void FBReader::FindPreviousAction::run() {
	((TextView*)((FBReader&)myApplication).myViewWidget->view())->findPrevious();
}

FBReader::ScrollingAction::ScrollingAction(FBReader &fbreader, const ScrollingOptions &options, bool forward) : Action(fbreader), myOptions(options), myForward(forward) {
}

void FBReader::ScrollingAction::run() {
  int delay = ((FBReader&)myApplication).myLastScrollingTime.millisecondsTo(ZLTime());
	TextView *textView = (TextView*)((FBReader&)myApplication).myViewWidget->view();
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
    ((TextView*)((FBReader&)myApplication).myViewWidget->view())->scrollPage(myForward, oType, oValue);
    ((FBReader&)myApplication).repaintView();
    ((FBReader&)myApplication).myLastScrollingTime = ZLTime();
  }
}

FBReader::ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : Action(fbreader), myDelta(delta) {
}

void FBReader::ChangeFontSizeAction::run() {
	ZLIntegerRangeOption &option = TextStyleCollection::instance().baseStyle().FontSizeOption;
	option.setValue(option.value() + myDelta);
	((FBReader&)myApplication).clearTextCaches();
	((FBReader&)myApplication).repaintView();
}
