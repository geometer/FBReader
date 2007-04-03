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

#ifndef __FBREADERACTIONS_H__
#define __FBREADERACTIONS_H__

#include <ZLApplication.h>

#include "FBReader.h"

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
	ACTION_FINGER_TAP_SCROLL_FORWARD = 31,
	ACTION_FINGER_TAP_SCROLL_BACKWARD = 32,
	ACTION_GOTO_NEXT_TOC_SECTION = 33,
	ACTION_GOTO_PREVIOUS_TOC_SECTION = 34,
};

class FBAction : public ZLApplication::Action {

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

class ShowHelpAction : public FBAction {

public:
	ShowHelpAction(FBReader &fbreader);
	void run();
};

class ShowRecentBooksListAction : public FBAction {

public:
	ShowRecentBooksListAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class ShowOptionsDialogAction : public FBAction {

public:
	ShowOptionsDialogAction(FBReader &fbreader);
	void run();
};

class ShowContentsAction : public FBAction {

public:
	ShowContentsAction(FBReader &fbreader);
	bool isVisible();
	bool isEnabled();
	void run();
};

class AddBookAction : public FBAction {

public:
	AddBookAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class ShowBookInfoAction : public FBAction {

public:
	ShowBookInfoAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class ScrollingAction : public FBAction {

public:
	ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward);
	bool isEnabled();
	bool useKeyDelay() const;
	void run();

private:
	const FBReader::ScrollingOptions &myOptions;
	const bool myForward;
};

class ScrollToHomeAction : public FBAction {

public:
	ScrollToHomeAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class ScrollToStartOfTextAction : public FBAction {

public:
	ScrollToStartOfTextAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class ScrollToEndOfTextAction : public FBAction {

public:
	ScrollToEndOfTextAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class UndoAction : public FBAction {

public:
	UndoAction(FBReader &fbreader);
	bool isEnabled();
	void run();
};

class RedoAction : public FBAction {

public:
	RedoAction(FBReader &fbreader);
	bool isVisible();
	bool isEnabled();
	void run();
};

class SearchAction : public FBAction {

public:
	SearchAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class FindNextAction : public FBAction {

public:
	FindNextAction(FBReader &fbreader);
	bool isEnabled();
	void run();
};

class FindPreviousAction : public FBAction {

public:
	FindPreviousAction(FBReader &fbreader);
	bool isEnabled();
	void run();
};

class ChangeFontSizeAction : public FBAction {

public:
	ChangeFontSizeAction(FBReader &fbreader, int delta);
	void run();

private:
	const int myDelta;
};

class CancelAction : public FBAction {

public:
	CancelAction(FBReader &fbreader);
	void run();
};

class ToggleIndicatorAction : public FBAction {

public:
	ToggleIndicatorAction(FBReader &fbreader);
	void run();
};

class QuitAction : public FBAction {

public:
	QuitAction(FBReader &fbreader);
	void run();
};

class OpenPreviousBookAction : public FBAction {

public:
	OpenPreviousBookAction(FBReader &fbreader);
	bool isVisible();
	void run();
};

class GotoNextTOCSectionAction : public FBAction {

public:
	GotoNextTOCSectionAction(FBReader &fbreader);
	bool isVisible();
	bool isEnabled();
	void run();
};

class GotoPreviousTOCSectionAction : public FBAction {

public:
	GotoPreviousTOCSectionAction(FBReader &fbreader);
	bool isVisible();
	bool isEnabled();
	void run();
};

#endif /* __FBREADERACTIONS_H__ */
