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

#ifndef __ZLMENU_H__
#define __ZLMENU_H__

#include <string>
#include <vector>

#include <shared_ptr.h>
#include <ZLResource.h>

class ZLMenu {

public:
	class Item {

	public:
		enum ItemType {
			ITEM,
			SUBMENU,
			SEPARATOR
		};
		
	public:
		virtual ~Item();
		ItemType type() const;

	protected:
		Item(ItemType type);

	private:
		const ItemType myType;
	};

	typedef shared_ptr<Item> ItemPtr;
	typedef std::vector<ItemPtr> ItemVector;

	void addItem(const std::string &actionId, const ZLResourceKey &key);
	void addSeparator();
	ItemPtr addSubmenu(const ZLResourceKey &key);

	const ItemVector &items() const;

	virtual ~ZLMenu();

protected:
	ZLMenu(const ZLResource &resource);

protected:
	const ZLResource &myResource;

private:
	ItemVector myItems;

friend class ZLApplication;
};

class ZLMenubar : public ZLMenu {

public:
	class PlainItem : public ZLMenu::Item {

	public:
		PlainItem(const std::string &name, const std::string &actionId);

		const std::string &name() const;
		const std::string &actionId() const;

	private:
		const std::string myName;
		const std::string myActionId;
	};

	class Submenu : public ZLMenu::Item, public ZLMenu {

	public:
		Submenu(const ZLResource &resource);

		const std::string &menuName() const;
	};

	class Separator : public ZLMenu::Item {

	public:
		Separator();
	};

public:
	ZLMenubar();

friend class ZLApplication;
};

class ZLMenuVisitor {

public:
	virtual ~ZLMenuVisitor();
	void processMenu(ZLApplication &application);

private:
	void processMenu(ZLMenu &menu);

protected:
	virtual void processSubmenuBeforeItems(ZLMenubar::Submenu &submenu) = 0;
	virtual void processSubmenuAfterItems(ZLMenubar::Submenu &submenu) = 0;
	virtual void processItem(ZLMenubar::PlainItem &item) = 0;
	virtual void processSepartor(ZLMenubar::Separator &separator) = 0;
};

#endif /* __ZLMENU_H__ */
