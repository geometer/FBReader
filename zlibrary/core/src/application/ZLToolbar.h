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

#ifndef __ZLTOOLBAR_H__
#define __ZLTOOLBAR_H__

#include <string>
#include <vector>
#include <map>
#include <set>

#include <shared_ptr.h>
#include <ZLOptions.h>

class ZLResource;
class ZLPopupData;

class ZLToolbar {

public:
	class Item {

	public:
		enum Type {
			PLAIN_BUTTON,
			MENU_BUTTON,
			TOGGLE_BUTTON,
			TEXT_FIELD,
			COMBO_BOX,
			SEARCH_FIELD,
			SEPARATOR,
			FILL_SEPARATOR
		};

	public:
		Item(ZLToolbar &toolbar, Type type);
		virtual ~Item();

		Type type() const;
		const ZLToolbar &toolbar() const;

	private:
		const ZLToolbar &myToolbar;
		const Type myType;
	};

	class ActionItem : public Item {

	public:
		ActionItem(ZLToolbar &toolbar, Type type, const std::string &actionId);
		const std::string &actionId() const;
		const std::string &label() const;
		const std::string &tooltip() const;

	private:
		const std::string myActionId;

	protected:
		const ZLResource &myResource;
	};

	class AbstractButtonItem : public ActionItem {

	public:
		AbstractButtonItem(ZLToolbar &toolbar, Type type, const std::string &actionId);
		const std::string &iconName() const;
	};

	class PlainButtonItem : public AbstractButtonItem {

	public:
		PlainButtonItem(ZLToolbar &toolbar, const std::string &actionId);
	};

	class MenuButtonItem : public AbstractButtonItem {

	public:
		MenuButtonItem(ZLToolbar &toolbar, const std::string &actionId);

		const std::string &popupTooltip() const;
		shared_ptr<ZLPopupData> popupData() const;
	};

	class ButtonGroup;

	class ToggleButtonItem : public AbstractButtonItem {

	public:
		ToggleButtonItem(ZLToolbar &toolbar, const std::string &actionId, ButtonGroup &group);

		ButtonGroup &buttonGroup();
		void press();
		bool isPressed() const;

	private:
		ButtonGroup &myGroup;
	};

	class ButtonGroup {

	private:
		ButtonGroup(const std::string &groupId);
		void press(const ToggleButtonItem *item);

	public:
		const std::string &defaultAction() const;
		void setDefaultAction(const std::string &actionId);

	private:
		typedef std::set<const ToggleButtonItem*> ItemSet;

		ItemSet myItems;
		const ToggleButtonItem *myPressedItem;

		ZLStringOption myDefaultButtonOption;

	friend class ToggleButtonItem;
	friend class ZLToolbar;
	friend class ZLApplicationWindow;
	};

	class SeparatorItem : public Item {

	public:
		SeparatorItem(ZLToolbar &toolbar, Type type);
	};

	class ParameterItem : public ActionItem {

	public:
		enum SymbolSet {
			SET_DIGITS,
			SET_ANY
		};

	public:
		ParameterItem(ZLToolbar &toolbar, Type type, const std::string &actionId, const std::string &parameterId, int maxWidth);
		const std::string &parameterId() const;
		int maxWidth() const;

		void setSymbolSet(SymbolSet symbolSet);
		SymbolSet symbolSet() const;

	private:
		const std::string myParameterId;
		const int myMaxWidth;
		SymbolSet mySymbolSet;
	};

public:
	typedef shared_ptr<Item> ItemPtr;
	typedef std::vector<ItemPtr> ItemVector;

	ZLToolbar();

	const ItemVector &items() const;
	const ZLResource &resource(const std::string &id) const;

	void registerPopupData(const std::string &actionId, shared_ptr<ZLPopupData> popupData);

private:
	ButtonGroup &getButtonGroup(const std::string &id);

private:
	ItemVector myItems;
	const ZLResource &myResource;
	std::map<std::string, shared_ptr<ZLPopupData> > myPopupDataMap;
	std::map<std::string, shared_ptr<ButtonGroup> > myButtonGroups;

friend class MenuButtonItem;
friend class ZLApplication;
friend class ZLToolbarCreator;
};

#endif /* __ZLTOOLBAR_H__ */
