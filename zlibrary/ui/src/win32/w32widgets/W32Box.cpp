/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "W32Container.h"

W32Box::W32Box() : myHomogeneous(false), mySpacing(0) {
}

void W32Box::addElement(W32WidgetPtr element) {
	if (!element.isNull()) {
		myElements.push_back(element);
	}
}

void W32Box::allocate(WORD *&p, short &id) const {
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		(*it)->allocate(p, id);
	}
}

void W32Box::init(HWND parent, W32ControlCollection *collection) {
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		(*it)->init(parent, collection);
	}
}

int W32Box::allocationSize() const {
	int size = 0;
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		size += (*it)->allocationSize();
	}
	return size;
}

void W32Box::setVisible(bool visible) {
	for (W32WidgetList::iterator it = myElements.begin(); it != myElements.end(); ++it) {
		(*it)->setVisible(visible);
	}
}

bool W32Box::isVisible() const {
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		if ((*it)->isVisible()) {
			return true;
		}
	}
	return false;
}

int W32Box::visibleElementsNumber() const {
	int counter = 0;
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		if ((*it)->isVisible()) {
			++counter;
		}
	}
	return counter;
}

int W32Box::controlNumber() const {
	int number = 0;
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		number += (*it)->controlNumber();
	}
	return number;
}

void W32Box::setHomogeneous(bool homogeneous) {
	myHomogeneous = homogeneous;
}

void W32Box::setSpacing(int spacing) {
	mySpacing = spacing;
}

W32HBox::W32HBox() : myAlignment(LEFT) {
}

W32Widget::Size W32HBox::minimumSize() const {
	if (myElements.empty()) {
		return Size(leftMargin() + rightMargin(), topMargin() + bottomMargin());
	}

	Size size;
	int elementCounter = 0;
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		if ((*it)->isVisible()) {
			Size elementSize = (*it)->minimumSize();
			if (homogeneous()) {
				size.Width = std::max(size.Width, elementSize.Width);
			} else {
				size.Width += elementSize.Width;
			}
			size.Height = std::max(size.Height, elementSize.Height);
			++elementCounter;
		}
	}
	if (homogeneous()) {
		size.Width *= elementCounter;
	}
	size.Width += leftMargin() + rightMargin() + spacing() * (elementCounter - 1);
	size.Height += topMargin() + bottomMargin();
	return size;
}

void W32HBox::setAlignment(Alignment alignment) {
	myAlignment = alignment;
}

void W32HBox::setPosition(int x, int y, Size size) {
	int elementCounter = visibleElementsNumber();
	if (elementCounter == 0) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		Size elementSize(0, size.Height - topMargin() - bottomMargin());
		for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			if ((*it)->isVisible()) {
				Size currentSize = (*it)->minimumSize();
				elementSize.Width = std::max(currentSize.Width, elementSize.Width);
			}
		}
		if (myAlignment != LEFT) {
			const short extraWidth = size.Width - leftMargin() - rightMargin() - elementSize.Width * elementCounter - spacing() * (elementCounter - 1);
			if (myAlignment == CENTER) {
				x += extraWidth / 2;
			} else if (myAlignment == RIGHT) {
				x += extraWidth;
			} else if (myAlignment == FILL) {
				elementSize.Width += extraWidth / elementCounter;
			}
		}
		const short deltaX = elementSize.Width + spacing();
		for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			if ((*it)->isVisible()) {
				(*it)->setPosition(x, y, elementSize);
				x += deltaX;
			}
		}
	} else {
		if (myAlignment != LEFT) {
			short extraWidth = size.Width - leftMargin() - rightMargin() - spacing() * (elementCounter - 1);
			for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
				if ((*it)->isVisible()) {
					Size currentSize = (*it)->minimumSize();
					extraWidth -= currentSize.Width;
				}
			}
			if (myAlignment == CENTER) {
				x += extraWidth / 2;
			} else if (myAlignment == RIGHT) {
				x += extraWidth;
			}
		}
		const int elementHeight = size.Height - topMargin() - bottomMargin();
		Size elementSize;
		for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			if ((*it)->isVisible()) {
				Size elementSize = (*it)->minimumSize();
				elementSize.Height = elementHeight;
				(*it)->setPosition(x, y, elementSize);
				x += elementSize.Width + spacing();
			}
		}
	}
}

W32VBox::W32VBox() {
}

W32Widget::Size W32VBox::minimumSize() const {
	if (myElements.empty()) {
		return Size(leftMargin() + rightMargin(), topMargin() + bottomMargin());
	}

	Size size;
	int elementCounter = 0;
	for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
		if ((*it)->isVisible()) {
			Size elementSize = (*it)->minimumSize();	
			size.Width = std::max(size.Width, elementSize.Width);
			if (homogeneous()) {
				size.Height = std::max(size.Height, elementSize.Height);
			} else {
				size.Height += elementSize.Height;
			}
			++elementCounter;
		}
	}
	if (homogeneous()) {
		size.Height *= elementCounter;
	}
	size.Width += leftMargin() + rightMargin();
	size.Height += topMargin() + bottomMargin() + spacing() * (elementCounter - 1);
	return size;
}

void W32VBox::setPosition(int x, int y, Size size) {
	int elementCounter = visibleElementsNumber();
	if (elementCounter == 0) {
		return;
	}

	x += leftMargin();
	y += topMargin();
	if (homogeneous()) {
		const Size elementSize(
			size.Width - leftMargin() - rightMargin(),
			(size.Height - topMargin() - bottomMargin() - spacing() * (elementCounter - 1)) / elementCounter
		);
		const short deltaY = elementSize.Height + spacing();
		for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			if ((*it)->isVisible()) {
				(*it)->setPosition(x, y, elementSize);
				y += deltaY;
			}
		}
	} else {
		const short elementWidth = size.Width - leftMargin() - rightMargin();
		for (W32WidgetList::const_iterator it = myElements.begin(); it != myElements.end(); ++it) {
			if ((*it)->isVisible()) {
				Size elementSize = (*it)->minimumSize();
				elementSize.Width = elementWidth;
				(*it)->setPosition(x, y, elementSize);
				y += elementSize.Height + spacing();
			}
		}
	}
}
