/*
 * FBReader -- electronic book reader
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

class View;
class PaintContext;

class ViewWidget {

protected:
	ViewWidget() : myIsRotated(false) {}

public:
	void setView(View *view);
	View *view() const { return myView; }

	virtual void repaintView() = 0;

	void rotate() { myIsRotated = !myIsRotated; }
	bool isRotated() const { return myIsRotated; }

private:
	View *myView;
	bool myIsRotated;
};

class View {

public:
	View(PaintContext &context) : myContext(context) {}
	virtual ~View() {}

	virtual const std::string &caption() const VIEW_SECTION = 0;
	virtual void paint() VIEW_SECTION = 0;
	PaintContext &context() const { return myContext; }

	/*
	 * returns true iff stylus pressing was processed
	 */
	virtual bool onStylusPress(int, int) { return false; }

	void repaintView() VIEW_SECTION;

private:
	ViewWidget *myWidget;
	PaintContext &myContext;

friend void ViewWidget::setView(View *view);
};

#endif /* __VIEW_H__ */
