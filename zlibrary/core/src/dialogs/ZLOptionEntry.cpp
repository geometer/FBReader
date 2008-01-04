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

#include "ZLOptionEntry.h"
#include "ZLOptionView.h"

void ZLOptionEntry::resetView() {
	if (myView != 0) {
		myView->reset();
	}
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

void ZLComboOptionEntry::onStringValueSelected(const char *value) {
	onStringValueSelected((value != 0) ? std::string(value) : std::string());
}

void ZLComboOptionEntry::onValueEdited(const char *value) {
	onValueEdited((value != 0) ? std::string(value) : std::string());
}

void ZLComboOptionEntry::onAccept(const char *value) {
	onAccept((value != 0) ? std::string(value) : std::string());
}

void ZLTextOptionEntry::onValueEdited(const char *value) {
	onValueEdited((value != 0) ? std::string(value) : std::string());
}

void ZLTextOptionEntry::onAccept(const char *value) {
	onAccept((value != 0) ? std::string(value) : std::string());
}
