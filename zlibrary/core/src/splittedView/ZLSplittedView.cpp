/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

ZLSplittedView::ZLSplittedView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : ZLView(application, context), myMoveSplitter(false) {
}

ZLSplittedView::~ZLSplittedView() {
}

int ZLSplittedView::partWidth(bool left) const {
	int w = context().width();
	int leftW = (w - splitterWidth()) * leftRatioN() / leftRatioD();
	return left ? leftW : w - leftW - splitterWidth() - 1;
}

void ZLSplittedView::paint() {
	if (!myLeftView.isNull()) { 
		myLeftView->paint();
	}

	if (!myLeftView.isNull() && !myRightView.isNull()) { 
		context().setFillColor(splitterColor());
		const int leftW = partWidth(true);
		context().fillRectangle(leftW + 1, 0, leftW + splitterWidth(), context().height());
	}

	if (!myRightView.isNull()) { 
		myRightView->paint();
	}
}

void ZLSplittedView::setLeftView(shared_ptr<ZLView> view) {
	myLeftView = view;
	if (!myLeftView.isNull() && hasContext()) { 
		myLeftView->setPaintContext(new ZLPartialPaintContext(*this, true));
	}
}

void ZLSplittedView::setRightView(shared_ptr<ZLView> view) {
	myRightView = view;
	if (!myRightView.isNull() && hasContext()) { 
		myRightView->setPaintContext(new ZLPartialPaintContext(*this, false));
	}
}

bool ZLSplittedView::onStylusPress(int x, int y) {
	int leftW = partWidth(true);

	if ((x < leftW) && !myLeftView.isNull()) {
		return myLeftView->onStylusPress(x, y);
	}

	if ((x > leftW + splitterWidth() + 1) && !myRightView.isNull()) {
		return myRightView->onStylusPress(x - leftW - splitterWidth() - 1, y);
	} 

	myMoveSplitter = true;
	return true;
}

bool ZLSplittedView::onStylusRelease(int x, int y) {
	myMoveSplitter = false;

	int leftW = partWidth(true);

	if ((x < leftW) && !myLeftView.isNull()) {
		return myLeftView->onStylusRelease(x, y);
	}

	if ((x > leftW + splitterWidth() + 1) && !myRightView.isNull()) {
		return myRightView->onStylusRelease(x - leftW - splitterWidth() - 1, y);
	} 

	return false;
}

bool ZLSplittedView::onStylusMove(int x, int y) {
	int leftW = partWidth(true);

	if ((x < leftW) && !myLeftView.isNull()) {
		return myLeftView->onStylusMove(x, y);
	}

	if ((x > leftW + splitterWidth() + 1) && !myRightView.isNull()) {
		return myRightView->onStylusMove(x - leftW - splitterWidth() - 1, y);
	} 

	return false;
}

bool ZLSplittedView::onStylusMovePressed(int x, int y) {
	if (myMoveSplitter) {
		moveSplitterTo(x);
		return true;
	}

	int leftW = partWidth(true);

	if ((x < leftW) && !myLeftView.isNull()) {
		return myLeftView->onStylusMovePressed(x, y);
	}

	if ((x > leftW + splitterWidth() + 1) && !myRightView.isNull()) {
		return myRightView->onStylusMovePressed(x - leftW - splitterWidth() - 1, y);
	} 

	return false;
}

bool ZLSplittedView::onFingerTap(int x, int y) {
	int leftW = partWidth(true);

	if ((x < leftW) && !myLeftView.isNull()) {
		return myLeftView->onFingerTap(x, y);
	}

	if ((x > leftW + splitterWidth() + 1) && !myRightView.isNull()) {
		return myRightView->onFingerTap(x - leftW - splitterWidth() - 1, y);
	} 

	return false;
}
