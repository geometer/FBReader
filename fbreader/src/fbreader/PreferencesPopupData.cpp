/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include "PreferencesPopupData.h"

#include <ZLResource.h>

#include "FBReaderActions.h"

PreferencesPopupData::PreferencesPopupData() {
	myActionIds.push_back(ActionCode::SHOW_BOOK_INFO_DIALOG);
	myActionIds.push_back(ActionCode::SHOW_READING_OPTIONS_DIALOG);
	myActionIds.push_back(ActionCode::SHOW_LOOKANDFEEL_OPTIONS_DIALOG);
	myActionIds.push_back(ActionCode::SHOW_LIBRARY_OPTIONS_DIALOG);
	myActionIds.push_back(ActionCode::SHOW_NETWORK_OPTIONS_DIALOG);
	myActionIds.push_back(ActionCode::SHOW_SYSTEM_OPTIONS_DIALOG);
}

size_t PreferencesPopupData::id() const {
	return 0;
}

size_t PreferencesPopupData::count() const {
	return myActionIds.size();
}

const std::string PreferencesPopupData::text(size_t index) {
	if (index >= myActionIds.size()) {
		return "";
	}
	const std::string &actionId = myActionIds[index];
	return resource(actionId)["label"].value();
}

void PreferencesPopupData::run(size_t index) {
	if (index >= myActionIds.size()) {
		return;
	}
	FBReader::Instance().doAction(myActionIds[index]);
}
