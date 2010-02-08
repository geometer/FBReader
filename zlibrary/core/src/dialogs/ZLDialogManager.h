/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLDIALOGMANAGER_H__
#define __ZLDIALOGMANAGER_H__

#include <string>

#include <shared_ptr.h>
#include <ZLRunnable.h>
#include <ZLResource.h>
#include <ZLOpenFileDialog.h>

class ZLDialog;
class ZLOptionsDialog;
class ZLTreeHandler;
class ZLApplication;
class ZLImageData;
class ZLProgressDialog;

class ZLDialogManager {

public:
	static const ZLResourceKey OK_BUTTON;
	static const ZLResourceKey CANCEL_BUTTON;
	static const ZLResourceKey YES_BUTTON;
	static const ZLResourceKey NO_BUTTON;
	static const ZLResourceKey APPLY_BUTTON;

	static const ZLResourceKey COLOR_KEY;
	static const ZLResourceKey DIALOG_TITLE;

	static const std::string &dialogTitle(const ZLResourceKey &key);
	static const std::string &dialogMessage(const ZLResourceKey &key);
	static const std::string &buttonName(const ZLResourceKey &key);
	static const std::string &waitMessageText(const ZLResourceKey &key);

protected:
	static const ZLResource &resource();

public:
	static bool isInitialized();
	static ZLDialogManager &Instance();
	static void deleteInstance();

protected:
	static ZLDialogManager *ourInstance;

protected:
	ZLDialogManager();
	virtual ~ZLDialogManager();

public:
	virtual void createApplicationWindow(ZLApplication *application) const = 0;

	virtual shared_ptr<ZLDialog> createDialog(const ZLResourceKey &key) const = 0;
	virtual shared_ptr<ZLOptionsDialog> createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction = 0, bool showApplyButton = false) const = 0;
	// TODO: change to pure virtual
	virtual shared_ptr<ZLOpenFileDialog> createOpenFileDialog(const std::string &directoryPath, const ZLOpenFileDialog::Filter &filter) const;
	virtual bool selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const = 0;

	void informationBox(const ZLResourceKey &key) const;
	void informationBox(const ZLResourceKey &key, const std::string &message) const;
	virtual void informationBox(const std::string &title, const std::string &message) const = 0;

	void errorBox(const ZLResourceKey &key) const;
	virtual void errorBox(const ZLResourceKey &key, const std::string &message) const = 0;

	int questionBox(const ZLResourceKey &key, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2 = ZLResourceKey()) const;
	virtual int questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2 = ZLResourceKey()) const = 0;

	virtual shared_ptr<ZLProgressDialog> createProgressDialog(const ZLResourceKey &key) const = 0;
	void wait(const ZLResourceKey &key, ZLRunnable &runnable) const;

	enum ClipboardType {
		CLIPBOARD_MAIN,
		CLIPBOARD_SELECTION
	};
	virtual bool isClipboardSupported(ClipboardType type) const = 0;
	virtual void setClipboardText(const std::string &text, ClipboardType type) const = 0;
	virtual void setClipboardImage(const ZLImageData &imageData, ClipboardType type) const = 0;
};

#endif /* __ZLDIALOGMANAGER_H__ */
