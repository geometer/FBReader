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

#ifndef __ZLWIN32DIALOG_H__
#define __ZLWIN32DIALOG_H__

#include <string>
#include <vector>
#include <map>

#include <windows.h>

#include <ZLDialog.h>

class ZLWin32Dialog : public ZLDialog {

public:
	ZLWin32Dialog(const std::string &name);
	~ZLWin32Dialog();

	void addButton(const std::string &text, bool accept);
	bool run();

private:
	typedef std::pair<std::string,bool> ButtonInfo;
	const std::string myTitle;
	std::vector<ButtonInfo> myButtons;
};

#endif /* __ZLWIN32DIALOG_H__ */