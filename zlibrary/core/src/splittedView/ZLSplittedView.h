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

#ifndef __ZLSPLITTEDVIEW_H__
#define __ZLSPLITTEDVIEW_H__

#include <ZLView.h>

class ZLSplittedView : public ZLView {

public:
	ZLSplittedView(ZLApplication &application, shared_ptr<ZLPaintContext> context = 0);
	~ZLSplittedView();
	void setPaintContext(shared_ptr<ZLPaintContext> context);

	void paint();

	void setLeftView(shared_ptr<ZLView> view);
	void setRightView(shared_ptr<ZLView> view);

	bool onStylusPress(int x, int y);
	bool onStylusRelease(int x, int y);
	bool onStylusMove(int x, int y);
	bool onStylusMovePressed(int x, int y);
	bool onFingerTap(int x, int y);

private:
	shared_ptr<ZLView> myLeftView;
	shared_ptr<ZLView> myRightView;
};

#endif /* __ZLSPLITTEDVIEW_H__ */
