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

#ifndef __ZLDIALOG_H__
#define __ZLDIALOG_H__

#include <string>
#include <ZLDialogContent.h>
#include <ZLResource.h>

class ZLDialog {

protected:
	ZLDialog();

public:
	virtual ~ZLDialog();

	virtual void addButton(const ZLResourceKey &key, bool accept) = 0;

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOption(const ZLResourceKey &key, ZLOptionEntry *option);
	void addOption(const ZLResourceKey &key, ZLSimpleOption &option);

	const ZLResource &resource(const ZLResourceKey &key) const;

	// TODO: change return type to int (or std::string?)
	virtual bool run() = 0;
	void acceptValues();

protected:
	ZLDialogContent *myTab;
};

#endif /* __ZLDIALOG_H__ */
