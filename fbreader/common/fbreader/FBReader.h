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

#ifndef __FBREADER_H__
#define __FBREADER_H__

#include <string>
#include <map>

#include <abstract/ZLOptions.h>
#include <abstract/ZLTime.h>
#include <abstract/ZLView.h>
#include <abstract/ZLApplication.h>

#include "../description/BookDescription.h"

class BookModel;
class BookTextView;
class FootnoteView;
class ContentsView;
class CollectionView;
class RecentBooksView;
class ZLViewWidget;
class ZLPaintContext;

enum ActionCode {
	// please, don't change these numbers
	// add new action id's at end of this enumeration
	NO_ACTION = 0,
	ACTION_SHOW_COLLECTION = 1,
	ACTION_SHOW_OPTIONS = 2,
	ACTION_UNDO = 3,
	ACTION_REDO = 4,
	ACTION_SHOW_CONTENTS = 5,
	ACTION_SEARCH = 6,
	ACTION_FIND_PREVIOUS = 7,
	ACTION_FIND_NEXT = 8,
	ACTION_LARGE_SCROLL_FORWARD = 9,
	ACTION_LARGE_SCROLL_BACKWARD = 10,
	ACTION_SMALL_SCROLL_FORWARD = 11,
	ACTION_SMALL_SCROLL_BACKWARD = 12,
	ACTION_MOUSE_SCROLL_FORWARD = 13,
	ACTION_MOUSE_SCROLL_BACKWARD = 14,
	ACTION_SCROLL_TO_HOME = 15,
	ACTION_SCROLL_TO_START_OF_TEXT = 16,
	ACTION_SCROLL_TO_END_OF_TEXT = 17,
	ACTION_CANCEL = 18,
	ACTION_INCREASE_FONT = 19,
	ACTION_DECREASE_FONT = 20,
	ACTION_SHOW_HIDE_POSITION_INDICATOR = 21,
	ACTION_TOGGLE_FULLSCREEN = 22,
	ACTION_FULLSCREEN_ON = 23,
	ACTION_ADD_BOOK = 24,
	ACTION_SHOW_BOOK_INFO = 25,
	ACTION_SHOW_HELP = 26,
	ACTION_ROTATE_SCREEN = 27,
	ACTION_SHOW_LAST_BOOKS = 28,
	ACTION_QUIT = 29,
	ACTION_OPEN_PREVIOUS_BOOK = 30,
};

class KeyBindings {

public:
	KeyBindings(const std::string &optionGroupName);
	~KeyBindings();

	void bindKey(const std::string &key, ActionCode code);
	ActionCode getBinding(const std::string &key);

private:
	void readDefaultBindings();
	void readCustomBindings();

private:
	const std::string myOptionGroupName;
	std::map<std::string,ActionCode> myBindingsMap;

	friend class FullKeyBindings;
};

class FullKeyBindings {

public:
	ZLBooleanOption UseSeparateBindingsOption;

public:
	FullKeyBindings();
	KeyBindings &getBindings(ZLViewWidget::Angle angle, bool force);

private:
	KeyBindings myBindings0;
	KeyBindings myBindings90;
	KeyBindings myBindings180;
	KeyBindings myBindings270;
};

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
	ZLBooleanOption StoreContentsPositionOption;
	ZLIntegerRangeOption KeyDelayOption;

	ScrollingOptions LargeScrollingOptions;
	ScrollingOptions SmallScrollingOptions;
	ScrollingOptions MouseScrollingOptions;

	ZLBooleanOption SearchBackwardOption;
	ZLBooleanOption SearchIgnoreCaseOption;
	ZLBooleanOption SearchInWholeTextOption;
	ZLBooleanOption SearchThisSectionOnlyOption;
	ZLStringOption SearchPatternOption;

