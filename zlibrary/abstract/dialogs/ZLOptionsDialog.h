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

#ifndef __ZLOPTIONSDIALOG_H__
#define __ZLOPTIONSDIALOG_H__

#include <string>

#include <abstract/ZLDialogContent.h>
#include <abstract/ZLOptions.h>

class ZLOptionEntry;

class ZLOptionsDialog {

protected:
	ZLOptionsDialog(const std::string &id);
	
public:
	virtual ~ZLOptionsDialog();
	virtual ZLDialogContent &createTab(const std::string &name) = 0;
	virtual bool run(const std::string &tabName);

protected:
	virtual const std::string &selectedTabName() const = 0;
	virtual void selectTab(const std::string &name) = 0;
	virtual bool run() = 0;

protected:
	ZLStringOption TabOption;
};

inline ZLOptionsDialog::~ZLOptionsDialog() {}

#endif /* __ZLOPTIONSDIALOG_H__ */
