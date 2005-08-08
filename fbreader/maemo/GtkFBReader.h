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

#include <hildon-widgets/hildon-app.h>
#include <hildon-widgets/hildon-appview.h>

#include "../common/fbreader/FBReader.h"

struct ActionSlotData;

class GtkFBReader : public FBReader { 

public:
	static const std::string ImageDirectory;

public:
	GtkFBReader();
	~GtkFBReader();

protected:
	void setWindowCaption(const std::string &caption) { hildon_app_set_title(myApp, caption.c_str()); }
	void addButton(ActionCode id, const std::string &name);
	void addButtonSeparator();
	void setButtonVisible(ActionCode id, bool visible);
	void setButtonEnabled(ActionCode id, bool enable);
	void enableMenuButtons();
	void searchSlot();
	void cancelSlot();
	void fullscreenSlot();
	bool isRotationSupported() const { return true; }

	ActionSlotData *getSlotData(ActionCode);

	void buildMenu();

public:
	void handleKeySlot(GdkEventKey *);
	void quitSlot();

	void close();

	// FIXME: we need to figure out the way to find the main window :( Bloody hildon.
	// GtkWindow *getMainWindow() { return myMainWindow; }
	GtkWindow *getMainWindow() { return NULL; }

private:
	HildonApp *myApp;
	HildonAppView *myAppView;
	GtkToolbar *myToolbar;
	GtkWidget *myMenu;

	bool myFullScreen;

	std::map<std::string,ActionCode> myKeyBindings;
	std::map<ActionCode,GtkToolItem*> myButtons;

	std::map<ActionCode,ActionSlotData*> myActions;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
