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

#ifndef __ZLOPTIONSDIALOG_H__
#define __ZLOPTIONSDIALOG_H__

#include <string>

#include <abstract/ZLOptions.h>

class ZLOptionEntry;

class ZLOptionsDialogTab {

public:
	virtual ~ZLOptionsDialogTab() {}
	virtual void addOption(ZLOptionEntry *option) = 0;
	virtual void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) = 0;
};

class ZLOptionsDialog {

protected:
	static ZLStringOption TabOption;

public:
	virtual ~ZLOptionsDialog() {}
	virtual ZLOptionsDialogTab *createTab(const std::string &name) = 0;
	virtual int run(const std::string &tabName);

protected:
	virtual std::string selectedTabName() = 0;
	virtual void selectTab(const std::string &name) = 0;
	virtual int run() = 0;
};

#endif /* __ZLOPTIONSDIALOG_H__ */
