/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __FBREADER_H__
#define __FBREADER_H__

#include <string>

#include <abstract/ZLOptions.h>
#include <abstract/ZLTime.h>
#include <abstract/ZLView.h>

#include "../description/BookDescription.h"

class BookModel;
class BookTextView;
class FootnoteView;
class ContentsView;
class CollectionView;
class ZLViewWidget;
class ZLPaintContext;

class FBReader : public ZLApplication {

public:
	enum ActionCode {
		ACTION_SHOW_COLLECTION,
		ACTION_SHOW_OPTIONS,
		ACTION_UNDO,
		ACTION_REDO,
		ACTION_SHOW_CONTENTS,
		ACTION_SEARCH,
		ACTION_FIND_PREVIOUS,
		ACTION_FIND_NEXT,
		ACTION_LARGE_SCROLL_FORWARD,
		ACTION_LARGE_SCROLL_BACKWARD,
		ACTION_SMALL_SCROLL_FORWARD,
		ACTION_SMALL_SCROLL_BACKWARD,
		ACTION_MOUSE_SCROLL_FORWARD,
		ACTION_MOUSE_SCROLL_BACKWARD,
		ACTION_SCROLL_TO_START_OF_TEXT,
		ACTION_SCROLL_TO_END_OF_TEXT,
		ACTION_CANCEL,
		ACTION_INCREASE_FONT,
		ACTION_DECREASE_FONT,
		ACTION_SHOW_HIDE_POSITION_INDICATOR,
		ACTION_FULLSCREEN,
		ACTION_ADD_BOOK,
		ACTION_SHOW_BOOK_INFO,
		ACTION_SHOW_HELP,
		ACTION_ROTATE_SCREEN,
		ACTION_SHOW_LAST_BOOKS,
		ACTION_QUIT,
	};

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
		
		ZLIntegerOption DelayOption;
		ZLIntegerOption ModeOption;
		ZLIntegerOption LinesToKeepOption;
		ZLIntegerOption LinesToScrollOption;
		ZLIntegerOption PercentToScrollOption;
	};

public:
	static ZLBooleanOption QuitOnCancelOption;
	static ZLBooleanOption StoreContentsPositionOption;

	static ScrollingOptions LargeScrollingOptions;
	static ScrollingOptions SmallScrollingOptions;
	static ScrollingOptions MouseScrollingOptions;

	static ZLBooleanOption SearchBackwardOption;
	static ZLBooleanOption SearchIgnoreCaseOption;
	static ZLBooleanOption SearchInWholeTextOption;
	static ZLStringOption SearchPatternOption;

	static const std::string HelpDirectory;

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
	virtual void cancelSlot() FB_SECTION = 0;
	virtual void fullscreenSlot() FB_SECTION = 0;
	virtual void quitSlot() FB_SECTION = 0;
	virtual bool isRotationSupported() const FB_SECTION = 0;

	virtual void bookInfoSlot() FB_SECTION;
	virtual void optionsSlot() FB_SECTION;
	virtual void addBookSlot() FB_SECTION;

private:
	BookDescriptionPtr createDescription(const std::string& fileName) const FB_SECTION;
	bool runBookInfoDialog(const std::string &fileName) FB_SECTION;
	void clearTextCaches() FB_SECTION;
	void FBReader::doScrolling(const ScrollingOptions &options, bool forward) FB_SECTION;

public:
	void openBook(BookDescriptionPtr description) FB_SECTION;
	BookTextView &textView() const FB_SECTION;
	void showBookTextView() FB_SECTION;
	void tryShowFootnoteView(const std::string &id) FB_SECTION;
	void restorePreviousMode() FB_SECTION;
	virtual void enableMenuButtons() FB_SECTION;
	void repaintView() FB_SECTION;
	void doAction(ActionCode code) FB_SECTION;

protected:
	ViewMode myMode;
	ViewMode myPreviousMode;
	ZLViewWidget *myViewWidget;
	FootnoteView *myFootnoteView;	
	BookTextView *myBookTextView;	
	ContentsView *myContentsView;	
	CollectionView *myCollectionView;	

private:
	ZLTime myLastScrollingTime;

	ZLPaintContext *myContext;
	BookModel *myModel;
};

#endif /* __FBREADER_H__ */
