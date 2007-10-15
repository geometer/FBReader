/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "ZLSplittedView.h"
#include "ZLPartialPaintContext.h"
#include "../application/ZLApplication.h"

ZLSplittedView::ZLSplittedView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : ZLView(application, context) {
}

ZLSplittedView::~ZLSplittedView() {
}

void ZLSplittedView::setPaintContext(shared_ptr<ZLPaintContext> context) {
	ZLView::setPaintContext(context);
	if (hasContext()) {
		if (!myLeftView.isNull()) { 
			myLeftView->setPaintContext(new ZLPartialPaintContext(this->context(), true));
		}
		if (!myRightView.isNull()) { 
			myRightView->setPaintContext(new ZLPartialPaintContext(this->context(), false));
		}
	}
}

void ZLSplittedView::paint() {
	if (!myLeftView.isNull()) { 
		myLeftView->paint();
	}
	if (!myRightView.isNull()) { 
		myRightView->paint();
	}
}

void ZLSplittedView::setLeftView(shared_ptr<ZLView> view) {
	myLeftView = view;
	if (!myLeftView.isNull() && hasContext()) { 
		myLeftView->setPaintContext(new ZLPartialPaintContext(context(), true));
	}
}

void ZLSplittedView::setRightView(shared_ptr<ZLView> view) {
	myRightView = view;
	if (!myRightView.isNull() && hasContext()) { 
		myRightView->setPaintContext(new ZLPartialPaintContext(context(), false));
	}
}

bool ZLSplittedView::onStylusPress(int x, int y) {
	return false;
}

bool ZLSplittedView::onStylusRelease(int, int) {
	return false;
}

bool ZLSplittedView::onStylusMove(int, int) {
	return false;
}

bool ZLSplittedView::onStylusMovePressed(int, int) {
	return false;
}

bool ZLSplittedView::onFingerTap(int, int) {
	return false;
}
