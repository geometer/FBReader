/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

//#include <ZLibrary.h>
//#include <ZLLanguageUtil.h>

#include "ZLCocoaViewWidget.h"
#include "ZLCocoaPaintContext.h"
//#include "../util/ZLCocoaSignalUtil.h"

int ZLCocoaViewWidget::width() const {
	return 500;
}

int ZLCocoaViewWidget::height() const {
	return 500;
}

ZLCocoaViewWidget::ZLCocoaViewWidget(ZLApplication *application, ZLView::Angle initialAngle) : ZLViewWidget(initialAngle) {
}

ZLCocoaViewWidget::~ZLCocoaViewWidget() {
}

void ZLCocoaViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
}

void ZLCocoaViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
}

void ZLCocoaViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
}

void ZLCocoaViewWidget::trackStylus(bool track) {
}

void ZLCocoaViewWidget::repaint()	{
}

void ZLCocoaViewWidget::doPaint()	{
}
