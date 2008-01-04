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

#ifndef __ZLDESKTOPSELECTIONDIALOG_H__
#define __ZLDESKTOPSELECTIONDIALOG_H__

#include <ZLSelectionDialog.h>
#include <ZLOptions.h>

class ZLDesktopSelectionDialog : public ZLSelectionDialog {

protected:
	ZLDesktopSelectionDialog(ZLTreeHandler &handler);

public:
	bool runWithSize();

protected:
	virtual void setSize(int width, int height) = 0;
	virtual int width() const = 0;
	virtual int height() const = 0;

private:
	ZLIntegerRangeOption WidthOption;
	ZLIntegerRangeOption HeightOption;
};

#endif /* __ZLDESKTOPSELECTIONDIALOG_H__ */
