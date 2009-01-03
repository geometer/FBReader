/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "ZLApplication.h"
#include "../view/ZLView.h"
#include "../view/ZLViewWidget.h"

ZLApplication::RotationAction::RotationAction(ZLApplication &application) : myApplication(application) {
}

bool ZLApplication::RotationAction::isVisible() const {
	return (myApplication.myViewWidget != 0) &&
				 ((myApplication.RotationAngleOption.value() != ZLView::DEGREES0) ||
					(myApplication.myViewWidget->rotation() != ZLView::DEGREES0));
}

void ZLApplication::RotationAction::run() {
	int optionValue = myApplication.RotationAngleOption.value();
	ZLView::Angle oldAngle = myApplication.myViewWidget->rotation();
	ZLView::Angle newAngle = ZLView::DEGREES0;
	if (optionValue == -1) {
		switch (oldAngle) {
			case ZLView::DEGREES0:
				newAngle = ZLView::DEGREES90;
				break;
			case ZLView::DEGREES90:
				newAngle = ZLView::DEGREES180;
				break;
			case ZLView::DEGREES180:
				newAngle = ZLView::DEGREES270;
				break;
			case ZLView::DEGREES270:
				newAngle = ZLView::DEGREES0;
				break;
		}
	} else {
		newAngle = (oldAngle == ZLView::DEGREES0) ?
			(ZLView::Angle)optionValue : ZLView::DEGREES0;
	}
	myApplication.myViewWidget->rotate(newAngle);
	myApplication.AngleStateOption.setValue(newAngle);
	myApplication.refreshWindow();
}

ZLApplication::FullscreenAction::FullscreenAction(ZLApplication &application) : myApplication(application) {
}

void ZLApplication::FullscreenAction::run() {
	myApplication.setFullscreen(!myApplication.isFullscreen());
}
