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

#ifndef __ZLVIEW_H__
#define __ZLVIEW_H__

#include <string>

class ZLView;
class ZLPaintContext;

class ZLViewWidget {

public:
	enum Angle {
		DEGREES0 = 0,
		DEGREES90 = 90,
		DEGREES180 = 180,
		DEGREES270 = 270
	};

protected:
	ZLViewWidget(Angle initialAngle);

public:
	virtual ~ZLViewWidget();
	void setView(ZLView *view);
	ZLView *view() const;

	virtual void trackStylus(bool track) = 0;
	virtual void repaintView() = 0;

	void rotate(Angle rotation);
	Angle rotation() const;

private:
	ZLView *myView;
	Angle myRotation;
};

class ZLView {

public:
	ZLView(ZLPaintContext &context);
	virtual ~ZLView();

	virtual const std::string &caption() const = 0;
	virtual void paint() = 0;
	ZLPaintContext &context() const;

	/*
	 * returns true iff stylus/finger event was processed
	 */
	virtual bool onStylusPress(int x, int y);
	virtual bool onStylusRelease(int x, int y);
	virtual bool onStylusMove(int x, int y);
	virtual bool onStylusMovePressed(int x, int y);
	virtual bool onFingerTap(int x, int y);

	void repaintView();

private:
	ZLViewWidget *myWidget;
	ZLPaintContext &myContext;

friend void ZLViewWidget::setView(ZLView *view);
};

inline ZLViewWidget::ZLViewWidget(Angle initialAngle) : myView(0), myRotation(initialAngle) {}
inline ZLViewWidget::~ZLViewWidget() {}
inline ZLView *ZLViewWidget::view() const { return myView; }
inline void ZLViewWidget::rotate(Angle rotation) { myRotation = rotation; }
inline ZLViewWidget::Angle ZLViewWidget::rotation() const { return myRotation; }

inline ZLPaintContext &ZLView::context() const { return myContext; }
inline bool ZLView::onStylusPress(int, int) { return false; }
inline bool ZLView::onStylusRelease(int, int) { return false; }
inline bool ZLView::onStylusMove(int, int) { return false; }
inline bool ZLView::onStylusMovePressed(int, int) { return false; }
inline bool ZLView::onFingerTap(int, int) { return false; }

#endif /* __ZLVIEW_H__ */