private:
	class FBAction : public Action {

	protected:
		FBAction(FBReader &fbreader);

	protected:
		FBReader &myFBReader;
	};

	class ShowCollectionAction : public FBAction {

	public:
		ShowCollectionAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ShowCollectionAction;

	class ShowRecentBooksListAction : public FBAction {

	public:
		ShowRecentBooksListAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ShowRecentBooksListAction;

	class ShowOptionsDialogAction : public FBAction {

	public:
		ShowOptionsDialogAction(FBReader &fbreader);
		void run();
	};
	friend class ShowOptionsDialogAction;

	class ShowContentsAction : public FBAction {

	public:
		ShowContentsAction(FBReader &fbreader);
		bool isVisible();
		bool isEnabled();
		void run();
	};
	friend class ShowContentsAction;

	class AddBookAction : public FBAction {

	public:
		AddBookAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class AddBookAction;

	class ShowBookInfoAction : public FBAction {

	public:
		ShowBookInfoAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ShowBookInfoAction;

	class ScrollToHomeAction : public FBAction {

	public:
		ScrollToHomeAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ScrollToHomeAction;

	class ScrollToStartOfTextAction : public FBAction {

	public:
		ScrollToStartOfTextAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ScrollToStartOfTextAction;

	class ScrollToEndOfTextAction : public FBAction {

	public:
		ScrollToEndOfTextAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class ScrollToEndOfTextAction;

	class UndoAction : public FBAction {

	public:
		UndoAction(FBReader &fbreader);
		bool isEnabled();
		void run();
	};
	friend class UndoAction;
	
	class RedoAction : public FBAction {

	public:
		RedoAction(FBReader &fbreader);
		bool isVisible();
		bool isEnabled();
		void run();
	};
	friend class RedoAction;
	
	class SearchAction : public FBAction {

	public:
		SearchAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class SearchAction;
	
	class FindNextAction : public FBAction {

	public:
		FindNextAction(FBReader &fbreader);
		bool isEnabled();
		void run();
	};
	friend class FindNextAction;
	
	class FindPreviousAction : public FBAction {

	public:
		FindPreviousAction(FBReader &fbreader);
		bool isEnabled();
		void run();
	};
	friend class FindPreviousAction;
	
	class ScrollingAction : public FBAction {

	public:
		ScrollingAction(FBReader &fbreader, const ScrollingOptions &options, bool forward);
		void run();

	private:
		const ScrollingOptions &myOptions;
		const bool myForward;
	};
	friend class ScrollingAction;
	
	class ChangeFontSizeAction : public FBAction {

	public:
		ChangeFontSizeAction(FBReader &fbreader, int delta);
		void run();

	private:
		const int myDelta;
	};
	friend class ChangeFontSizeAction;
	
	class CancelAction : public FBAction {

	public:
		CancelAction(FBReader &fbreader);
		void run();
	};
	friend class CancelAction;
	
	class ToggleIndicatorAction : public FBAction {

	public:
		ToggleIndicatorAction(FBReader &fbreader);
		void run();
	};
	friend class ToggleIndicatorAction;
	
	class QuitAction : public FBAction {

	public:
		QuitAction(FBReader &fbreader);
		void run();
	};
	friend class QuitAction;
	
	class OpenPreviousBookAction : public FBAction {

	public:
		OpenPreviousBookAction(FBReader &fbreader);
		bool isVisible();
		void run();
	};
	friend class OpenPreviousBookAction;
	
public:
	FBReader(ZLPaintContext *context, const std::string& bookToOpen);
	~FBReader();
	void initWindow();

private:
	void setMode(ViewMode mode);

	void searchSlot();
	void bookInfoSlot();
	void optionsSlot();
	void addBookSlot();

	void doActionByKey(const std::string &key);

	BookDescriptionPtr createDescription(const std::string& fileName) const;

	bool runBookInfoDialog(const std::string &fileName);
	void clearTextCaches();

	bool isScrollingAction(ActionCode code);

	void restorePreviousMode();

	bool closeView();
	void openFile(const std::string &fileName);

public:
	ZLBooleanOption &useSeparateBindings();
	KeyBindings &keyBindings(ZLViewWidget::Angle angle, bool force = false);

	void tryShowFootnoteView(const std::string &id);
	BookTextView &bookTextView() const;
	void showBookTextView();
	void openBook(BookDescriptionPtr description);

private:
	void openBookInternal(BookDescriptionPtr description);
	friend class OpenBookRunnable;
	void rebuildCollectionInternal();
	friend class RebuildCollectionRunnable;

	ViewMode myMode;
	ViewMode myPreviousMode;

	FootnoteView *myFootnoteView;	
	BookTextView *myBookTextView;	
	ContentsView *myContentsView;	
	CollectionView *myCollectionView;	
	RecentBooksView *myRecentBooksView;	

	ZLTime myLastScrollingTime;
	ZLTime myLastKeyActionTime;

	ZLPaintContext *myContext;
	BookModel *myModel;

	FullKeyBindings myKeyBindings;

friend class OptionsDialog;
};

inline ZLBooleanOption &FBReader::useSeparateBindings() {
	return myKeyBindings.UseSeparateBindingsOption;
}

inline KeyBindings &FBReader::keyBindings(ZLViewWidget::Angle angle, bool force) {
	return myKeyBindings.getBindings(angle, force);
}

#endif /* __FBREADER_H__ */
