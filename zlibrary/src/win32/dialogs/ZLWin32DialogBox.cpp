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

ZLWin32DialogHBox::ZLWin32DialogHBox() {
}

void ZLWin32DialogHBox::minimumSize(int &x, int &y) const {
	if (myElements.empty()) {
		x = leftMargin() + rightMargin();
		y = topMargin() + bottomMargin();
		return;
	}

	x = 0; y = 0;
	int elementX, elementY;
	if (homogeneous()) {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(elementX, elementY);	
			x = std::max(x, elementX);
			y = std::max(y, elementY);
		}
		x *= myElements.size();
	} else {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(elementX, elementY);	
			x += elementX;
			y = std::max(y, elementY);
		}
	}
	x += leftMargin() + rightMargin() + spacing() * (myElements.size() - 1);
	y += topMargin() + bottomMargin();
}

void ZLWin32DialogHBox::setPosition(int x, int y, int width, int height) {
	if (myElements.empty()) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		const int elementWidth = (width - leftMargin() - rightMargin() - spacing() * (myElements.size() - 1)) / myElements.size();
		const int elementHeight = height - topMargin() - bottomMargin();
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->setPosition(x, y, elementWidth, elementHeight);
			x += elementWidth + spacing();
		}
	} else {
		int elementWidth;
		const int elementHeight = height - topMargin() - bottomMargin();
		int minimumHeight;
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(elementWidth, minimumHeight);
			(*it)->setPosition(x, y, elementWidth, elementHeight);
			x += elementWidth + spacing();
		}
	}
}

ZLWin32DialogVBox::ZLWin32DialogVBox() {
}

void ZLWin32DialogVBox::minimumSize(int &x, int &y) const {
	if (myElements.empty()) {
		x = leftMargin() + rightMargin();
		y = topMargin() + bottomMargin();
		return;
	}

	x = 0; y = 0;
	int elementX, elementY;
	if (homogeneous()) {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(elementX, elementY);	
			x = std::max(x, elementX);
			y = std::max(y, elementY);
		}
		y *= myElements.size();
	} else {
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(elementX, elementY);	
			x = std::max(x, elementX);
			y += elementY;
		}
	}
	x += leftMargin() + rightMargin();
	y += topMargin() + bottomMargin() + spacing() * (myElements.size() - 1);
}

void ZLWin32DialogVBox::setPosition(int x, int y, int width, int height) {
	if (myElements.empty()) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		const int elementWidth = width - leftMargin() - rightMargin();
		const int elementHeight = (height - topMargin() - bottomMargin() - spacing() * (myElements.size() - 1)) / myElements.size();
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->setPosition(x, y, elementWidth, elementHeight);
			y += elementHeight + spacing();
		}
	} else {
		const int elementWidth = width - leftMargin() - rightMargin();
		int elementHeight;
		int minimumWidth;
		for (ZLWin32DialogElementList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			(*it)->minimumSize(minimumWidth, elementHeight);
			(*it)->setPosition(x, y, elementWidth, elementHeight);
			y += elementHeight + spacing();
		}
	}
}
