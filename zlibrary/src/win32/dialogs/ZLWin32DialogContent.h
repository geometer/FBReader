/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "../w32widgets/W32Element.h"

#include "ZLWin32OptionView.h"

class ZLWin32DialogContent : public ZLDialogContent {

public:
	ZLWin32DialogContent();
	~ZLWin32DialogContent();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	//Win32Widget *widget() { return WIN32_WIDGET(myTable); }

	//void addItem(Win32Widget *what, int row, int fromColumn, int toColumn);

	W32ElementPtr content() const;

private:
	int addRow();
	ZLWin32OptionView *createViewByEntry(ZLOptionEntry *option);

private:
	W32VBox *myContentBox;
	W32ElementPtr myContentBoxAsElementPtr;
};

#endif /* __ZLWIN32DIALOGCONTENT_H__ */
