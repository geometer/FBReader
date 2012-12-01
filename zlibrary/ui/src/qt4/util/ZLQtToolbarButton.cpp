/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLFile.h>

#include "ZLQtToolbarButton.h"

ZLQtToolbarButton::ZLQtToolbarButton(const std::string &iconName, QWidget *parent) : QPushButton(parent) {
	static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	myIcon = QPixmap(ZLFile(imagePrefix + iconName + ".png").path().c_str());
	setIconSize(myIcon.size());
	setIcon(myIcon);
	setAutoDefault(false);
	setFocusPolicy(Qt::NoFocus);
	// see https://bugreports.qt-project.org/browse/QTBUG-14591
  setAttribute(Qt::WA_LayoutUsesWidgetRect);
}
