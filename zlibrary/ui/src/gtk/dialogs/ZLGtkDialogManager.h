/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLGTKDIALOGMANAGER_H__
#define __ZLGTKDIALOGMANAGER_H__

#include <stack>

#include <gtk/gtkwindow.h>
#include <gtk/gtkdialog.h>

#include <ZLDialogManager.h>

class ZLGtkDialogManager : public ZLDialogManager {

public:
	static void createInstance() { ourInstance = new ZLGtkDialogManager(); }

private:
	ZLGtkDialogManager() : myWindow(0), myIsKeyboardGrabbed(false) {}

public:
	void createApplicationWindow(ZLApplication *application) const;

	shared_ptr<ZLDialog> createDialog(const ZLResourceKey &key) const;
	shared_ptr<ZLOptionsDialog> createOptionsDialog(const ZLResourceKey &id, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const;
	void informationBox(const ZLResourceKey &key, const std::string &message) const;
	void errorBox(const ZLResourceKey &key, const std::string &message) const;
	int questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const;
	bool selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const;
	void wait(const ZLResourceKey &key, ZLRunnable &runnable) const;

	bool isClipboardSupported(ClipboardType type) const;
	void setClipboardText(const std::string &text, ClipboardType type) const;

	void grabKeyboard(bool grab) { myIsKeyboardGrabbed = grab; }
	bool isKeyboardGrabbed() const { return myIsKeyboardGrabbed; }

private:
	int internalBox(const gchar *icon, const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0 = OK_BUTTON, const ZLResourceKey &button1 = ZLResourceKey(), const ZLResourceKey &button2 = ZLResourceKey()) const;

private:
	mutable GtkWindow *myWindow;
	mutable std::stack<GtkWindow*> myDialogs;
	bool myIsKeyboardGrabbed;

friend GtkDialog *createGtkDialog(const std::string& title);
friend void destroyGtkDialog(GtkDialog *dialog);
};

#endif /* __ZLGTKDIALOGMANAGER_H__ */
