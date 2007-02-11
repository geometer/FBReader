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

#ifndef __ZLWIN32OPTIONSDIALOG_H__
#define __ZLWIN32OPTIONSDIALOG_H__

#include <vector>

#include <windows.h>

#include <shared_ptr.h>
#include <ZLOptionsDialog.h>

#include "../w32widgets/W32DialogPanel.h"

class ZLWin32DialogContent;
class ZLWin32ApplicationWindow;

class ZLWin32OptionsDialog : public ZLOptionsDialog, public W32Listener {

public:
	ZLWin32OptionsDialog(ZLWin32ApplicationWindow &window, const std::string &id, const std::string &caption);
	ZLDialogContent &createTab(const std::string &name);

protected:
	const std::string &selectedTabName() const;
	void selectTab(const std::string &name);
	bool run();

private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	ZLWin32ApplicationWindow &myWindow;
	W32PropertySheet myPropertySheet;

	std::vector<shared_ptr<ZLWin32DialogContent> > myTabs;
	std::string mySelectedTabName;
};

#endif /* __ZLWIN32OPTIONSDIALOG_H__ */
