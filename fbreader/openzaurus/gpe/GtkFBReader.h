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

#ifndef __GTKFBREADER_H__
#define __GTKFBREADER_H__

#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtktoolbar.h>
#include <gtk/gtktoolitem.h>
#include <gtk/gtkwindow.h>

#include "../../common/fbreader/FBReader.h"

struct ActionSlotData;

class GtkFBReader : public FBReader { 

public:
	static const std::string ImageDirectory;

public:
	GtkFBReader(const std::string& bookToOpen);
	~GtkFBReader();

protected:
	void setWindowCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }
	void addToolbarItem(Toolbar::ItemPtr item);
	void refresh();
	void searchSlot();
	void toggleFullscreenSlot();
	bool isFullscreen() const;

	ActionSlotData *getSlotData(ActionCode);

public:
	void handleKeyEventSlot(GdkEventKey *event);
	void handleScrollEventSlot(GdkEventScroll *event);
	// MSS: this is not good, I suppose: the original method is protected, I make it public here...
	void quitSlot();

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
  ZLIntegerRangeOption myWidthOption;
  ZLIntegerRangeOption myHeightOption;

	GtkWindow *myMainWindow;
	GtkToolbar *myToolbar;

	bool myFullScreen;

	std::map<Toolbar::ItemPtr,GtkToolItem*> myButtons;
	std::map<ActionCode,ActionSlotData*> myActions;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
