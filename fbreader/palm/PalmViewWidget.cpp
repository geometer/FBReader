/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "PalmFBReader.h"
#include "PalmViewWidget.h"
#include "PalmPaintContext.h"

PalmViewWidget::PalmViewWidget() {
}

/*
void QViewWidget::paintEvent(QPaintEvent*) {
	((QPaintContext&)view()->context()).setSize(width(), height());
	view()->paint();
	QPainter realPainter(this);
	realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
}

void QViewWidget::mousePressEvent(QMouseEvent *event) {
	view()->onStylusPress(
		event->x() - view()->context().leftMargin().value(),
		event->y() - view()->context().topMargin().value());
}
*/

void PalmViewWidget::repaintView()	{
	if (!DO_PAINT) {
		return;
	}
	static const UInt32 RomVersion50 = sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0);
	static UInt32 romVersion = 0;
	if (romVersion == 0) {
		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	}
	
	FrmDrawForm(FrmGetActiveForm());

	RectangleType bufferBounds;
	bufferBounds.topLeft.x = 0;
	bufferBounds.topLeft.y = 0;
	if (romVersion >= RomVersion50) {
		WinSetCoordinateSystem(kCoordinatesNative);
		bufferBounds.extent.x = 320;
		bufferBounds.extent.y = 320;
	} else {
		bufferBounds.extent.x = 160;
		bufferBounds.extent.y = 160;
	}
	((PalmPaintContext&)view()->context()).setSize(bufferBounds.extent.x, bufferBounds.extent.y);

	Err err;
	WinHandle screenWindow = WinGetDrawWindow();
	WinHandle bufferWindow = WinCreateOffscreenWindow(bufferBounds.extent.x, bufferBounds.extent.y, nativeFormat, &err);

	if (err == errNone) {
		WinSetDrawWindow(bufferWindow);
	}
	view()->paint();
	if (err == errNone) {
		WinSetDrawWindow(screenWindow);
		WinCopyRectangle(bufferWindow, screenWindow, &bufferBounds, 0, 0, winPaint);
		WinDeleteWindow(bufferWindow, false);
	}

	if (romVersion >= RomVersion50) {
		WinSetCoordinateSystem(kCoordinatesStandard);
	}
	//repaint(false);
	//((QFBReader*)parent())->enableMenuButtons();
}
