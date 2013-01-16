/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

#include "ZLQtGeometryOptions.h"

static const std::string OPTIONS = "Options";

ZLQtGeometryOptions::ZLQtGeometryOptions(const std::string &prefix) :
	myX(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, prefix + "XPosition", 0, QApplication::desktop()->width() - 10, 10),
	myY(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, prefix + "YPosition", 0, QApplication::desktop()->height() - 10, 10),
	myWidth(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, prefix + "Width", 10, QApplication::desktop()->width(), QApplication::desktop()->width() / 2),
	myHeight(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, prefix + "Height", 10, QApplication::desktop()->height(), QApplication::desktop()->height() / 2)
  {
}

void ZLQtGeometryOptions::getFromWidget(const QWidget &widget) {
	QPoint position = widget.pos();
	if (position.x() != -1) {
		myX.setValue(position.x());
	}
	if (position.y() != -1) {
		myY.setValue(position.y());
	}
	myWidth.setValue(widget.width());
	myHeight.setValue(widget.height());
}

void ZLQtGeometryOptions::setToWidget(QWidget &widget) const {
	const int x = myX.value();
	const int y = myY.value();
	const int w = myWidth.value();
	const int h = myHeight.value();
	widget.setGeometry(x, y, w, h);
	QPoint position = widget.pos();
	const int deltaX = x - position.x();
	const int deltaY = y - position.y();
	const int deltaW = w - widget.width();
	const int deltaH = h - widget.height();
	widget.setGeometry(x + deltaX, y + deltaY, w + deltaW, h + deltaH);
}
