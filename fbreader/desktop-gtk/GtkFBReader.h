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

#include <gtk/gtk.h>

#include "../common/fbreader/FBReader.h"

class GtkFBReader : public FBReader { 

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
	void showOptionsDialogSlot();

	void repaintView();
	void close();
/*
	void increaseFontSlot() { increaseFont(); }
	void decreaseFontSlot() { decreaseFont(); }
	void scrollForwardSlot();
	void scrollBackwardSlot();
	void cancelSlot();

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
};

#endif /* __GTKFBREADER_H__ */
