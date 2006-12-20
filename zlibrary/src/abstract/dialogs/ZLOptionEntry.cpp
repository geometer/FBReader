/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "ZLOptionEntry.h"

ZLOptionView::ZLOptionView(ZLOptionEntry *option) : myOption(option) {
	myOption->setView(this);
}

ZLOptionView::~ZLOptionView() {
	delete myOption;
}

void ZLOptionEntry::setVisible(bool visible) {
	myIsVisible = visible;
	if (myView != 0) {
		myView->setVisible(visible);
	}
}

void ZLOptionEntry::setActive(bool active) {
	myIsActive = active;
	if (myView != 0) {
		myView->setActive(active);
	}
}

void ZLComboOptionEntry::onStringValueSelected(const std::string &value) {
	const std::vector<std::string> valuesVector = values();
	int index = 0;
	for (std::vector<std::string>::const_iterator it = valuesVector.begin(); it != valuesVector.end(); ++it, ++index) {
		if (value == *it) {
			onValueSelected(index);
			break;
		}
	}
}
