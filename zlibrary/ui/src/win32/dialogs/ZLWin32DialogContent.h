/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32DIALOGCONTENT_H__
#define __ZLWIN32DIALOGCONTENT_H__

#include <shared_ptr.h>

#include <ZLDialogContent.h>

#include "../w32widgets/W32Container.h"

#include "ZLWin32OptionView.h"

class ZLWin32DialogContent : public ZLDialogContent {

public:
	ZLWin32DialogContent(const ZLResource &resource);
	~ZLWin32DialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	W32WidgetPtr contentPtr() const;
	W32Table &contentTable() const;

	void insertWidget(W32WidgetPtr widget, int from, int to);

private:
	void createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int from, int to);

private:
	W32WidgetPtr myContentTable;
	int myRowCounter;
};

#endif /* __ZLWIN32DIALOGCONTENT_H__ */
