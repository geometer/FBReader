/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __W32CONTAINER_H__
#define __W32CONTAINER_H__

#include <map>
#include <string>
#include <list>

#include <shared_ptr.h>

#include "W32Widget.h"

class W32Box : public W32Widget {

public:
	W32Box();
	void addElement(W32WidgetPtr element);

	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	void setVisible(bool visible);
	bool isVisible() const;
	int controlNumber() const;

	void setHomogeneous(bool homogeneous);
	void setMargins(int top, int bottom, int left, int right);
	void setSpacing(int spacing);

protected:
	bool homogeneous() const { return myHomogeneous; }
	int topMargin() const { return myTopMargin; }
	int bottomMargin() const { return myBottomMargin; }
	int leftMargin() const { return myLeftMargin; }
	int rightMargin() const { return myRightMargin; }
	int spacing() const { return mySpacing; }
	void setDimensions(Size charDimension);
	void init(HWND parent, W32ControlCollection *collection);

	int visibleElementsNumber() const;

protected:
	W32WidgetList myElements;

private:
	bool myHomogeneous;
	int myTopMargin, myBottomMargin, myLeftMargin, myRightMargin;
	int mySpacing;
};

class W32HBox : public W32Box {

public:
	enum Alignment {
		LEFT,
		RIGHT,
		CENTER
	};

public:
	W32HBox();

	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
	void setAlignment(Alignment alignment);

private:
	Alignment myAlignment;
};

class W32VBox : public W32Box {

public:
	W32VBox();

	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
};

class W32Table : public W32Widget {
	
public:
	W32Table();
	void setElement(W32WidgetPtr element, int row, int fromColumn, int toColumn);

	void setMargins(int top, int bottom, int left, int right);
	void setSpacings(int vertical, int horizontal);

private:
	void calculateSizes(std::vector<short> &widths, std::vector<short> &heights) const;

	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	void setVisible(bool visible);
	bool isVisible() const;
	int controlNumber() const;
	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
	void setDimensions(Size charDimension);
	void init(HWND parent, W32ControlCollection *collection);

private:
	struct CellInfo {
		CellInfo(int xFrom, int xTo, W32WidgetPtr widget) : XFrom(xFrom), XTo(xTo), Widget(widget) {}
		int XFrom, XTo;
		W32WidgetPtr Widget;
	};
	typedef std::list<CellInfo> RowList;

	std::vector<RowList> myRows;
	int myTopMargin, myBottomMargin, myLeftMargin, myRightMargin;
	int myVerticalSpacing, myHorizontalSpacing;
};

#endif /* __W32CONTAINER_H__ */
