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
		ACTION_SCROLL_FORWARD,
		ACTION_SCROLL_BACKWARD,
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
	};

protected:
	enum ViewMode {
		UNDEFINED_MODE,
		BOOK_TEXT_MODE,
		FOOTNOTE_MODE,
		CONTENTS_MODE,
		BOOKMARKS_MODE,
		BOOK_COLLECTION_MODE,
	};

public:
	static ZLBooleanOption QuitOnCancelOption;
	static ZLIntegerOption ScrollingDelayOption;

	static ZLBooleanOption SearchBackwardOption;
	static ZLBooleanOption SearchIgnoreCaseOption;
	static ZLBooleanOption SearchInWholeTextOption;
	static ZLStringOption SearchPatternOption;

	static std::string HelpDirectory;

protected:
	FBReader(ZLPaintContext *context) FB_SECTION;
	virtual ~FBReader() FB_SECTION;

	virtual void setMode(ViewMode mode) FB_SECTION;
	virtual void setWindowCaption(const std::string &caption) FB_SECTION = 0;
	void createToolbar() FB_SECTION;
	virtual void addButton(ActionCode id, const std::string &name) FB_SECTION = 0;
	virtual void setButtonVisible(ActionCode id, bool visible) FB_SECTION = 0;
	virtual void setButtonEnabled(ActionCode id, bool enable) FB_SECTION = 0;
	virtual void searchSlot() FB_SECTION = 0;
	virtual void cancelSlot() FB_SECTION = 0;
	virtual void fullscreenSlot() FB_SECTION = 0;
	virtual bool isRotationSupported() const FB_SECTION = 0;

private:
	bool runBookInfoDialog(const std::string &fileName) FB_SECTION;
	void clearTextCaches() FB_SECTION;

public:
	void openBook(BookDescriptionPtr description) FB_SECTION;
	BookTextView &textView() const FB_SECTION;
	void showBookTextView() FB_SECTION;
	void tryShowFootnoteView(const std::string &id) FB_SECTION;
	void restorePreviousMode() FB_SECTION;
	void enableMenuButtons() FB_SECTION;
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
