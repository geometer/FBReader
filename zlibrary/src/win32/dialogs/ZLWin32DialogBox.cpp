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

#include "ZLWin32DialogElement.h"

ZLWin32DialogBox::ZLWin32DialogBox() {
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

ZLWin32DialogHBox::ZLWin32DialogHBox() {
}

ZLWin32DialogVBox::ZLWin32DialogVBox() {
}
