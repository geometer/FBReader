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

#ifndef __FBREADERACTIONS_H__
#define __FBREADERACTIONS_H__

#include <ZLApplication.h>

#include "FBReader.h"

class ZLTextView;

class ActionCode {

public:
	static const std::string SHOW_READING;
	static const std::string SHOW_LIBRARY;
	static const std::string SHOW_NETWORK_LIBRARY;
	static const std::string SHOW_TOC;
	static const std::string SHOW_HELP;
	static const std::string SHOW_OPTIONS_DIALOG;
	static const std::string SHOW_BOOK_INFO_DIALOG;
	static const std::string UNDO;
	static const std::string REDO;
	static const std::string SEARCH;
	static const std::string FIND_PREVIOUS;
	static const std::string FIND_NEXT;
	static const std::string PAGE_SCROLL_FORWARD;
	static const std::string PAGE_SCROLL_BACKWARD;
	static const std::string LINE_SCROLL_FORWARD;
	static const std::string LINE_SCROLL_BACKWARD;
	static const std::string MOUSE_SCROLL_FORWARD;
	static const std::string MOUSE_SCROLL_BACKWARD;
	static const std::string TAP_SCROLL_FORWARD;
	static const std::string TAP_SCROLL_BACKWARD;
	static const std::string SCROLL_TO_HOME;
	static const std::string SCROLL_TO_START_OF_TEXT;
	static const std::string SCROLL_TO_END_OF_TEXT;
	static const std::string CANCEL;
	static const std::string INCREASE_FONT;
	static const std::string DECREASE_FONT;
	static const std::string SHOW_HIDE_POSITION_INDICATOR;
	static const std::string TOGGLE_FULLSCREEN;
	static const std::string FULLSCREEN_ON;
	static const std::string ADD_BOOK;
	static const std::string ROTATE_SCREEN;
	static const std::string QUIT;
	static const std::string FORCE_QUIT;
	static const std::string OPEN_PREVIOUS_BOOK;
	static const std::string GOTO_NEXT_TOC_SECTION;
	static const std::string GOTO_PREVIOUS_TOC_SECTION;
	static const std::string COPY_SELECTED_TEXT_TO_CLIPBOARD;
	static const std::string CLEAR_SELECTION;
	static const std::string OPEN_SELECTED_TEXT_IN_DICTIONARY;
	static const std::string GOTO_PAGE_NUMBER;
	static const std::string GOTO_PAGE_NUMBER_WITH_PARAMETER;
	static const std::string SEARCH_ON_NETWORK;
	static const std::string ADVANCED_SEARCH_ON_NETWORK;
	static const std::string ORGANIZE_BOOKS_BY_AUTHOR;
	static const std::string ORGANIZE_BOOKS_BY_TAG;
	static const std::string FILTER_LIBRARY;

private:
	ActionCode();
};

class ModeDependentAction : public ZLApplication::Action {

protected:
	ModeDependentAction(int visibleInModes);

public:
	bool isVisible() const;

private:
	int myVisibleInModes;
};

class SetModeAction : public ModeDependentAction {

public:
	SetModeAction(FBReader::ViewMode modeToSet, int visibleInModes);
	void run();

private:
	FBReader::ViewMode myModeToSet;
};

class ShowHelpAction : public ZLApplication::Action {

public:
	void run();
};

class ShowOptionsDialogAction : public ZLApplication::Action {

public:
	void run();
};

class ShowContentsAction : public SetModeAction {

public:
	ShowContentsAction();
	bool isVisible() const;
};

class ShowNetworkLibraryAction : public SetModeAction {

public:
	ShowNetworkLibraryAction();
	bool isVisible() const;
};

class AddBookAction : public ModeDependentAction {

public:
	AddBookAction(int visibleInModes);
	void run();
};

class ShowBookInfoAction : public ModeDependentAction {

public:
	ShowBookInfoAction();
	void run();
};

class ScrollToHomeAction : public ModeDependentAction {

public:
	ScrollToHomeAction();
	bool isEnabled() const;
	void run();
};

