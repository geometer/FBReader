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
#include <gtk/gtkwindow.h>

#include <hildon-widgets/hildon-program.h>
#include <libosso.h>

#include "../../common/fbreader/FBReader.h"

struct ActionSlotData;

class GtkFBReader : public FBReader { 

public:
	static const std::string ImageDirectory;

public:
	GtkFBReader(const std::string& bookToOpen);
	~GtkFBReader();

protected:
	void setWindowCaption(const std::string &caption) { gtk_window_set_title(GTK_WINDOW(myWindow), caption.c_str()); }
	void addButton(ActionCode id, const std::string &name);
	void addButtonSeparator();
	void setButtonVisible(ActionCode id, bool visible);
	void setButtonEnabled(ActionCode id, bool enable);
	void enableMenuButtons();
	void searchSlot();
	void toggleFullscreenSlot();
	bool isFullscreen() const;
	bool isRotationSupported() const { return true; }

	ActionSlotData *getSlotData(ActionCode);

	void buildMenu();

public:
	void handleKeyEventSlot(GdkEventKey*);
	void quitSlot();

	HildonWindow *getMainWindow() const { return myWindow; }

private:
	GtkMenu *makeSubmenu(GtkMenu *menu, const char *label);
	void addMenuItem(GtkMenu *menu, const char *label, ActionCode code);

private:
	HildonProgram *myProgram;
	HildonWindow *myWindow;
	GtkToolbar *myToolbar;
	GtkMenu *myMenu;

	bool myFullScreen;

	std::map<ActionCode,GtkToolItem*> myButtons;
	std::map<ActionCode,GtkMenuItem*> myMenuItems;

	std::map<ActionCode,ActionSlotData*> myActions;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
