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

#include "ZLView.h"
#include "../application/ZLApplication.h"

ZLView::ZLView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : myApplication(application), myContext(context) {
}

ZLView::~ZLView() {
}

void ZLView::setPaintContext(shared_ptr<ZLPaintContext> context) {
	myContext = context;
}

void ZLViewWidget::setView(shared_ptr<ZLView> view) {
	myView = view;
}

bool ZLView::onStylusPress(int, int) {
	return false;
}

bool ZLView::onStylusRelease(int, int) {
	return false;
}

bool ZLView::onStylusMove(int, int) {
	return false;
}

bool ZLView::onStylusMovePressed(int, int) {
	return false;
}

bool ZLView::onFingerTap(int, int) {
	return false;
}
