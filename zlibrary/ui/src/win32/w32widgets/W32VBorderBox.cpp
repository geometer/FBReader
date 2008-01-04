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

#include "W32Container.h"

W32VBorderBox::W32VBorderBox() : mySpacing(0) {
}

void W32VBorderBox::setTopElement(W32WidgetPtr widget) {
	myTopElement = widget;
}

void W32VBorderBox::setCenterElement(W32WidgetPtr widget) {
	myCenterElement = widget;
}

void W32VBorderBox::setBottomElement(W32WidgetPtr widget) {
	myBottomElement = widget;
}

void W32VBorderBox::setSpacing(int spacing) {
	mySpacing = spacing;
}

void W32VBorderBox::allocate(WORD *&p, short &id) const {
	if (!myTopElement.isNull()) {
		myTopElement->allocate(p, id);
	}
	if (!myCenterElement.isNull()) {
		myCenterElement->allocate(p, id);
	}
	if (!myBottomElement.isNull()) {
		myBottomElement->allocate(p, id);
	}
}

int W32VBorderBox::allocationSize() const {
	int size = 0;
	if (!myTopElement.isNull()) {
		size += myTopElement->allocationSize();
	}
	if (!myCenterElement.isNull()) {
		size += myCenterElement->allocationSize();
	}
	if (!myBottomElement.isNull()) {
		size += myBottomElement->allocationSize();
	}
	return size;
}

void W32VBorderBox::setVisible(bool visible) {
	if (!myTopElement.isNull()) {
		myTopElement->setVisible(visible);
	}
	if (!myCenterElement.isNull()) {
		myCenterElement->setVisible(visible);
	}
	if (!myBottomElement.isNull()) {
		myBottomElement->setVisible(visible);
	}
}

bool W32VBorderBox::isVisible() const {
	if (!myTopElement.isNull() && myTopElement->isVisible()) {
		return true;
	}
	if (!myCenterElement.isNull() && myCenterElement->isVisible()) {
		return true;
	}
	if (!myBottomElement.isNull() && myBottomElement->isVisible()) {
		return true;
	}
	return false;
}

int W32VBorderBox::controlNumber() const {
	int counter = 0;
	if (!myTopElement.isNull()) {
		counter += myTopElement->controlNumber();
	}
	if (!myCenterElement.isNull()) {
		counter += myCenterElement->controlNumber();
	}
	if (!myBottomElement.isNull()) {
		counter += myBottomElement->controlNumber();
	}
	return counter;
}

W32Widget::Size W32VBorderBox::minimumSize() const {
	Size size;
	int counter = 0;

	if (!myTopElement.isNull() && myTopElement->isVisible()) {
		++counter;
		Size elementSize = myTopElement->minimumSize();
		size.Height += elementSize.Height;
		size.Width = std::max(size.Width, elementSize.Width);
	}
	if (!myCenterElement.isNull() && myCenterElement->isVisible()) {
		++counter;
		Size elementSize = myCenterElement->minimumSize();
		size.Height += elementSize.Height;
		size.Width = std::max(size.Width, elementSize.Width);
	}
	if (!myBottomElement.isNull() && myBottomElement->isVisible()) {
		++counter;
		Size elementSize = myBottomElement->minimumSize();
		size.Height += elementSize.Height;
		size.Width = std::max(size.Width, elementSize.Width);
	}

	if (counter > 0) {
		size.Height += (counter - 1) * mySpacing;
	}

	size.Width += leftMargin() + rightMargin();
	size.Height += topMargin() + bottomMargin();
	return size;
}

void W32VBorderBox::setPosition(int x, int y, Size size) {
	const short elementX = x + leftMargin();
	const short elementWidth = size.Width - leftMargin() - rightMargin();

	short centerElementX = elementX;
	short centerElementY = y + topMargin();
	short centerElementHeight = size.Height - topMargin() - bottomMargin();

	if (!myTopElement.isNull() && myTopElement->isVisible()) {
		Size topElementSize = myTopElement->minimumSize();
		myTopElement->setPosition(elementX, centerElementY, Size(elementWidth, topElementSize.Height));
		centerElementY += topElementSize.Height + mySpacing;
		centerElementHeight -= topElementSize.Height + mySpacing;
	}
	if (!myBottomElement.isNull() && myBottomElement->isVisible()) {
		Size bottomElementSize = myBottomElement->minimumSize();
		myBottomElement->setPosition(elementX, y + size.Height - bottomMargin() - bottomElementSize.Height, Size(elementWidth, bottomElementSize.Height));
		centerElementHeight -= bottomElementSize.Height + mySpacing;
	}
	if (!myCenterElement.isNull() && myCenterElement->isVisible()) {
		myCenterElement->setPosition(centerElementX, centerElementY, Size(elementWidth, centerElementHeight));
	}
}

void W32VBorderBox::init(HWND parent, W32ControlCollection *collection) {
	if (!myTopElement.isNull()) {
		myTopElement->init(parent, collection);
	}
	if (!myCenterElement.isNull()) {
		myCenterElement->init(parent, collection);
	}
	if (!myBottomElement.isNull()) {
		myBottomElement->init(parent, collection);
	}
}
