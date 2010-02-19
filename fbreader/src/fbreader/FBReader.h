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

#ifndef __FBREADER_H__
#define __FBREADER_H__

#include <string>
#include <map>

#include <ZLOptions.h>
#include <ZLTime.h>
#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

#include "../library/Library.h"
#include "../external/ProgramCollection.h"

class ZLMessageHandler;

class Book;
class BookModel;
class BookTextView;

class FBReader : public ZLApplication {

public:
	static FBReader &Instance();

public:
	// returns true if description was found or error message was shown
	static bool createBook(const std::string &fileName, shared_ptr<Book> &book);

	static const std::string PageIndexParameter;

public:
	enum ViewMode {
		UNDEFINED_MODE = 0,
		BOOK_TEXT_MODE = 1 << 0,
		FOOTNOTE_MODE = 1 << 1,
		CONTENTS_MODE = 1 << 2,
		BOOKMARKS_MODE = 1 << 3,
		LIBRARY_MODE = 1 << 4,
		NETWORK_LIBRARY_MODE = 1 << 5,
		ALL_MODES = 0xFF
	};

public:
	ZLBooleanOption QuitOnCancelOption;

	ZLIntegerRangeOption KeyScrollingDelayOption;
	ZLIntegerRangeOption LinesToScrollOption;
	ZLIntegerRangeOption LinesToKeepOption;
	ZLBooleanOption EnableTapScrollingOption;
	ZLBooleanOption TapScrollingOnFingerOnlyOption;

	ZLBooleanOption UseSeparateBindingsOption;

	ZLBooleanOption EnableSingleClickDictionaryOption;

public:
	FBReader(const std::string &bookToOpen);
	~FBReader();

	void setMode(ViewMode mode);
	ViewMode mode() const;

	shared_ptr<Book> currentBook() const;

	void refreshWindow();

private:
	void initWindow();

	void clearTextCaches();

	void restorePreviousMode();

	bool closeView();
	std::string helpFileName(const std::string &language) const;

	void openFile(const std::string &filePath);
	bool canDragFiles(const std::vector<std::string> &filePaths) const;
	void dragFiles(const std::vector<std::string> &filePaths);

	bool isViewFinal() const;

	void showLibraryView();

public:
	shared_ptr<ZLKeyBindings> keyBindings();
	shared_ptr<ZLKeyBindings> keyBindings(ZLView::Angle angle);

	bool isDictionarySupported() const;
	void openInDictionary(const std::string &word);

	shared_ptr<ProgramCollection> webBrowserCollection() const;
	void openLinkInBrowser(const std::string &url) const;

	void tryShowFootnoteView(const std::string &id, const std::string &type);
	BookTextView &bookTextView() const;
	void showBookTextView();
	void openBook(shared_ptr<Book> book);

	void invalidateNetworkView();
	void invalidateAccountDependents();

private:
	shared_ptr<ProgramCollection> dictionaryCollection() const;

	void openBookInternal(shared_ptr<Book> book);
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
	shared_ptr<ZLView> myNetworkLibraryView;
	shared_ptr<ZLView> myLibraryByAuthorView;
	shared_ptr<ZLView> myLibraryByTagView;
	shared_ptr<ZLPopupData> myRecentBooksPopupData;

	ZLTime myLastScrollingTime;

	shared_ptr<BookModel> myModel;

	shared_ptr<ZLKeyBindings> myBindings0;
	shared_ptr<ZLKeyBindings> myBindings90;
	shared_ptr<ZLKeyBindings> myBindings180;
	shared_ptr<ZLKeyBindings> myBindings270;

	std::string myBookToOpen;
	bool myBookAlreadyOpen;

	ProgramCollectionMap myProgramCollectionMap;

	shared_ptr<ZLMessageHandler> myOpenFileHandler;

	enum {
		RETURN_TO_TEXT_MODE,
		UNFULLSCREEN
	} myActionOnCancel;

friend class OpenFileHandler;

friend class OptionsDialog;
friend class FBView;

//friend class ShowCollectionAction;
friend class ShowHelpAction;
//friend class ShowOptionsDialogAction;
friend class ShowContentsAction;
friend class AddBookAction;
friend class ShowBookInfoAction;
//friend class ScrollToHomeAction;
//friend class ScrollToStartOfTextAction;
//friend class ScrollToEndOfTextAction;
friend class UndoAction;
//friend class RedoAction;
friend class SearchAction;
friend class SearchPatternAction;
friend class FindNextAction;
friend class FindPreviousAction;
friend class ScrollingAction;
friend class ScrollingAction2;
friend class ChangeFontSizeAction;
friend class CancelAction;
//friend class ToggleIndicatorAction;
friend class QuitAction;
friend class OpenPreviousBookAction;
friend class GotoNextTOCSectionAction;
friend class GotoPreviousTOCSectionAction;
//friend class GotoPageNumber;
friend class SelectionAction;
friend class SearchOnNetworkAction;
friend class AdvancedSearchOnNetworkAction;
friend class FBFullscreenAction;
friend class BooksOrderAction;
friend class LogOutAction;
};

#endif /* __FBREADER_H__ */
