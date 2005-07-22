/*
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

#ifndef __GTKDIALOGMANAGER_H__
#define __GTKDIALOGMANAGER_H__

#include <gtk/gtkwindow.h>

#include <abstract/ZLDialogManager.h>

class GtkDialogManager : public ZLDialogManager {

public:
	static void createInstance() { ourInstance = new GtkDialogManager(); }

private:
	GtkDialogManager() : myWindow(0) {}

public:
	ZLOptionsDialog *createOptionsDialog(const std::string &id, const std::string &title) const;
	int informationBox(const char *title, const char *message, const char *button0, const char *button1, const char *button2) const;
	void openFileDialog(const std::string &title, const ZLTreeHandler &handler) const;

	void setPixmapPath(const std::string &pixmapPath) { myPixmapPath = pixmapPath; }
	const std::string &getPixmapPath() const { return myPixmapPath; }

	void setMainWindow(GtkWindow *window) { myWindow = window; }

private:
	std::string myPixmapPath;
	GtkWindow *myWindow;
};

#endif /* __GTKDIALOGMANAGER_H__ */
