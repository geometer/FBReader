/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLApplication.h>

#include "ZLTextSelectionScroller.h"
#include "ZLTextView.h"
#include "../area/ZLTextSelectionModel.h"

ZLTextSelectionScroller::ZLTextSelectionScroller(ZLTextView &view) : myView(view), myDirection(DONT_SCROLL) {
}

void ZLTextSelectionScroller::setDirection(Direction direction) {
	myDirection = direction;
}

void ZLTextSelectionScroller::run() {
	if (myDirection != DONT_SCROLL) {
		myView.scrollPage(myDirection == SCROLL_FORWARD, ZLTextAreaController::SCROLL_LINES, 1);
		myView.selectionModel().invalidate();
		ZLApplication::Instance().refreshWindow();
	}
}
