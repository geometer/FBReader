/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLDIALOGMANAGER_H__
#define __ZLDIALOGMANAGER_H__

#include <string>

class ZLOptionsDialog;

class ZLDialogManager {

public:
	static ZLDialogManager &instance() { return *ourInstance; } 
	static void deleteInstance() { delete ourInstance; }

protected:
	static ZLDialogManager *ourInstance;

protected:
	virtual ~ZLDialogManager() {}

public:
	virtual ZLOptionsDialog *createOptionsDialog(const char *title) const = 0;
	virtual int informationBox(const char *title, const char *message, const char *button0 = 0, const char *button1 = 0, const char *button2 = 0) const = 0;
	virtual std::string getFileName(const std::string &title, const std::string &dir) const = 0;
};

#endif /* __ZLDIALOGMANAGER_H__ */
