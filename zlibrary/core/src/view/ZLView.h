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

#ifndef __ZLVIEW_H__
#define __ZLVIEW_H__

#include <string>

#include <shared_ptr.h>

#include <ZLPaintContext.h>

class ZLApplication;

class ZLView {

public:
	ZLView(ZLApplication &application, shared_ptr<ZLPaintContext> context = 0);
	virtual ~ZLView();
	virtual void setPaintContext(shared_ptr<ZLPaintContext> context);

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

protected:
	ZLApplication &application();
	const ZLApplication &application() const;

	bool hasContext() const;

private:
	ZLApplication &myApplication;
	shared_ptr<ZLPaintContext> myContext;

private:
	ZLView(const ZLView&);
	const ZLView &operator=(const ZLView&);
};

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
	void setView(shared_ptr<ZLView> view);
	shared_ptr<ZLView> view() const;

	virtual void trackStylus(bool track) = 0;

	void rotate(Angle rotation);
	Angle rotation() const;

protected:
	virtual void repaint() = 0;

private:
	shared_ptr<ZLView> myView;
	Angle myRotation;

friend class ZLApplication;
};

inline ZLViewWidget::ZLViewWidget(Angle initialAngle) : myView(0), myRotation(initialAngle) {}
inline ZLViewWidget::~ZLViewWidget() {}
inline shared_ptr<ZLView> ZLViewWidget::view() const { return myView; }
inline void ZLViewWidget::rotate(Angle rotation) { myRotation = rotation; }
inline ZLViewWidget::Angle ZLViewWidget::rotation() const { return myRotation; }

inline bool ZLView::hasContext() const { return !myContext.isNull(); }
inline ZLPaintContext &ZLView::context() const { return *myContext; }
inline ZLApplication &ZLView::application() { return myApplication; }
inline const ZLApplication &ZLView::application() const { return myApplication; }

#endif /* __ZLVIEW_H__ */
