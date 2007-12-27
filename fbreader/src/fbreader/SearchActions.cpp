/*
 * Copyright (C) 2004-2007 Geometer Plus <contact@geometerplus.com>
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
#include <ZLStringUtil.h>

#include <ZLTextView.h>

#include "FBReader.h"
#include "FBReaderActions.h"

SearchAction::SearchAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SearchAction::isVisible() {
	return !fbreader().currentView().isNull();
}

void SearchAction::run() {
	ZLTextView &textView = (ZLTextView&)*fbreader().currentView();

	shared_ptr<ZLDialog> searchDialog = ZLDialogManager::instance().createDialog(ZLResourceKey("textSearchDialog"));

	searchDialog->addOption(ZLResourceKey("text"), fbreader().SearchPatternOption);
	searchDialog->addOption(ZLResourceKey("ignoreCase"), fbreader().SearchIgnoreCaseOption);
	searchDialog->addOption(ZLResourceKey("wholeText"), fbreader().SearchInWholeTextOption);
	searchDialog->addOption(ZLResourceKey("backward"), fbreader().SearchBackwardOption);
	if (textView.hasMultiSectionModel()) {
		searchDialog->addOption(ZLResourceKey("currentSection"), fbreader().SearchThisSectionOnlyOption);
	}
	searchDialog->addButton(ZLResourceKey("go"), true);
	searchDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (searchDialog->run()) {
		searchDialog->acceptValues();
		std::string pattern = fbreader().SearchPatternOption.value();
		ZLStringUtil::stripWhiteSpaces(pattern);
		fbreader().SearchPatternOption.setValue(pattern);
		textView.search(
			pattern,
			fbreader().SearchIgnoreCaseOption.value(),
			fbreader().SearchInWholeTextOption.value(),
			fbreader().SearchBackwardOption.value(),
			fbreader().SearchThisSectionOnlyOption.value()
		);
	}
}

FindNextAction::FindNextAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindNextAction::isEnabled() {
	shared_ptr<ZLView> view = fbreader().currentView();
	return (!view.isNull()) && ((ZLTextView&)*view).canFindNext();
}

void FindNextAction::run() {
	((ZLTextView&)*fbreader().currentView()).findNext();
}

FindPreviousAction::FindPreviousAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool FindPreviousAction::isEnabled() {
	shared_ptr<ZLView> view = fbreader().currentView();
	return (!view.isNull()) && ((ZLTextView&)*view).canFindPrevious();
}

void FindPreviousAction::run() {
	((ZLTextView&)*fbreader().currentView()).findPrevious();
}
