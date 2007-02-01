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

#include <algorithm>

#include "ZLWin32DialogElement.h"

ZLWin32DialogBox::ZLWin32DialogBox() : myHomogeneous(false), myTopMargin(0), myBottomMargin(0), myLeftMargin(0), myRightMargin(0), mySpacing(0) {
}

void ZLWin32DialogBox::addElement(ZLWin32DialogElementPtr element) {
	if (!element.isNull()) {
		myElements.push_back(element);
	}
}

void ZLWin32DialogBox::allocate(WORD *p) const {
	for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		(*it)->allocate(p);
		p += (*it)->allocationSize();
	}
}

int ZLWin32DialogBox::allocationSize() const {
	int size = 0;
	for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		size += (*it)->allocationSize();
	}
	return size;
}

void ZLWin32DialogBox::setVisible(bool visible) {
	// TODO: implement
}

int ZLWin32DialogBox::controlNumber() const {
	int number = 0;
	for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		number += (*it)->controlNumber();
	}
	return number;
}

void ZLWin32DialogBox::setHomogeneous(bool homogeneous) {
	myHomogeneous = homogeneous;
}

void ZLWin32DialogBox::setMargins(int top, int bottom, int left, int right) {
	myTopMargin = top;
	myBottomMargin = bottom;
	myLeftMargin = left;
	myRightMargin = right;
}

void ZLWin32DialogBox::setSpacing(int spacing) {
	mySpacing = spacing;
}

void ZLWin32DialogBox::setDimensions(Size charDimension) {
	for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		(*it)->setDimensions(charDimension);
	}
}

ZLWin32DialogHBox::ZLWin32DialogHBox() {
}

ZLWin32DialogElement::Size ZLWin32DialogHBox::minimumSize() const {
	if (myElements.empty()) {
		return Size(leftMargin() + rightMargin(), topMargin() + bottomMargin());
	}

	Size size;
	if (homogeneous()) {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();
			size.Width = std::max(size.Width, elementSize.Width);
			size.Height = std::max(size.Height, elementSize.Height);
		}
		size.Width *= myElements.size();
	} else {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();
			size.Width += elementSize.Width;
			size.Height = std::max(size.Height, elementSize.Height);
		}
	}
	size.Width += leftMargin() + rightMargin() + spacing() * (myElements.size() - 1);
	size.Height += topMargin() + bottomMargin();
	return size;
}

void ZLWin32DialogHBox::setPosition(int x, int y, Size size) {
	if (myElements.empty()) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		const Size elementSize(
			(size.Width - leftMargin() - rightMargin() - spacing() * (myElements.size() - 1)) / myElements.size(),
			size.Height - topMargin() - bottomMargin()
		);
		const short deltaX = elementSize.Width + spacing();
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->setPosition(x, y, elementSize);
			x += deltaX;
		}
	} else {
		const int elementHeight = size.Height - topMargin() - bottomMargin();
		Size elementSize;
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();
			elementSize.Height = elementHeight;
			(*it)->setPosition(x, y, elementSize);
			x += elementSize.Width + spacing();
		}
	}
}

ZLWin32DialogVBox::ZLWin32DialogVBox() {
}

ZLWin32DialogElement::Size ZLWin32DialogVBox::minimumSize() const {
	if (myElements.empty()) {
		return Size(leftMargin() + rightMargin(), topMargin() + bottomMargin());
	}

	Size size;
	if (homogeneous()) {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();	
			size.Width = std::max(size.Width, elementSize.Width);
			size.Height = std::max(size.Height, elementSize.Height);
		}
		size.Height *= myElements.size();
	} else {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();	
			size.Width = std::max(size.Width, elementSize.Width);
			size.Height += elementSize.Height;
		}
	}
	size.Width += leftMargin() + rightMargin();
	size.Height += topMargin() + bottomMargin() + spacing() * (myElements.size() - 1);
	return size;
}

void ZLWin32DialogVBox::setPosition(int x, int y, Size size) {
	if (myElements.empty()) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		const Size elementSize(
			size.Width - leftMargin() - rightMargin(),
			(size.Height - topMargin() - bottomMargin() - spacing() * (myElements.size() - 1)) / myElements.size()
		);
		const short deltaY = elementSize.Height + spacing();
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->setPosition(x, y, elementSize);
			y += deltaY;
		}
	} else {
		const short elementWidth = size.Width - leftMargin() - rightMargin();
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			Size elementSize = (*it)->minimumSize();
			elementSize.Width = elementWidth;
			(*it)->setPosition(x, y, elementSize);
			y += elementSize.Height + spacing();
		}
	}
}
