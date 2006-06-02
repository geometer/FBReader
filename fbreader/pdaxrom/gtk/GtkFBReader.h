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

#include "../../common/fbreader/FBReader.h"

class GtkFBReader : public FBReader { 

public:
	static const std::string ImageDirectory;
	
public:
	GtkFBReader(const std::string& bookToOpen);
	~GtkFBReader();

protected:
	void setWindowCaption(const std::string &caption);
	void addButton(ActionCode id, const std::string &name);
	void addButtonSeparator() {} // TODO: implement
	void setButtonVisible(ActionCode id, bool visible);
	void setButtonEnabled(ActionCode id, bool enable);
	void searchSlot();
	void toggleFullscreenSlot();
	bool isFullscreen() const;
	void quitSlot();
	bool isRotationSupported() const { return true; }

public:
	void handleKeyEventSlot(GdkEventKey*);

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
  ZLIntegerRangeOption myWidthOption;
  ZLIntegerRangeOption myHeightOption;

	GtkWindow *myMainWindow;
	GtkWidget *myToolbar;

	std::map<ActionCode,GtkWidget*> myButtons;

	bool myFullScreen;
};

#endif /* __GTKFBREADER_H__ */

// vim:ts=2:sw=2:noet
