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
	void doAction(ActionCode code);

	gboolean handleKeySlot(GdkEventKey *);

	void repaintView();
	void close();

private:
	void searchSlot();
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

	std::map<std::string,ActionCode> myKeyBindings;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
