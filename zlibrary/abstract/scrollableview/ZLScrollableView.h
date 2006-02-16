/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLSCROLLABLEVIEW_H__
#define __ZLSCROLLABLEVIEW_H__

class ZLViewable;

class ZLScrollableView {

protected:
	ZLScrollableView() {}
	virtual ~ZLScrollableView() {}
	ZLViewable &viewable() const { return *myViewable; }
	
public:
	virtual void repaintView() = 0;
	virtual void clearView() = 0;

	virtual int currentWidth() const = 0;
	virtual int currentHeight() const = 0;
	void adjustViewSize(int x, int y);

	virtual void startTimer(int mseconds) = 0;

protected:
	virtual int visibleAreaWidth(bool scrollBarHidden) const = 0;
	virtual int visibleAreaHeight(bool scrollBarHidden) const = 0;
	virtual void resizeView(int w, int h) = 0;

private:
	ZLViewable *myViewable;

friend class ZLViewable;
};

class ZLViewable {

protected:
	ZLViewable(ZLScrollableView *view) { myView = view; view->myViewable = this; }
	virtual ~ZLViewable() {}

public:
	virtual void doRealPaint() = 0;

	virtual void onTimerTick() {}
	virtual void onStylusPress(int, int) {}
	virtual void onStylusRelease(int, int) {}
	virtual void onStylusMove(int, int) {}
	virtual void onStylusDoubleClick(int, int) {}

	void startTimer(int mseconds) { myView->startTimer(mseconds); }
	void repaintView() { myView->repaintView(); }
	int currentViewWidth() const { return myView->currentWidth(); }
	int currentViewHeight() const { return myView->currentHeight(); }

	ZLScrollableView &view() const { return *myView; }

private:
	ZLScrollableView *myView;
};

#endif /* __ZLSCROLLABLEVIEW_H__ */
