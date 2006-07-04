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

#ifndef __ZLDIALOGMANAGER_H__
#define __ZLDIALOGMANAGER_H__

#include <string>

#include <abstract/ZLRunnable.h>

class ZLDialog;
class ZLOptionsDialog;
class ZLTreeHandler;
class ZLApplication;

class ZLDialogManager {

public:
	static bool isInitialized();
	static ZLDialogManager &instance();
	static void deleteInstance();

protected:
	static ZLDialogManager *ourInstance;

protected:
	ZLDialogManager();
	virtual ~ZLDialogManager();

public:
	virtual void createApplicationWindow(ZLApplication *application) const = 0;

	virtual ZLDialog *createDialog(const std::string &title) const = 0;
	virtual ZLOptionsDialog *createOptionsDialog(const std::string &id, const std::string &title) const = 0;
	virtual int questionBox(const std::string &title, const std::string &message, const std::string &button0 = "", const std::string &button1 = "", const std::string &button2 = "") const = 0;
	virtual void openFileDialog(const std::string &title, const ZLTreeHandler &handler) const = 0;
	virtual void wait(ZLRunnable &runnable, const std::string &message) const = 0;
};

#endif /* __ZLDIALOGMANAGER_H__ */
