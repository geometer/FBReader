/*
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

#include "PalmViewWidget.h"
#include "PalmPaintContext.h"

PalmViewWidget::PalmViewWidget(UInt16 formId) : myFormId(formId), myScreenWindow(0), myBufferWindow(0) {
}

PalmViewWidget::~PalmViewWidget() {
	if (myBufferWindow != 0) {
		WinDeleteWindow(myBufferWindow, false);
	}
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

void PalmViewWidget::repaintView() {
	//FrmUpdateForm(myFormId, frmRedrawUpdateCode);
	paintEvent();
}

void PalmViewWidget::paintEvent()	{
	if (!DO_PAINT) {
		return;
	}
	static const UInt32 RomVersion50 = sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0);
	static UInt32 romVersion = 0;
	if (romVersion == 0) {
		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	}
	
	if (myScreenWindow == 0) { 
		FrmDrawForm(FrmGetFormPtr(myFormId));
	}

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

	if (myScreenWindow == 0) { 
		myScreenWindow = WinGetDrawWindow();
		Err err;
		myBufferWindow = WinCreateOffscreenWindow(bufferBounds.extent.x, bufferBounds.extent.y, nativeFormat, &err);
		if (err != errNone) {
			myBufferWindow = 0;
		}
	}

	((PalmPaintContext&)view()->context()).setSize(bufferBounds.extent.x, bufferBounds.extent.y);

	if (myBufferWindow != 0) {
		WinSetDrawWindow(myBufferWindow);
	}
	view()->paint();
	if (myBufferWindow != 0) {
		WinSetDrawWindow(myBufferWindow);
		WinCopyRectangle(myBufferWindow, myScreenWindow, &bufferBounds, 0, 0, winPaint);
	}

	if (romVersion >= RomVersion50) {
		WinSetCoordinateSystem(kCoordinatesStandard);
	}
	//repaint(false);
	//((ZLApplication*)parent())->enableMenuButtons();
}
