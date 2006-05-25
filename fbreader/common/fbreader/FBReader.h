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
	KeyBindings();
	~KeyBindings();

	void bindKey(const std::string &key, ActionCode code);
	ActionCode getBinding(const std::string &key);

private:
	std::map<std::string,ActionCode> myBindingsMap;
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
		) FB_SECTION;
		
		ZLIntegerRangeOption DelayOption;
		ZLIntegerOption ModeOption;
		ZLIntegerRangeOption LinesToKeepOption;
		ZLIntegerRangeOption LinesToScrollOption;
		ZLIntegerRangeOption PercentToScrollOption;
	};

public:
	static ZLBooleanOption QuitOnCancelOption;
	static ZLBooleanOption StoreContentsPositionOption;
	static ZLIntegerRangeOption KeyDelayOption;

	static ScrollingOptions LargeScrollingOptions;
	static ScrollingOptions SmallScrollingOptions;
	static ScrollingOptions MouseScrollingOptions;

	static ZLBooleanOption SearchBackwardOption;
	static ZLBooleanOption SearchIgnoreCaseOption;
	static ZLBooleanOption SearchInWholeTextOption;
	static ZLBooleanOption SearchThisSectionOnlyOption;
	static ZLStringOption SearchPatternOption;

	static ZLBooleanOption KeyboardControlOption;

	static ZLIntegerOption RotationAngleOption;
	static ZLIntegerOption AngleStateOption;

protected:
	FBReader(ZLPaintContext *context, const std::string& bookToOpen = std::string()) FB_SECTION;
	virtual ~FBReader() FB_SECTION;

	virtual void setMode(ViewMode mode) FB_SECTION;
	virtual void setWindowCaption(const std::string &caption) FB_SECTION = 0;
	void createToolbar() FB_SECTION;
	virtual void addButton(ActionCode id, const std::string &name) FB_SECTION = 0;
	virtual void addButtonSeparator() FB_SECTION = 0;
	virtual void setButtonVisible(ActionCode id, bool visible) FB_SECTION = 0;
	virtual void setButtonEnabled(ActionCode id, bool enable) FB_SECTION = 0;
	virtual void searchSlot() FB_SECTION = 0;
	void cancelSlot() FB_SECTION;
	void fullscreenOnSlot() FB_SECTION;
	virtual void toggleFullscreenSlot() FB_SECTION = 0;
	virtual bool isFullscreen() const FB_SECTION = 0;
	virtual void quitSlot() FB_SECTION = 0;

	virtual void bookInfoSlot() FB_SECTION;
	virtual void optionsSlot() FB_SECTION;
	virtual void addBookSlot() FB_SECTION;

	void doAction(const std::string &key) FB_SECTION;

protected:
	BookDescriptionPtr createDescription(const std::string& fileName) const FB_SECTION;
	void resetWindowCaption();

private:
	bool runBookInfoDialog(const std::string &fileName) FB_SECTION;
	void clearTextCaches() FB_SECTION;
	void doScrolling(const ScrollingOptions &options, bool forward) FB_SECTION;

	bool isScrollingAction(ActionCode code) FB_SECTION;

public:
	virtual bool isRotationSupported() const FB_SECTION = 0;
	virtual bool isFullKeyboardControlSupported() const FB_SECTION;
	virtual void grabAllKeys(bool grab) FB_SECTION;

	void openBook(BookDescriptionPtr description) FB_SECTION;
	BookTextView &textView() const FB_SECTION;
	void showBookTextView() FB_SECTION;
	void tryShowFootnoteView(const std::string &id) FB_SECTION;
	void restorePreviousMode() FB_SECTION;
	virtual void enableMenuButtons() FB_SECTION;
	void repaintView() FB_SECTION;
	void doAction(ActionCode code) FB_SECTION;

	KeyBindings &keyBindings();

private:
	void openBookInternal(BookDescriptionPtr description) FB_SECTION;
	friend class OpenBookRunnable;
	void rebuildCollectionInternal() FB_SECTION;
	friend class RebuildCollectionRunnable;

protected:
	ViewMode myMode;
	ViewMode myPreviousMode;
	ZLViewWidget *myViewWidget;

private:
	FootnoteView *myFootnoteView;	
	BookTextView *myBookTextView;	
	ContentsView *myContentsView;	
	CollectionView *myCollectionView;	
	RecentBooksView *myRecentBooksView;	

	ZLTime myLastScrollingTime;
	ZLTime myLastKeyActionTime;

	ZLPaintContext *myContext;
	BookModel *myModel;

	KeyBindings myKeyBindings;
};

inline KeyBindings &FBReader::keyBindings() {
	return myKeyBindings;
}

#endif /* __FBREADER_H__ */
