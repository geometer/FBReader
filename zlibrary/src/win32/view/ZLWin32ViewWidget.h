/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLWIN32VIEWWIDGET_H__
#define __ZLWIN32VIEWWIDGET_H__

#include <windows.h>

#include <ZLView.h>
#include <ZLApplication.h>

class ZLWin32ViewWidget : public ZLViewWidget {

public:
	ZLWin32ViewWidget(ZLApplication &application, HWND window);

private:
	void repaint();
	void trackStylus(bool track);

	void doPaint();

private:
	ZLApplication &myApplication;
	HWND myWindow;

friend class ZLWin32ApplicationWindow;
};

#endif /* __ZLWIN32VIEWWIDGET_H__ */
