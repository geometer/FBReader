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

#ifndef __ZLTOOLBAR_H__
#define __ZLTOOLBAR_H__

#include <string>
#include <vector>

#include <shared_ptr.h>
#include <ZLOptions.h>

class ZLResource;
class ZLPopupData;

class ZLToolbar {

public:
	class Item {

	public:
		Item(ZLToolbar &toolbar);
		virtual ~Item();

		virtual bool isSeparator() const = 0;
	};

	class ButtonItem : public Item {

	public:
		ButtonItem(ZLToolbar &toolbar, const std::string &actionId);

		bool isSeparator() const;

		const std::string &actionId() const;
		const std::string &label() const;
		const std::string &tooltip() const;
		const std::string &iconName() const;

	private:
		const ZLResource &myResource;
		const std::string myActionId;
	};

	class SeparatorItem : public Item {

	public:
		SeparatorItem(ZLToolbar &toolbar);
		bool isSeparator() const;
	};

public:
	typedef shared_ptr<Item> ItemPtr;
	typedef std::vector<ItemPtr> ItemVector;

	ZLToolbar(const ZLApplication &application);

	const ItemVector &items() const;
	static const ZLResource &resource(const std::string &id);

private:
	const ZLApplication &myApplication;
	ItemVector myItems;

friend class ZLApplication;
friend class ZLToolbarCreator;
};

#endif /* __ZLTOOLBAR_H__ */
