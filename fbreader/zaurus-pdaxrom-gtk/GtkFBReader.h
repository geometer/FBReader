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
	static std::string ImageDirectory;
	
public:
	GtkFBReader();
	~GtkFBReader();

protected:
	void setWindowCaption(const std::string &caption);
	void addButton(ActionCode id, const std::string &name);
	void setButtonVisible(ActionCode id, bool visible);
	void setButtonEnabled(ActionCode id, bool enable);
	void searchSlot();
	void cancelSlot();
	void fullscreenSlot();
	bool isRotationSupported() const { return true; }

public:
	void handleKeySlot(GdkEventKey *);
	void close();

private:
	void addKeyBinding(guint keyval, GdkModifierType state, ActionCode code);
	void addKeyBinding(const std::string &accelerator, ActionCode code);

private:
	GtkWindow *myMainWindow;
	GtkWidget *myToolbar;

	std::map<std::pair<guint,GdkModifierType>,ActionCode> myKeyBindings;
	std::map<ActionCode,GtkWidget*> myButtons;

	bool myFullScreen;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
