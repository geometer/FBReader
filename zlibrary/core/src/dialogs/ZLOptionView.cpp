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

#include "ZLOptionView.h"
#include "ZLOptionEntry.h"

ZLOptionView::ZLOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option) : myName(name), myTooltip(tooltip), myOption(option), myInitialized(false) {
	myOption->setView(this);
}

ZLOptionView::~ZLOptionView() {
}

const std::string &ZLOptionView::name() const {
	return myName;
}

const std::string &ZLOptionView::tooltip() const {
	return myTooltip;
}

void ZLOptionView::setVisible(bool visible) {
	if (visible) {
		if (!myInitialized) {
			_createItem();
			myInitialized = true;
		}
		setActive(myOption->isActive());
		_show();
	} else {
		if (myInitialized) _hide();
	}
}

void ZLOptionView::setActive(bool active) {
	if (myInitialized) {
		_setActive(active);
	}
}

void ZLOptionView::onAccept() const {
	if (myInitialized) {
		_onAccept();
	}
}
