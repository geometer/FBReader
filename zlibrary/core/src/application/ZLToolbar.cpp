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

#include <stdlib.h>

#include <ZLApplication.h>
#include <ZLXMLReader.h>
#include <ZLResource.h>
#include <ZLibrary.h>
#include <ZLPopupData.h>
#include "ZLToolbar.h"
#include "ZLApplicationWindow.h"

#include <optionEntries/ZLStringInfoEntry.h>

class ZLToolbarCreator : public ZLXMLReader {

public:
	ZLToolbarCreator(ZLToolbar &toolbar);
	void startElementHandler(const char *tag, const char **attributes);

private:
	ZLToolbar &myToolbar;
};

void ZLApplication::createToolbar(int index) {
	toolbar(index);
	std::string fileName = ZLibrary::DefaultFilesPathPrefix();
	const bool isWindowToolbar = index == ZLApplicationWindow::WINDOW_TOOLBAR;
	fileName += isWindowToolbar ? "toolbar.xml" : "fullscreen_toolbar.xml";
	ZLToolbarCreator(isWindowToolbar ? *myToolbar : *myFullscreenToolbar).readDocument(fileName);
}

const ZLToolbar &ZLApplication::toolbar(int index) const {
	if (index == ZLApplicationWindow::WINDOW_TOOLBAR) {
		if (myToolbar.isNull()) {
			myToolbar = new ZLToolbar();
		}
		return *myToolbar;
	} else {
		if (myFullscreenToolbar.isNull()) {
			myFullscreenToolbar = new ZLToolbar();
		}
		return *myFullscreenToolbar;
	}
}

void ZLApplication::registerPopupData(const std::string &actionId, shared_ptr<ZLPopupData> popupData) {
	toolbar(ZLApplicationWindow::WINDOW_TOOLBAR);
	myToolbar->registerPopupData(actionId, popupData);
}

void ZLToolbar::registerPopupData(const std::string &actionId, shared_ptr<ZLPopupData> popupData) {
	myPopupDataMap[actionId] = popupData;
}

ZLToolbarCreator::ZLToolbarCreator(ZLToolbar &toolbar) : myToolbar(toolbar) {
}

void ZLToolbarCreator::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BUTTON = "button";
	static const std::string MENU_BUTTON = "menuButton";
	static const std::string TOGGLE_BUTTON = "toggleButton";
	static const std::string TEXT_FIELD = "textField";
	static const std::string COMBO_BOX = "comboBox";
	static const std::string SEARCH_FIELD = "searchField";
	static const std::string SEPARATOR = "separator";

	const char *id = attributeValue(attributes, "id");
	const char *location = attributeValue(attributes, "location");
	ZLToolbar::Item *item = 0;

	if (BUTTON == tag) {
		if (id != 0) {
			myToolbar.addPlainButton(id);
		}
	} else if (MENU_BUTTON == tag) {
		if (id != 0) {
			myToolbar.addMenuButton(id);
		}
	} else if (TOGGLE_BUTTON == tag) {
		const char *groupId = attributeValue(attributes, "group");
		const char *isDefault = attributeValue(attributes, "default");
		if ((id != 0) && (groupId != 0)) {
			myToolbar.addToggleButton(id, groupId, isDefault != 0);
			/*
			if (isDefault != 0) {
				button.press();
			}
			*/
		}
	} else if (TEXT_FIELD == tag || COMBO_BOX == tag || SEARCH_FIELD == tag) {
		const char *parameterId = attributeValue(attributes, "parameterId");
		const char *maxWidth = attributeValue(attributes, "maxWidth");
		const char *symbolSet = attributeValue(attributes, "symbols");
		if ((id != 0) && (parameterId != 0) && (maxWidth != 0)) {
			int nMaxWidth = atoi(maxWidth);
			ZLToolbar::ParameterItem::SymbolSet sSet =
				((symbolSet != 0) && (std::string(symbolSet) == "digits")) ?
					ZLToolbar::ParameterItem::SET_DIGITS :
					ZLToolbar::ParameterItem::SET_ANY;
			if (nMaxWidth > 0) {
				ZLToolbar::Item::Type type = ZLToolbar::Item::TEXT_FIELD;
				if (COMBO_BOX == tag) {
					type = ZLToolbar::Item::COMBO_BOX;
				} else if (SEARCH_FIELD == tag) {
					type = ZLToolbar::Item::SEARCH_FIELD;
				}
				myToolbar.addParameterItem(type, id, parameterId, nMaxWidth, sSet);
			}
		}
	} else if (SEPARATOR == tag) {
		myToolbar.addSeparator();
	}
}

const ZLToolbar::ItemVector &ZLToolbar::items() const {
	return myItems;
}

ZLToolbar::Item::Item(ZLToolbar &toolbar) : myToolbar(toolbar) {
	toolbar.myItems.push_back(this);
}

ZLToolbar::Item::~Item() {
}

const ZLToolbar &ZLToolbar::Item::toolbar() const {
	return myToolbar;
}

ZLToolbar::Item::Location ZLToolbar::Item::location() const {
	return myLocation;
}

void ZLToolbar::Item::setLocation(Location location) {
	myLocation = location;
}

ZLToolbar::ActionItem::ActionItem(ZLToolbar &toolbar, const std::string &actionId, const ZLResource &resource) : Item(toolbar), myActionId(actionId), myResource(resource) {
}

const std::string &ZLToolbar::ActionItem::actionId() const {
	return myActionId;
}

