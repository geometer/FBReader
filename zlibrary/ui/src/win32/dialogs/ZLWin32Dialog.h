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

#ifndef __ZLWIN32DIALOG_H__
#define __ZLWIN32DIALOG_H__

#include <string>

#include <ZLDialog.h>

#include "../w32widgets/W32DialogPanel.h"
#include "../w32widgets/W32Container.h"

class ZLWin32ApplicationWindow;

class ZLWin32Dialog : public ZLDialog {

public:
	ZLWin32Dialog(ZLWin32ApplicationWindow &window, const ZLResource &resource);

	void addButton(const ZLResourceKey &key, bool accept);
	bool run();

private:
	ZLWin32ApplicationWindow &myWindow;
	W32StandaloneDialogPanel myPanel;
	W32HBox *myButtonBox;
};

#endif /* __ZLWIN32DIALOG_H__ */
