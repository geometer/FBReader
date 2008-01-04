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

#ifndef __ZLSPLITTEDVIEW_H__
#define __ZLSPLITTEDVIEW_H__

#include <ZLView.h>

class ZLSplittedView : public ZLView {

public:
	ZLSplittedView(ZLApplication &application, shared_ptr<ZLPaintContext> context = 0);
	~ZLSplittedView();

	void paint();

	void setLeftView(shared_ptr<ZLView> view);
	void setRightView(shared_ptr<ZLView> view);

	bool onStylusPress(int x, int y);
	bool onStylusRelease(int x, int y);
	bool onStylusMove(int x, int y);
	bool onStylusMovePressed(int x, int y);
	bool onFingerTap(int x, int y);

	virtual int splitterWidth() const = 0;
	virtual ZLColor splitterColor() const = 0;
	virtual int leftRatioN() const = 0;
	virtual int leftRatioD() const = 0;
	virtual void moveSplitterTo(int x) = 0;

	int partWidth(bool left) const;

private:
	shared_ptr<ZLView> myLeftView;
	shared_ptr<ZLView> myRightView;
	bool myMoveSplitter;
};

#endif /* __ZLSPLITTEDVIEW_H__ */