const std::string &ZLToolbar::ActionItem::label() const {
	const ZLResource &labelResource = myResource["label"];
	if (!labelResource.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return labelResource.value();
}

const std::string &ZLToolbar::ActionItem::tooltip() const {
	const ZLResource &tooltipResource = myResource["tooltip"];
	if (!tooltipResource.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return tooltipResource.value();
}

ZLToolbar::AbstractButtonItem::AbstractButtonItem(ZLToolbar &toolbar, const std::string &actionId, const ZLResource &resource) : ActionItem(toolbar, actionId, resource) {
}

ZLToolbar::PlainButtonItem::PlainButtonItem(ZLToolbar &toolbar, const std::string &actionId, const ZLResource &resource) : AbstractButtonItem(toolbar, actionId, resource) {
}

ZLToolbar::MenuButtonItem::MenuButtonItem(ZLToolbar &toolbar, const std::string &actionId, const ZLResource &resource) : AbstractButtonItem(toolbar, actionId, resource) {
}

const std::string &ZLToolbar::MenuButtonItem::popupTooltip() const {
	const ZLResource &popupResource = myResource["popup"];
	if (!popupResource.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return popupResource.value();
}

shared_ptr<ZLPopupData> ZLToolbar::MenuButtonItem::popupData() const {
	std::map<std::string,shared_ptr<ZLPopupData> >::const_iterator it = toolbar().myPopupDataMap.find(actionId());
	return (it == toolbar().myPopupDataMap.end()) ? 0 : it->second;
}

ZLToolbar::ToggleButtonItem::ToggleButtonItem(ZLToolbar &toolbar, const std::string &actionId, ButtonGroup &group, const ZLResource &resource) : AbstractButtonItem(toolbar, actionId, resource), myGroup(group) {
	myGroup.myItems.insert(this);
}

const std::string &ZLToolbar::AbstractButtonItem::iconName() const {
	return actionId();
}

ZLToolbar::ButtonGroup &ZLToolbar::ToggleButtonItem::buttonGroup() {
	return myGroup;
}

void ZLToolbar::ToggleButtonItem::press() {
	myGroup.press(this);
}

bool ZLToolbar::ToggleButtonItem::isPressed() const {
	return this == myGroup.myPressedItem;
}

ZLToolbar::ButtonGroup &ZLToolbar::getButtonGroup(const std::string &id) {
	shared_ptr<ButtonGroup> group = myButtonGroups[id];
	if (group.isNull()) {
		group = new ButtonGroup(id);
		myButtonGroups[id] = group;
	}
	return *group;
}

ZLToolbar::ZLToolbar() : myResource(ZLResource::resource("toolbar")) {
}

void ZLToolbar::addPlainButton(const std::string &actionId) {
	new PlainButtonItem(*this, actionId, myResource[ZLResourceKey(actionId)]);
}

void ZLToolbar::addMenuButton(const std::string &actionId) {
	new MenuButtonItem(*this, actionId, myResource[ZLResourceKey(actionId)]);
}

void ZLToolbar::addToggleButton(const std::string &actionId, const std::string &groupId, bool isDefault) {
	ButtonGroup &group = getButtonGroup(groupId);
	ToggleButtonItem *button = new ToggleButtonItem(*this, actionId, group, myResource[ZLResourceKey(actionId)]);
	if (isDefault) {
		group.setDefaultAction(actionId);
	}
	if (group.defaultAction() == actionId) {
		button->press();
	}
}

void ZLToolbar::addParameterItem(Item::Type type, const std::string &actionId, const std::string &parameterId, int maxWidth, ParameterItem::SymbolSet symbolSet) {
	new ParameterItem(
		*this, type, actionId, parameterId, maxWidth, symbolSet, myResource[ZLResourceKey(actionId)]
	);
}

void ZLToolbar::addSeparator() {
	new SeparatorItem(*this);
}

ZLToolbar::ButtonGroup::ButtonGroup(const std::string &groupId) : myPressedItem(0), myDefaultButtonOption(ZLCategoryKey::LOOK_AND_FEEL, "ToggleButtonGroup", groupId, "") {
}

void ZLToolbar::ButtonGroup::press(const ToggleButtonItem *item) {
	myPressedItem = item;
	myDefaultButtonOption.setValue(item->actionId());
}

void ZLToolbar::ButtonGroup::setDefaultAction(const std::string &actionId) {
	if (myDefaultButtonOption.value().empty()) {
		myDefaultButtonOption.setValue(actionId);
	}
}

const std::string &ZLToolbar::ButtonGroup::defaultAction() const {
	return myDefaultButtonOption.value();
}

ZLToolbar::Item::Type ZLToolbar::PlainButtonItem::type() const {
	return PLAIN_BUTTON;
}

ZLToolbar::Item::Type ZLToolbar::MenuButtonItem::type() const {
	return MENU_BUTTON;
}

ZLToolbar::Item::Type ZLToolbar::ToggleButtonItem::type() const {
	return TOGGLE_BUTTON;
}

ZLToolbar::SeparatorItem::SeparatorItem(ZLToolbar &toolbar) : Item(toolbar) {
}

ZLToolbar::Item::Type ZLToolbar::SeparatorItem::type() const {
	return SEPARATOR;
}

ZLToolbar::ParameterItem::ParameterItem(ZLToolbar &toolbar, Type type, const std::string &actionId, const std::string &parameterId, int maxWidth, SymbolSet symbolSet, const ZLResource &resource) : ActionItem(toolbar, actionId, resource), myType(type), myParameterId(parameterId), myMaxWidth(maxWidth), mySymbolSet(symbolSet) {
}

ZLToolbar::Item::Type ZLToolbar::ParameterItem::type() const {
	return myType;
}

const std::string &ZLToolbar::ParameterItem::parameterId() const {
	return myParameterId;
}

int ZLToolbar::ParameterItem::maxWidth() const {
	return myMaxWidth;
}

ZLToolbar::ParameterItem::SymbolSet ZLToolbar::ParameterItem::symbolSet() const {
	return mySymbolSet;
}
