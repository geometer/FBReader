/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLQtPopupMenu.h"

ZLQtPopupMenu::ZLQtPopupMenu(QWidget *parent) : QPopupMenu(parent) {
	connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
}

void ZLQtPopupMenu::onActivated(int itemId) {
	if (!myPopupData.isNull()) {
		myPopupData->run(myItemIdToIndex[itemId]);
	}
}

void ZLQtPopupMenu::reset(shared_ptr<ZLPopupData> popupData) {
	clear();
	myItemIdToIndex.clear();
	myPopupData = popupData;
	if (!popupData.isNull()) {
		const size_t count = popupData->count();
		for (size_t i = 0; i < count; ++i) {
			myItemIdToIndex[insertItem(QString::fromUtf8(popupData->text(i).c_str()))] = i;
		}
	}
}
