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

#include <iostream>
#include <algorithm>

#include "W32Element.h"

W32HPair::W32HPair(W32ElementPtr left, W32ElementPtr right, short leftPartPercent, short rightPartPercent) : myLeft(left), myRight(right), myLeftPartPercent(leftPartPercent), myRightPartPercent(rightPartPercent) {
	if (myLeftPartPercent < 0) {
		myLeftPartPercent = 0;
	} else if (myLeftPartPercent > 100) {
		myLeftPartPercent = 100;
	}
	if (myRightPartPercent < 0) {
		myRightPartPercent = 0;
	} else if (myLeftPartPercent + myRightPartPercent > 100) {
		myRightPartPercent = 100 - myLeftPartPercent;
	}
}

void W32HPair::allocate(WORD *&p, short &id) const {
	if (!myLeft.isNull()) {
		myLeft->allocate(p, id);
	}
	if (!myRight.isNull()) {
		myRight->allocate(p, id);
	}
}

int W32HPair::allocationSize() const {
	int size = 0;
	if (!myLeft.isNull()) {
		size += myLeft->allocationSize();
	}
	if (!myRight.isNull()) {
		size += myRight->allocationSize();
	}
	return size;
}

void W32HPair::setVisible(bool visible) {
	if (!myLeft.isNull()) {
		myLeft->setVisible(visible);
	}
	if (!myRight.isNull()) {
		myRight->setVisible(visible);
	}
}

bool W32HPair::isVisible() const {
	return
		(!myLeft.isNull() && myLeft->isVisible()) ||
		(!myRight.isNull() && myRight->isVisible());
}

int W32HPair::controlNumber() const {
	int counter = 0;
	if (!myLeft.isNull()) {
		counter += myLeft->controlNumber();
	}
	if (!myRight.isNull()) {
		counter += myRight->controlNumber();
	}
	return counter;
}

W32Element::Size W32HPair::minimumSize() const {
	if ((myLeftPartPercent == 0) && (myRightPartPercent == 0)) {
		return Size();
	}

	Size leftSize, rightSize;
	if (!myLeft.isNull()) {
		leftSize = myLeft->minimumSize();
	}
	if (!myRight.isNull()) {
		rightSize = myRight->minimumSize();
	}

	Size size;
	if ((myRightPartPercent == 0) ||
			(leftSize.Width * myRightPartPercent > rightSize.Width * myLeftPartPercent)) {
		size.Width = leftSize.Width * 100 / myLeftPartPercent;
	} else {
		size.Width = rightSize.Width * 100 / myRightPartPercent;
	}
	size.Height = std::max(leftSize.Height, rightSize.Height);
	return size;
}

void W32HPair::setPosition(int x, int y, Size size) {
	if (!myLeft.isNull()) {
		myLeft->setPosition(x, y, Size(size.Width * myLeftPartPercent / 100, size.Height));
	}
	if (!myRight.isNull()) {
		myRight->setPosition(x + size.Width * (100 - myRightPartPercent) / 100, y, Size(size.Width * myRightPartPercent / 100, size.Height));
	}
}

void W32HPair::setDimensions(Size charDimension) {
	if (!myLeft.isNull()) {
		myLeft->setDimensions(charDimension);
	}
	if (!myRight.isNull()) {
		myRight->setDimensions(charDimension);
	}
}

void W32HPair::init(HWND parent, short &id) {
	if (!myLeft.isNull()) {
		myLeft->init(parent, id);
	}
	if (!myRight.isNull()) {
		myRight->init(parent, id);
	}
}
