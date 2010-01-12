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

#include <ZLXMLReader.h>
#include <ZLResource.h>
#include <ZLibrary.h>

#include "ZLApplication.h"
#include "ZLMenu.h"

class ZLMenubarCreator : public ZLXMLReader {

public:
	ZLMenubarCreator(ZLMenubar &menubar);
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
	ZLMenu &myMenubar;
	std::vector<ZLMenu::ItemPtr> mySubmenuStack;
};

void ZLApplication::createMenubar() {
	menubar();
	ZLMenubarCreator(*myMenubar).readDocument(ZLibrary::DefaultFilesPathPrefix() + "menubar.xml");
}

ZLMenubarCreator::ZLMenubarCreator(ZLMenubar &menubar) : myMenubar(menubar) {
}

static const std::string ITEM = "item";
static const std::string SUBMENU = "submenu";

void ZLMenubarCreator::startElementHandler(const char *tag, const char **attributes) {
	ZLMenu &menu =
		mySubmenuStack.empty() ?  myMenubar : (ZLMenubar::Submenu&)*mySubmenuStack.back();
	if (ITEM == tag) {
		const char *id = attributeValue(attributes, "id");
		if (id != 0) {
			const std::string sid = id;
			menu.addItem(sid, ZLResourceKey(sid));
		}
	} else if (SUBMENU == tag) {
		const char *id = attributeValue(attributes, "id");
		if (id != 0) {
			mySubmenuStack.push_back(menu.addSubmenu(ZLResourceKey(id)));
		}
	}
}

void ZLMenubarCreator::endElementHandler(const char *tag) {
	if (SUBMENU == tag) {
		if (!mySubmenuStack.empty()) {
			mySubmenuStack.pop_back();
		}
	}
}

ZLMenu::ZLMenu(const ZLResource &resource) : myResource(resource) {
}

ZLMenu::~ZLMenu() {
}

const ZLMenu::ItemVector &ZLMenu::items() const {
	return myItems;
}

ZLMenu::Item::Item(ItemType type): myType(type) {
}

ZLMenu::Item::~Item() {
}

ZLMenu::Item::ItemType ZLMenubar::Item::type() const {
	return myType;
}

void ZLMenu::addItem(const std::string &actionId, const ZLResourceKey &key) {
	myItems.push_back(new ZLMenubar::PlainItem(myResource[key].value(), actionId));
}

void ZLMenu::addSeparator() {
	myItems.push_back(new ZLMenubar::Separator());
}

ZLMenu::ItemPtr ZLMenu::addSubmenu(const ZLResourceKey &key) {
	ItemPtr submenu = new ZLMenubar::Submenu(myResource[key]);
	myItems.push_back(submenu);
	return submenu;
}


ZLMenubar::ZLMenubar() : ZLMenu(ZLResource::resource("menu")) {
}

ZLMenubar::PlainItem::PlainItem(const std::string& name, const std::string &actionId) : Item(ITEM), myName(name), myActionId(actionId) {
}

const std::string& ZLMenubar::PlainItem::name() const {
	return myName;
}

const std::string &ZLMenubar::PlainItem::actionId() const {
	return myActionId;
}

ZLMenubar::Submenu::Submenu(const ZLResource &resource) : ZLMenu::Item(SUBMENU), ZLMenu(resource) {
}

const std::string &ZLMenubar::Submenu::menuName() const {
	return myResource.value();
}

ZLMenubar::Separator::Separator() : Item(SEPARATOR) {
}

ZLMenuVisitor::~ZLMenuVisitor() {
}

void ZLMenuVisitor::processMenu(ZLApplication &application) {
	if (!application.myMenubar.isNull()) {
		processMenu(*application.myMenubar);
	}
}

void ZLMenuVisitor::processMenu(ZLMenu &menu) {
	const ZLMenu::ItemVector &items = menu.items();
	for (ZLMenu::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		switch ((*it)->type()) {
			case ZLMenu::Item::ITEM:
				processItem((ZLMenubar::PlainItem&)**it);
				break;
			case ZLMenu::Item::SUBMENU:
			{
				ZLMenubar::Submenu &submenu = (ZLMenubar::Submenu&)**it;
				processSubmenuBeforeItems(submenu);
				processMenu(submenu);
				processSubmenuAfterItems(submenu);
				break;
			}
			case ZLMenu::Item::SEPARATOR:
				processSepartor((ZLMenubar::Separator&)**it);
				break;
		}							
	}
}

const ZLMenubar &ZLApplication::menubar() const {
	if (myMenubar.isNull()) {
		myMenubar = new ZLMenubar();
	}
	return *myMenubar;
}
