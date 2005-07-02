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

#ifndef __PALMDIALOGMANAGER_H__
#define __PALMDIALOGMANAGER_H__

#include <abstract/ZLDialogManager.h>

class PalmDialogManager : public ZLDialogManager {

public:
	static void createInstance() DIALOG_SECTION;

private:
	PalmDialogManager() DIALOG_SECTION;

public:
	~PalmDialogManager() DIALOG_SECTION;
	ZLOptionsDialog *createOptionsDialog(const std::string &id, const std::string &title) const DIALOG_SECTION;
	int informationBox(const char *title, const char *message, const char *button0, const char *button1, const char *button2) const DIALOG_SECTION;
	void openFileDialog(const std::string &title, const ZLTreeHandler &handler) const DIALOG_SECTION;
};

#endif /* __PALMDIALOGMANAGER_H__ */
