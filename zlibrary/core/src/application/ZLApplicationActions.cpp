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

#include "ZLApplication.h"
#include "../view/ZLView.h"
#include "../view/ZLViewWidget.h"

bool ZLApplication::RotationAction::isVisible() const {
	ZLApplication &application = ZLApplication::Instance();
	return (application.myViewWidget != 0) &&
				 ((application.RotationAngleOption.value() != ZLView::DEGREES0) ||
					(application.myViewWidget->rotation() != ZLView::DEGREES0));
}

void ZLApplication::RotationAction::run() {
	ZLApplication &application = ZLApplication::Instance();

	const int optionValue = application.RotationAngleOption.value();
	ZLView::Angle oldAngle = application.myViewWidget->rotation();
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
	application.myViewWidget->rotate(newAngle);
	application.AngleStateOption.setValue(newAngle);
	application.refreshWindow();
}

void ZLApplication::FullscreenAction::run() {
	ZLApplication &application = ZLApplication::Instance();
	application.setFullscreen(!application.isFullscreen());
}
