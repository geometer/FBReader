/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "ZLOptionView.h"
#include "ZLDialogContent.h"

ZLDialogContent::ZLDialogContent(const ZLResource &resource) : myResource(resource) {
}

ZLDialogContent::~ZLDialogContent() {
	for (std::vector<ZLOptionView*>::iterator it = myViews.begin(); it != myViews.end(); ++it) {
		delete *it;
	}
}

const std::string &ZLDialogContent::name() const {
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

void ZLDialogContent::addOption(const ZLResourceKey &key, ZLOptionEntry *option) {
	const ZLResource &resource = myResource[key];
	addOption(resource.value(), resource[TOOLTIP_KEY].value(), option);
}

void ZLDialogContent::addOptions(const ZLResourceKey &key0, ZLOptionEntry *option0, const ZLResourceKey &key1, ZLOptionEntry *option1) {
	const ZLResource &resource0 = myResource[key0];
	const ZLResource &resource1 = myResource[key1];
	addOptions(resource0.value(), resource0[TOOLTIP_KEY].value(), option0, resource1.value(), resource1[TOOLTIP_KEY].value(), option1);
}
