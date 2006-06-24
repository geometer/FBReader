/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005, 2006 Mikhail Sobolev <mss@mawhrin.net>
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

static const std::string ANGLE = "Angle";
static const std::string STATE = "State";

ZLApplication::ZLApplication(const std::string &name) :
	RotationAngleOption(ZLOption::CONFIG_CATEGORY, "Rotation", ANGLE, ZLViewWidget::DEGREES90),
	AngleStateOption(ZLOption::CONFIG_CATEGORY, STATE, ANGLE, ZLViewWidget::DEGREES0),
	myName(name),
	myViewWidget(0),
	myWindow(0) {
}

void ZLApplication::initWindow(ZLApplicationWindow *view) {
	myWindow = view;
	myWindow->myApplication = this;
	myWindow->init();
	myWindow->refresh();
	resetWindowCaption();
}

void ZLApplicationWindow::init() {
	const ZLApplication::Toolbar::ItemVector &toolbarItems = myApplication->toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it) {
		addToolbarItem(*it);
	}

	const ZLApplication::Menubar::ItemVector &menubarItems = myApplication->menubar().items();
	for (ZLApplication::Menubar::ItemVector::const_iterator it = menubarItems.begin(); it != menubarItems.end(); ++it) {
		addMenubarItem(*it);
	}
}

void ZLApplication::addAction(int actionId, shared_ptr<Action> action) {
	myActionMap[actionId] = action;
}

void ZLApplication::setView(ZLView *view) {
	if (myViewWidget != 0) {
		myViewWidget->setView(view);
	}
}

ZLView *ZLApplication::currentView() {
	return (myViewWidget != 0) ? myViewWidget->view() : 0;
}

void ZLApplication::repaintView() {
	if (myViewWidget != 0) {
		myViewWidget->repaintView();
	}
}

bool ZLApplication::isActionVisible(int actionId) const {
	const std::map<int,shared_ptr<Action> >::const_iterator it = myActionMap.find(actionId);
	return (it != myActionMap.end()) && it->second->isVisible();
}

bool ZLApplication::isActionEnabled(int actionId) const {
	const std::map<int,shared_ptr<Action> >::const_iterator it = myActionMap.find(actionId);
	return (it != myActionMap.end()) && it->second->isEnabled();
}

void ZLApplication::doAction(int actionId) {
	std::map<int,shared_ptr<Action> >::const_iterator it = myActionMap.find(actionId);
	if (it != myActionMap.end() && it->second->isEnabled()) {
		it->second->run();
	}
}

void ZLApplication::resetWindowCaption() {
	if (myWindow != 0) {
		if ((currentView() == 0) || (currentView()->caption().empty())) {
			myWindow->setCaption(myName);
		} else {
			myWindow->setCaption(myName + " - " + currentView()->caption());
		}
	}
}

void ZLApplication::Toolbar::addButton(int actionId, const std::string &iconName) {
	myItems.push_back(new ButtonItem(actionId, iconName));
}

void ZLApplication::Toolbar::addSeparator() {
	myItems.push_back(new SeparatorItem());
}

void ZLApplication::Menu::addItem(const std::string &itemName, int actionId) {
	ItemPtr item = new Menubar::MenuItem(itemName, actionId);
	myItems.push_back(item);
}

void ZLApplication::Menu::addSeparator() {
	ItemPtr item = new Menubar::SeparatorItem();
	myItems.push_back(item);
}

ZLApplication::Menu::Menu &ZLApplication::Menu::addSubmenu(const std::string &menuName) {
	Menubar::SubMenuItem *submenu = new Menubar::SubMenuItem(menuName);
	ItemPtr item = submenu;
	myItems.push_back(item);

	return *submenu;
}

ZLApplication::Action::~Action() {
}

bool ZLApplication::Action::isVisible() {
	return true;
}

bool ZLApplication::Action::isEnabled() {
	return isVisible();
}

// vim:noet:ts=2:sw=2
