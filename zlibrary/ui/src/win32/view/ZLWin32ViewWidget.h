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

#ifndef __ZLWIN32VIEWWIDGET_H__
#define __ZLWIN32VIEWWIDGET_H__

#include <ZLView.h>
#include <ZLApplication.h>

#include "../application/ZLWin32ApplicationWindow.h"

class ZLWin32ViewWidget : public ZLViewWidget {

public:
	ZLWin32ViewWidget(ZLWin32ApplicationWindow &window);

	void onMousePress(int x, int y);
	void onMouseRelease(int x, int y);
	void onMouseMove(int x, int y);
	void onMouseMovePressed(int x, int y);

private:
	void repaint();
	void trackStylus(bool track);

	void doPaint();

private:
	ZLWin32ApplicationWindow &myWindow;
	bool myMouseCaptured;

	class Rotator {

	public:
		Rotator(int width, int height);
		~Rotator();
		void setSize(int width, int height);

		void setRotation(bool counterClockWise);
		void retrieve(HDC dc, HBITMAP bitmap);
		void rotate();
		void draw(HDC dc, int topOffset);

	private:
		int myWidth;
		int myHeight;
		long *myBuffer;
		long *myBuffer1;
		int mySign;
	};

	Rotator *myRotator;

friend class ZLWin32ApplicationWindow;
};

#endif /* __ZLWIN32VIEWWIDGET_H__ */
