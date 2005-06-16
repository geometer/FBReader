/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <string>

class ZLApplication {

protected:
	ZLApplication() APPLICATION_SECTION;
	virtual ~ZLApplication() APPLICATION_SECTION;

public:
	virtual void enableMenuButtons() APPLICATION_SECTION = 0;
};

class View;
class PaintContext;

class ViewWidget {

protected:
	ViewWidget() VIEW_SECTION;
	virtual ~ViewWidget() VIEW_SECTION;

public:
	void setView(View *view) VIEW_SECTION;
	View *view() const VIEW_SECTION;

	virtual void repaintView() VIEW_SECTION = 0;

	void rotate() VIEW_SECTION;
	bool isRotated() const VIEW_SECTION;

private:
	View *myView;
	bool myIsRotated;
};

class View {

public:
	View(PaintContext &context) VIEW_SECTION;
	virtual ~View() VIEW_SECTION;

	virtual const std::string &caption() const VIEW_SECTION = 0;
	virtual void paint() VIEW_SECTION = 0;
	PaintContext &context() const VIEW_SECTION;

	/*
	 * returns true iff stylus pressing was processed
	 */
	virtual bool onStylusPress(int, int) VIEW_SECTION;

	void repaintView() VIEW_SECTION;

private:
	ViewWidget *myWidget;
	PaintContext &myContext;

friend void ViewWidget::setView(View *view);
};

inline ZLApplication::ZLApplication() {}
inline ZLApplication::~ZLApplication() {}

inline ViewWidget::ViewWidget() : myIsRotated(false) {}
inline ViewWidget::~ViewWidget() {}
inline View *ViewWidget::view() const { return myView; }
inline void ViewWidget::rotate() { myIsRotated = !myIsRotated; }
inline bool ViewWidget::isRotated() const { return myIsRotated; }

inline PaintContext &View::context() const { return myContext; }
inline bool View::onStylusPress(int, int) { return false; }

#endif /* __VIEW_H__ */
