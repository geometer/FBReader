/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLDESKTOPOPENFILEDIALOG_H__
#define __ZLDESKTOPOPENFILEDIALOG_H__

#include <ZLOpenFileDialog.h>
#include <ZLOptions.h>

class ZLDesktopOpenFileDialog : public ZLOpenFileDialog {

protected:
	ZLDesktopOpenFileDialog();

public:
	bool run();

protected:
	virtual bool runInternal() = 0;
	virtual void setPosition(int x, int y) = 0;
	virtual void setSize(int width, int height) = 0;
	virtual int x() const = 0;
	virtual int y() const = 0;
	virtual int width() const = 0;
	virtual int height() const = 0;

private:
	ZLIntegerOption myXOption;
	ZLIntegerOption myYOption;
	ZLIntegerOption myWidthOption;
	ZLIntegerOption myHeightOption;
};

#endif /* __ZLDESKTOPOPENFILEDIALOG_H__ */
