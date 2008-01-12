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

#include <ZLApplication.h>
#include <ZLXMLReader.h>
#include <ZLResource.h>
#include <ZLibrary.h>

class ZLToolbarCreator : public ZLXMLReader {

public:
	ZLToolbarCreator(ZLApplication::Toolbar &toolbar);
	void startElementHandler(const char *tag, const char **attributes);

private:
	ZLApplication::Toolbar &myToolbar;
};

void ZLApplication::createToolbar() {
	ZLToolbarCreator(myToolbar).readDocument(ZLibrary::DefaultFilesPathPrefix() + "toolbar.xml");
}

ZLToolbarCreator::ZLToolbarCreator(ZLApplication::Toolbar &toolbar) : myToolbar(toolbar) {
}

void ZLToolbarCreator::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BUTTON = "button";
	static const std::string SEPARATOR = "separator";
	if (BUTTON == tag) {
		const char *id = attributeValue(attributes, "id");
		if (id != 0) {
			const std::string sid = id;
			myToolbar.addButton(sid);
		}
	} else if (SEPARATOR == tag) {
		myToolbar.addSeparator();
	}
}

const ZLApplication::Toolbar &ZLApplication::toolbar() const {
	return myToolbar;
}

const ZLApplication::Toolbar::ItemVector &ZLApplication::Toolbar::items() const {
	return myItems;
}

ZLApplication::Toolbar::Item::Item() {
}

ZLApplication::Toolbar::Item::~Item() {
}

ZLApplication::Toolbar::ButtonItem::ButtonItem(const std::string &actionId, const ZLResource &tooltip) : myActionId(actionId), myTooltip(tooltip) {
}

const std::string &ZLApplication::Toolbar::ButtonItem::actionId() const {
	return myActionId;
}

const std::string &ZLApplication::Toolbar::ButtonItem::iconName() const {
	return myActionId;
}

shared_ptr<ZLApplication::Toolbar::ButtonGroup> ZLApplication::Toolbar::ButtonItem::buttonGroup() const {
	return myButtonGroup;
}

bool ZLApplication::Toolbar::ButtonItem::isToggleButton() const {
	return !myButtonGroup.isNull();
}

void ZLApplication::Toolbar::ButtonItem::press() {
	if (isToggleButton()) {
		myButtonGroup->press(this);
	}
}

bool ZLApplication::Toolbar::ButtonItem::isPressed() const {
	return isToggleButton() && (this == myButtonGroup->PressedItem);
}

shared_ptr<ZLApplication::Toolbar::ButtonGroup> ZLApplication::Toolbar::createButtonGroup(const std::string &unselectAllButtonsActionId) {
	return new ButtonGroup(unselectAllButtonsActionId);
}

ZLApplication::Toolbar::Toolbar() : myResource(ZLResource::resource("toolbar")) {
}

void ZLApplication::Toolbar::addButton(const std::string &actionId, shared_ptr<ButtonGroup> group) {
	ButtonItem *button = new ButtonItem(actionId, myResource[ZLResourceKey(actionId)]);
	myItems.push_back(button);
	button->setButtonGroup(group);
}

void ZLApplication::Toolbar::addOptionEntry(shared_ptr<ZLOptionEntry> entry) {
	if (!entry.isNull()) {
		myItems.push_back(new OptionEntryItem(entry));
	}
}

void ZLApplication::Toolbar::addSeparator() {
	myItems.push_back(new SeparatorItem());
}

ZLApplication::Toolbar::ButtonGroup::ButtonGroup(const std::string &unselectAllButtonsActionId) : UnselectAllButtonsActionId(unselectAllButtonsActionId), PressedItem(0) {
}

void ZLApplication::Toolbar::ButtonGroup::press(const ButtonItem *item) {
	PressedItem = item;
}

void ZLApplication::Toolbar::ButtonItem::setButtonGroup(shared_ptr<ButtonGroup> group) {
	if (!myButtonGroup.isNull()) {
		myButtonGroup->Items.erase(this);
	}
	myButtonGroup = group;
	if (!myButtonGroup.isNull()) {
		myButtonGroup->Items.insert(this);
	}
}

const std::string &ZLApplication::Toolbar::ButtonItem::tooltip() const {
	if (!myTooltip.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return myTooltip.value();
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::ButtonItem::type() const {
	return BUTTON;
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::SeparatorItem::type() const {
	return SEPARATOR;
}

ZLApplication::Toolbar::OptionEntryItem::OptionEntryItem(shared_ptr<ZLOptionEntry> entry) : myOptionEntry(entry) {
}

shared_ptr<ZLOptionEntry> ZLApplication::Toolbar::OptionEntryItem::entry() const {
	return myOptionEntry;
}

ZLApplication::Toolbar::Item::Type ZLApplication::Toolbar::OptionEntryItem::type() const {
	return OPTION_ENTRY;
}
