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

class BookDescription;
class BookModel;
class BookTextView;
class FootnoteView;
class ContentsView;
class CollectionView;
class ViewWidget;
class PaintContext;

class FBReader {

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
	FBReader(PaintContext *context);
	virtual ~FBReader();

	ViewMode mode() const { return myMode; }
	virtual void setMode(ViewMode mode) = 0;
	
	virtual void setWindowCaption(const std::string &caption) = 0;

	virtual void searchSlot() = 0;
	virtual void cancelSlot() = 0;
	virtual void fullscreenSlot() = 0;

public:
	void openBook(BookDescription *description);
	BookTextView &textView() const { return *myBookTextView; }
	void showBookTextView() { setMode(BOOK_TEXT_MODE); }
	void tryShowFootnoteView(const std::string &id);
	void restorePreviousMode() { setMode(myPreviousMode); myPreviousMode = BOOK_TEXT_MODE; }
	virtual void enableMenuButtons() = 0;
	void repaintView();
	void doAction(ActionCode code);

protected:
	ViewMode myMode;
	ViewMode myPreviousMode;
	ViewWidget *myViewWidget;
  FootnoteView *myFootnoteView;	
  BookTextView *myBookTextView;	
  ContentsView *myContentsView;	
  CollectionView *myCollectionView;	

private:
	ZLTime myLastScrollingTime;

	PaintContext *myContext;
	BookModel *myModel;
};

#endif /* __FBREADER_H__ */
