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

#include <ZLTextView.h>
#include <ZLBlockTreeView.h>

#include "FBReader.h"
#include "ScrollingAction.h"

ScrollingAction::ScrollingAction(
	ZLTextAreaController::ScrollingMode textScrollingMode,
	ZLBlockTreeView::ScrollingMode blockScrollingMode,
	bool forward
) : myTextScrollingMode(textScrollingMode), myBlockScrollingMode(blockScrollingMode), myForward(forward) {
}

int ScrollingAction::scrollingDelay() const {
	return 0;
}

bool ScrollingAction::isEnabled() const {
	return true;
}

bool ScrollingAction::useKeyDelay() const {
	return false;
}

void ScrollingAction::run() {
	FBReader &fbreader = FBReader::Instance();
	shared_ptr<ZLView> view = fbreader.currentView();
	int delay = fbreader.myLastScrollingTime.millisecondsTo(ZLTime());
	if (view.isNull() ||
			(delay >= 0 && delay < scrollingDelay())) {
		return;
	}

	if (view->isInstanceOf(ZLTextView::TYPE_ID)) {
		((ZLTextView&)*view).scrollPage(myForward, myTextScrollingMode, textOptionValue());
		FBReader::Instance().refreshWindow();
	} else if (view->isInstanceOf(ZLBlockTreeView::TYPE_ID)) {
		((ZLBlockTreeView&)*view).scroll(myBlockScrollingMode, !myForward);
	}
	fbreader.myLastScrollingTime = ZLTime();
}

LineScrollingAction::LineScrollingAction(bool forward) : ScrollingAction(ZLTextAreaController::SCROLL_LINES, ZLBlockTreeView::ITEM, forward) {
}

int LineScrollingAction::scrollingDelay() const {
	return FBReader::Instance().KeyScrollingDelayOption.value();
}

size_t LineScrollingAction::textOptionValue() const {
	return FBReader::Instance().LinesToScrollOption.value();
}

PageScrollingAction::PageScrollingAction(bool forward) : ScrollingAction(ZLTextAreaController::KEEP_LINES, ZLBlockTreeView::PAGE, forward) {
}

int PageScrollingAction::scrollingDelay() const {
	return FBReader::Instance().KeyScrollingDelayOption.value();
}

size_t PageScrollingAction::textOptionValue() const {
	return FBReader::Instance().LinesToKeepOption.value();
}

MouseWheelScrollingAction::MouseWheelScrollingAction(bool forward) : ScrollingAction(ZLTextAreaController::SCROLL_LINES, ZLBlockTreeView::ITEM, forward) {
}

size_t MouseWheelScrollingAction::textOptionValue() const {
	return 1;
}

TapScrollingAction::TapScrollingAction(bool forward) : ScrollingAction(ZLTextAreaController::KEEP_LINES, ZLBlockTreeView::NONE, forward) {
}

size_t TapScrollingAction::textOptionValue() const {
	return FBReader::Instance().LinesToKeepOption.value();
}

bool TapScrollingAction::isEnabled() const {
	return FBReader::Instance().EnableTapScrollingOption.value();
}
