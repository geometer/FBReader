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

#include <stdlib.h>

#include <ZLApplication.h>
#include <ZLFile.h>
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

void ZLApplication::createToolbar() {
	toolbar();
	std::string fileName = ZLibrary::DefaultFilesPathPrefix() + "toolbar.xml";
	ZLToolbarCreator(*myToolbar).readDocument(ZLFile(fileName));
}

const ZLToolbar &ZLApplication::toolbar() const {
	if (myToolbar.isNull()) {
		myToolbar = new ZLToolbar(*this);
	}
	return *myToolbar;
}

void ZLApplication::registerPopupData(const std::string &actionId, shared_ptr<ZLPopupData> popupData) {
	myPopupDataMap[actionId] = popupData;
}

ZLToolbarCreator::ZLToolbarCreator(ZLToolbar &toolbar) : myToolbar(toolbar) {
}

void ZLToolbarCreator::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BUTTON = "button";
	static const std::string SEPARATOR = "separator";

	if (SEPARATOR == tag) {
		new ZLToolbar::SeparatorItem(myToolbar);
	} else if (BUTTON == tag) {
		const char *id = attributeValue(attributes, "id");
		if (id != 0) {
			new ZLToolbar::ButtonItem(myToolbar, id);
		}
	}
}

const ZLToolbar::ItemVector &ZLToolbar::items() const {
	return myItems;
}

const ZLResource &ZLToolbar::resource(const std::string &id) {
	return ZLResource::resource("toolbar")[id];
}

ZLToolbar::Item::Item(ZLToolbar &toolbar) {
	toolbar.myItems.push_back(this);
}

ZLToolbar::Item::~Item() {
}

ZLToolbar::ButtonItem::ButtonItem(ZLToolbar &toolbar, const std::string &actionId) : Item(toolbar), myResource(toolbar.resource(actionId)), myActionId(actionId) {
}

const std::string &ZLToolbar::ButtonItem::actionId() const {
	return myActionId;
}

const std::string &ZLToolbar::ButtonItem::label() const {
	const ZLResource &labelResource = myResource["label"];
	if (!labelResource.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return labelResource.value();
}

const std::string &ZLToolbar::ButtonItem::tooltip() const {
	const ZLResource &tooltipResource = myResource["tooltip"];
	if (!tooltipResource.hasValue()) {
		static const std::string EMPTY;
		return EMPTY;
	}
	return tooltipResource.value();
}

const std::string &ZLToolbar::ButtonItem::iconName() const {
	return actionId();
}

bool ZLToolbar::ButtonItem::isSeparator() const {
	return false;
}

shared_ptr<ZLPopupData> ZLApplication::popupData(const std::string &actionId) const {
	std::map<std::string,shared_ptr<ZLPopupData> >::const_iterator it = myPopupDataMap.find(actionId);
	return it == myPopupDataMap.end() ? 0 : it->second;
}

ZLToolbar::ZLToolbar(const ZLApplication &application) : myApplication(application) {
}

ZLToolbar::SeparatorItem::SeparatorItem(ZLToolbar &toolbar) : Item(toolbar) {
}

bool ZLToolbar::SeparatorItem::isSeparator() const {
	return true;
}

const ZLResource &ZLPopupData::resource(const std::string &actionId) {
	return ZLToolbar::resource(actionId);
}
