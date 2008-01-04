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

#include "ZLApplication.h"
#include "../view/ZLView.h"

ZLApplication::RotationAction::RotationAction(ZLApplication &application) : myApplication(application) {
}

bool ZLApplication::RotationAction::isVisible() {
	return (myApplication.myViewWidget != 0) &&
				 ((myApplication.RotationAngleOption.value() != ZLViewWidget::DEGREES0) ||
					(myApplication.myViewWidget->rotation() != ZLViewWidget::DEGREES0));
}

void ZLApplication::RotationAction::run() {
	int optionValue = myApplication.RotationAngleOption.value();
	ZLViewWidget::Angle oldAngle = myApplication.myViewWidget->rotation();
	ZLViewWidget::Angle newAngle = ZLViewWidget::DEGREES0;
	if (optionValue == -1) {
		switch (oldAngle) {
			case ZLViewWidget::DEGREES0:
				newAngle = ZLViewWidget::DEGREES90;
				break;
			case ZLViewWidget::DEGREES90:
				newAngle = ZLViewWidget::DEGREES180;
				break;
			case ZLViewWidget::DEGREES180:
				newAngle = ZLViewWidget::DEGREES270;
				break;
			case ZLViewWidget::DEGREES270:
				newAngle = ZLViewWidget::DEGREES0;
				break;
		}
	} else {
		newAngle = (oldAngle == ZLViewWidget::DEGREES0) ?
			(ZLViewWidget::Angle)optionValue : ZLViewWidget::DEGREES0;
	}
	myApplication.myViewWidget->rotate(newAngle);
	myApplication.AngleStateOption.setValue(newAngle);
	myApplication.refreshWindow();
}

ZLApplication::FullscreenAction::FullscreenAction(ZLApplication &application, bool toggle) : myApplication(application), myIsToggle(toggle) {
}

bool ZLApplication::FullscreenAction::isVisible() {
	return myIsToggle || !myApplication.isFullscreen();
}

void ZLApplication::FullscreenAction::run() {
	myApplication.setFullscreen(!myApplication.isFullscreen());
}