class ScrollToStartOfTextAction : public ModeDependentAction {

public:
	ScrollToStartOfTextAction();
	bool isEnabled() const;
	void run();
};

class ScrollToEndOfTextAction : public ModeDependentAction {

public:
	ScrollToEndOfTextAction();
	bool isEnabled() const;
	void run();
};

class UndoAction : public ModeDependentAction {

public:
	UndoAction(int visibleInModes);
	bool isEnabled() const;
	void run();
};

class RedoAction : public ModeDependentAction {

public:
	RedoAction();
	bool isEnabled() const;
	void run();
};

class SearchAction : public ZLApplication::Action {

public:
	bool isVisible() const;
};

class SearchPatternAction : public SearchAction {

public:
	SearchPatternAction();
	void run();

private:
	ZLBooleanOption SearchBackwardOption;
	ZLBooleanOption SearchIgnoreCaseOption;
	ZLBooleanOption SearchInWholeTextOption;
	ZLBooleanOption SearchThisSectionOnlyOption;
	ZLStringOption SearchPatternOption;

friend class SearchPatternEntry;
};

class FindNextAction : public SearchAction {

public:
	bool isEnabled() const;
	void run();
};

class FindPreviousAction : public SearchAction {

public:
	bool isEnabled() const;
	void run();
};

class ChangeFontSizeAction : public ZLApplication::Action {

public:
	ChangeFontSizeAction(int delta);
	bool isEnabled() const;
	void run();

private:
	const int myDelta;
};

class CancelAction : public ZLApplication::Action {

public:
	void run();
};

class ToggleIndicatorAction : public ZLApplication::Action {

public:
	bool isVisible() const;
	void run();
};

class QuitAction : public ZLApplication::Action {

public:
	void run();
};

class ForceQuitAction : public ZLApplication::Action {

public:
	void run();
};

class OpenPreviousBookAction : public ZLApplication::Action {

public:
	bool isVisible() const;
	void run();
};

class GotoNextTOCSectionAction : public ZLApplication::Action {

public:
	bool isVisible() const;
	bool isEnabled() const;
	void run();
};

class GotoPreviousTOCSectionAction : public ZLApplication::Action {

public:
	bool isVisible() const;
	bool isEnabled() const;
	void run();
};

class GotoPageNumberAction : public ModeDependentAction {

public:
	GotoPageNumberAction(const std::string &parameter);
	bool isVisible() const;
	bool isEnabled() const;
	void run();

private:
	const std::string myParameter;
};

class SelectionAction : public ZLApplication::Action {

public:
	bool isVisible() const;
	bool isEnabled() const;

protected:
	ZLTextView &textView() const;
};

class CopySelectedTextAction : public SelectionAction {

public:
	bool isVisible() const;
	void run();
};

class OpenSelectedTextInDictionaryAction : public SelectionAction {

public:
	bool isVisible() const;
	void run();
};

class ClearSelectionAction : public SelectionAction {

public:
	void run();
};

class SearchOnNetworkAction : public ModeDependentAction {

public:
	SearchOnNetworkAction();
	void run();

private:
	virtual void doSearch() = 0;
};

class SimpleSearchOnNetworkAction : public SearchOnNetworkAction {

private:
	void doSearch();
	std::string makeSummary(const std::string &pattern);
};

class AdvancedSearchOnNetworkAction : public SearchOnNetworkAction {

private:
	void doSearch();
	std::string makeSummary(const std::string &titleAndSeries, const std::string &author, const std::string &category, const std::string &description);
	void appendQueryValue(std::string &query, const std::string &name, const std::string &value);
};

class FBFullscreenAction : public ZLApplication::FullscreenAction {

public:
	void run();
};

class BooksOrderAction : public ModeDependentAction {

public:
	BooksOrderAction();
	void run();
};

class FilterLibraryAction : public ModeDependentAction {

public:
	FilterLibraryAction();
	void run();
};

#endif /* __FBREADERACTIONS_H__ */
