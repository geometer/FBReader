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

#ifndef __ZLWIN32OPTIONSDIALOG_H__
#define __ZLWIN32OPTIONSDIALOG_H__

#include <vector>
#include <map>

#include <windows.h>

#include <shared_ptr.h>
#include <ZLOptionsDialog.h>

#include "../w32widgets/W32DialogPanel.h"

class ZLWin32ApplicationWindow;

class ZLWin32OptionsDialog : public ZLOptionsDialog, public W32Listener {

public:
	ZLWin32OptionsDialog(ZLWin32ApplicationWindow &window, const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	ZLWin32ApplicationWindow &myWindow;
	W32PropertySheet myPropertySheet;

	std::string mySelectedTabKey;

	std::map<W32DialogPanel*,std::string> myPanelToKeyMap;
};

#endif /* __ZLWIN32OPTIONSDIALOG_H__ */
