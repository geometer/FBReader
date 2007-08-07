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

#ifndef __FBREADER_H__
#define __FBREADER_H__

#include <string>
#include <map>

#include <ZLOptions.h>
#include <ZLTime.h>
#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

#include "../description/BookDescription.h"
#include "../external/ProgramCollection.h"

class ZLViewWidget;
class ZLMessageHandler;

class BookModel;
class BookTextView;
class FootnoteView;
class ContentsView;
class CollectionView;
class RecentBooksView;

class FBReader : public ZLApplication {

protected:
	enum ViewMode {
		UNDEFINED_MODE,
		BOOK_TEXT_MODE,
		FOOTNOTE_MODE,
		CONTENTS_MODE,
		BOOKMARKS_MODE,
		BOOK_COLLECTION_MODE,
		RECENT_BOOKS_MODE,
	};

public:
	struct ScrollingOptions {
		ScrollingOptions(
			const std::string &delayGroup, const std::string &delayName, long delayValue,
			const std::string &modeGroup, const std::string &modeName, long modeValue,
			const std::string &linesToKeepGroup, const std::string &linesToKeepName, long linesToKeepValue,
			const std::string &linesToScrollGroup, const std::string &linesToScrollName, long linesToScrollValue,
			const std::string &percentToScrollGroup, const std::string &percentToScrollName, long percentToScrollValue
		);
		
		ZLIntegerRangeOption DelayOption;
		ZLIntegerOption ModeOption;
		ZLIntegerRangeOption LinesToKeepOption;
		ZLIntegerRangeOption LinesToScrollOption;
		ZLIntegerRangeOption PercentToScrollOption;
	};

public:
	ZLBooleanOption QuitOnCancelOption;

	ScrollingOptions LargeScrollingOptions;
	ScrollingOptions SmallScrollingOptions;
	ScrollingOptions MouseScrollingOptions;
	ScrollingOptions FingerTapScrollingOptions;
	ZLBooleanOption EnableFingerScrollingOption;

	ZLBooleanOption SearchBackwardOption;
	ZLBooleanOption SearchIgnoreCaseOption;
	ZLBooleanOption SearchInWholeTextOption;
	ZLBooleanOption SearchThisSectionOnlyOption;
	ZLStringOption SearchPatternOption;

	ZLBooleanOption UseSeparateBindingsOption;
	ZLBooleanOption ShowHelpIconOption;

	ZLBooleanOption EnableSingleClickDictionaryOption;

public:
	FBReader(const std::string &bookToOpen);
	~FBReader();

private:
	void initWindow();

	void setMode(ViewMode mode);

	void searchSlot();

	BookDescriptionPtr createDescription(const std::string &fileName) const;

	bool runBookInfoDialog(const std::string &fileName);

	void clearTextCaches();

	void restorePreviousMode();

	bool closeView();
	std::string helpFileName(const std::string &language) const;
	void openFile(const std::string &fileName);

public:
	ZLKeyBindings &keyBindings();
	ZLKeyBindings &keyBindings(ZLViewWidget::Angle angle);

	bool isDictionarySupported() const;
	void openInDictionary(const std::string &word);

	shared_ptr<ProgramCollection> webBrowserCollection() const;

	void tryShowFootnoteView(const std::string &id, bool external);
	BookTextView &bookTextView() const;
	void showBookTextView();
	void openBook(BookDescriptionPtr description);

private:
	shared_ptr<ProgramCollection> dictionaryCollection() const;

	void openBookInternal(BookDescriptionPtr description);
	friend class OpenBookRunnable;
	void rebuildCollectionInternal();
	friend class RebuildCollectionRunnable;
	friend class OptionsApplyRunnable;

private:
	ViewMode myMode;
	ViewMode myPreviousMode;

	shared_ptr<ZLView> myFootnoteView;	
	shared_ptr<ZLView> myBookTextView;	
	shared_ptr<ZLView> myContentsView;	
	shared_ptr<ZLView> myCollectionView;	
	shared_ptr<ZLView> myRecentBooksView;	

	ZLTime myLastScrollingTime;

	BookModel *myModel;

	ZLKeyBindings myBindings0;
	ZLKeyBindings myBindings90;
	ZLKeyBindings myBindings180;
	ZLKeyBindings myBindings270;

	std::string myBookToOpen;
	bool myBookAlreadyOpen;

	ProgramCollectionMap myProgramCollectionMap;

	shared_ptr<ZLMessageHandler> myOpenFileHandler;

friend class OpenFileHandler;

friend class OptionsDialog;
friend class FBView;

friend class ShowCollectionAction;
friend class ShowHelpAction;
friend class ShowRecentBooksListAction;
//friend class ShowOptionsDialogAction;
friend class ShowContentsAction;
friend class AddBookAction;
friend class ShowBookInfoAction;
friend class ScrollToHomeAction;
friend class ScrollToStartOfTextAction;
friend class ScrollToEndOfTextAction;
friend class UndoAction;
friend class RedoAction;
friend class SearchAction;
friend class FindNextAction;
friend class FindPreviousAction;
friend class ScrollingAction;
friend class ChangeFontSizeAction;
friend class CancelAction;
//friend class ToggleIndicatorAction;
friend class QuitAction;
friend class OpenPreviousBookAction;
friend class GotoNextTOCSectionAction;
friend class GotoPreviousTOCSectionAction;
friend class SelectionAction;
};

#endif /* __FBREADER_H__ */
