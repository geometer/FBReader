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

#include "ZLApplication.h"

void ZLApplicationWindow::init() {
	const ZLApplication::Toolbar::ItemVector &items = myApplication->toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		addToolbarItem(*it);
	}
}

void ZLApplication::setActionVisible(int actionId, bool visible) {
	toolbar().setActionVisible(actionId, visible);
}

void ZLApplication::setActionEnabled(int actionId, bool enabled) {
	toolbar().setActionEnabled(actionId, enabled);
}

void ZLApplication::addAction(int actionId, shared_ptr<Action> action) {
	myActionMap[actionId] = action;
}

ZLApplication::Toolbar::Toolbar() : myVisibilityChanged(true) {
}

void ZLApplication::Toolbar::reset() {
	myVisibilityChanged = false;
}

void ZLApplication::Toolbar::addButton(int actionId, const std::string &iconName) {
	ItemPtr item = new ButtonItem(actionId, iconName);
	myItems.push_back(item);
	myItemsById[actionId] = item;
}

void ZLApplication::Toolbar::addSeparator() {
	myItems.push_back(new SeparatorItem());
}

void ZLApplication::Toolbar::setActionVisible(int actionId, bool visible) {
	std::map<int,ItemPtr>::iterator it = myItemsById.find(actionId);
	if ((it != myItemsById.end()) && (it->second->isVisible() != visible)) {
		myVisibilityChanged = true;
		it->second->setVisible(visible);
	}
}

void ZLApplication::Toolbar::setActionEnabled(int actionId, bool enabled) {
	std::map<int,ItemPtr>::iterator it = myItemsById.find(actionId);
	if (it != myItemsById.end()) {
		((ButtonItem&)*it->second).setEnabled(enabled);
	}
}

ZLApplication::Action::Action(ZLApplication &application) : myApplication(application) {
}

ZLApplication::Action::~Action() {
}

bool ZLApplication::Action::isVisible() {
	return true;
}

bool ZLApplication::Action::isEnabled() {
	return isVisible();
}
