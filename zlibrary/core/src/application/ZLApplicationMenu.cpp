/*
 * Copyright (C) 2004-2007 Geometer Plus <contact@geometerplus.com>
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

ZLApplication::Menu::Menu(const ZLResource &resource) : myResource(resource) {
}

ZLApplication::Menu::~Menu() {
}

const ZLApplication::Menu::ItemVector &ZLApplication::Menu::items() const {
	return myItems;
}

ZLApplication::Menu::Item::Item(ItemType type): myType(type) {
}

ZLApplication::Menu::Item::~Item() {
}

ZLApplication::Menu::Item::ItemType ZLApplication::Menubar::Item::type() const {
	return myType;
}

void ZLApplication::Menu::addItem(int actionId, const ZLResourceKey &key) {
	myItems.push_back(new Menubar::PlainItem(myResource[key].value(), actionId));
}

void ZLApplication::Menu::addSeparator() {
	myItems.push_back(new Menubar::Separator());
}

ZLApplication::Menu::Menu &ZLApplication::Menu::addSubmenu(const ZLResourceKey &key) {
	Menubar::Submenu *submenu = new Menubar::Submenu(myResource[key]);
	myItems.push_back(submenu);
	return *submenu;
}


ZLApplication::Menubar::Menubar() : Menu(ZLResource::resource("menu")) {
}

ZLApplication::Menubar::PlainItem::PlainItem(const std::string& name, int actionId) : Item(ITEM), myName(name), myActionId(actionId) {
}

const std::string& ZLApplication::Menubar::PlainItem::name() const {
	return myName;
}

int ZLApplication::Menubar::PlainItem::actionId() const {
	return myActionId;
}

ZLApplication::Menubar::Submenu::Submenu(const ZLResource &resource) : Menu::Item(SUBMENU), Menu(resource) {
}

const std::string &ZLApplication::Menubar::Submenu::menuName() const {
	return myResource.value();
}

ZLApplication::Menubar::Separator::Separator() : Item(SEPARATOR) {
}

ZLApplication::MenuVisitor::~MenuVisitor() {
}

void ZLApplication::MenuVisitor::processMenu(ZLApplication::Menu &menu) {
	const ZLApplication::Menu::ItemVector &items = menu.items();
	for (ZLApplication::Menu::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		switch ((*it)->type()) {
			case ZLApplication::Menu::Item::ITEM:
				processItem((ZLApplication::Menubar::PlainItem&)**it);
				break;
			case ZLApplication::Menu::Item::SUBMENU:
			{
				ZLApplication::Menubar::Submenu &submenu = (ZLApplication::Menubar::Submenu&)**it;
				processSubmenuBeforeItems(submenu);
				processMenu(submenu);
				processSubmenuAfterItems(submenu);
				break;
			}
			case ZLApplication::Menu::Item::SEPARATOR:
				processSepartor((ZLApplication::Menubar::Separator&)**it);
				break;
		}							
	}
}
