/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GTKFBREADER_H__
#define __GTKFBREADER_H__

#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "../common/fbreader/FBReader.h"

class GtkFBReader : public FBReader { 
protected:
	// FIXME: do we really need this enum to be protected?
	enum EventCode {
		EVENT_SHOW_COLLECTION,
		EVENT_SHOW_OPTIONS,
		EVENT_UNDO,
		EVENT_REDO,
		EVENT_SHOW_CONTENTS,
		EVENT_SEARCH,
		EVENT_FIND_PREVIOUS,
		EVENT_FIND_NEXT,
		EVENT_SCROLL_FORWARD,
		EVENT_SCROLL_BACKWARD,
		EVENT_CANCEL,
		EVENT_INCREASE_FONT,
		EVENT_DECREASE_FONT
	};

private:
	static std::string ImageDirectory;
	
public:
	GtkFBReader();
	~GtkFBReader();

	void enableMenuButtons();

private:
	GtkWidget *addToolButton(GtkWidget *toolbar, const std::string &name, GtkSignalFunc signal);

protected:
	void setWindowCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }
	void setMode(ViewMode mode);

public:
	void showCollectionSlot() { setMode(BOOK_COLLECTION_MODE); }
	void showContentsSlot() { setMode(CONTENTS_MODE); }
	void undoSlot() { undoPage(); }
	void redoSlot() { redoPage(); }
	void searchSlot();
	void findNextSlot() { findNext(); }
	void findPreviousSlot() { findPrevious(); }
	void showOptionsDialogSlot() { showOptionsDialog(); repaintView(); }
	void showHidePositionIndicatorSlot() { showHidePositionIndicator(); }  

	gboolean handleKeySlot(GdkEventKey *);

	void repaintView();
	void close();

	void increaseFontSlot() { increaseFont(); }
	void decreaseFontSlot() { decreaseFont(); }

	void scrollForwardSlot();
	void scrollBackwardSlot();
	void cancelSlot();

/*
private:
	QTime myLastScrollingTime;
*/

private:
	GtkWindow *myMainWindow;

	GtkWidget *myBookCollectionButton;
	GtkWidget *mySettingsButton;
	GtkWidget *myLeftArrowButton;
	GtkWidget *myRightArrowButton;
	GtkWidget *myContentsTableButton;
	GtkWidget *mySearchButton;
	GtkWidget *myFindNextButton;
	GtkWidget *myFindPreviousButton;

	std::map<std::string, EventCode> myKeyBindings;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
