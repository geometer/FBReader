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

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "ZLOptionView.h"
#include "ZLDialogContent.h"

ZLDialogContent::ZLDialogContent(const ZLResource &resource) : myResource(resource) {
}

ZLDialogContent::~ZLDialogContent() {
	for (std::vector<ZLOptionView*>::iterator it = myViews.begin(); it != myViews.end(); ++it) {
		delete *it;
	}
}

const std::string &ZLDialogContent::key() const {
	return myResource.name();
}

const std::string &ZLDialogContent::displayName() const {
	return myResource.value();
}

const std::string &ZLDialogContent::value(const ZLResourceKey &key) const {
	return myResource[key].value();
}

const ZLResource &ZLDialogContent::resource(const ZLResourceKey &key) const {
	return myResource[key];
}

void ZLDialogContent::accept() {
	for (std::vector<ZLOptionView*>::iterator it = myViews.begin(); it != myViews.end(); ++it) {
		(*it)->onAccept();
	}
}

void ZLDialogContent::addView(ZLOptionView *view) {
	if (view != 0) {
		myViews.push_back(view);
	}
}

static const ZLResourceKey TOOLTIP_KEY("tooltip");

static ZLOptionEntry *createEntryByOption(ZLSimpleOption &option) {
	switch (option.type()) {
		case ZLSimpleOption::TYPE_BOOLEAN:
			return new ZLSimpleBooleanOptionEntry((ZLBooleanOption&)option);
		case ZLSimpleOption::TYPE_BOOLEAN3:
			return new ZLSimpleBoolean3OptionEntry((ZLBoolean3Option&)option);
		case ZLSimpleOption::TYPE_STRING:
			return new ZLSimpleStringOptionEntry((ZLStringOption&)option);
		default:
			return 0;
	}
}

void ZLDialogContent::addOption(const ZLResourceKey &key, ZLOptionEntry *option) {
	const ZLResource &resource = myResource[key];
	addOption(resource.value(), resource[TOOLTIP_KEY].value(), option);
}

void ZLDialogContent::addOption(const ZLResourceKey &key, ZLSimpleOption &option) {
	addOption(key, createEntryByOption(option));
}

void ZLDialogContent::addOptions(const ZLResourceKey &key0, ZLOptionEntry *option0, const ZLResourceKey &key1, ZLOptionEntry *option1) {
	const ZLResource &resource0 = myResource[key0];
	const ZLResource &resource1 = myResource[key1];
	addOptions(resource0.value(), resource0[TOOLTIP_KEY].value(), option0, resource1.value(), resource1[TOOLTIP_KEY].value(), option1);
}

void ZLDialogContent::addOptions(const ZLResourceKey &key0, ZLSimpleOption &option0, const ZLResourceKey &key1, ZLSimpleOption &option1) {
	addOptions(key0, createEntryByOption(option0), key1, createEntryByOption(option1));
}
