/*
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

#ifndef __GTKDIALOGMANAGER_H__
#define __GTKDIALOGMANAGER_H__

#include <gtk/gtkwindow.h>
#include <gtk/gtkdialog.h>

#include <ZLDialogManager.h>

class GtkDialogManager : public ZLDialogManager {

public:
	static void createInstance() { ourInstance = new GtkDialogManager(); }

private:
	GtkDialogManager() : myIsInitialized(false), myWindow(0), myIsKeyboardGrabbed(false), myIsWaiting(false) {}

public:
	void createApplicationWindow(ZLApplication *application) const;

	ZLDialog *createDialog(const std::string &title) const;
	ZLOptionsDialog *createOptionsDialog(const std::string &id, const std::string &title) const;
	int infoBox(const InfoBoxType type, const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const;
	void openFileDialog(const std::string &title, ZLTreeHandler &handler) const;

	void wait(ZLRunnable &runnable, const std::string &message) const;
	bool isWaiting() const { return myIsWaiting; }

	void setMainWindow(GtkWindow *window) const { myIsInitialized = true; myWindow = window; }

	void grabKeyboard(bool grab) { myIsKeyboardGrabbed = grab; }
	bool isKeyboardGrabbed() const { return myIsKeyboardGrabbed; }

private:
	mutable bool myIsInitialized;
	mutable GtkWindow *myWindow;
	bool myIsKeyboardGrabbed;

	mutable bool myIsWaiting;

friend GtkDialog *createGtkDialog(const std::string& title);
};

#endif /* __GTKDIALOGMANAGER_H__ */
