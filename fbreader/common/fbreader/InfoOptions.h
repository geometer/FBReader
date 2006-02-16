/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __INFOOPTIONS_H__
#define __INFOOPTIONS_H__

#include <abstract/ZLOptionEntry.h>

class StringInfoEntry : public ZLStringOptionEntry {

public:
	StringInfoEntry(const std::string &name, const std::string &value) FB_DIALOG_SECTION;
	~StringInfoEntry() FB_DIALOG_SECTION;
	const std::string &name() const FB_DIALOG_SECTION;
	const std::string &initialValue() const FB_DIALOG_SECTION;
	void onAccept(const std::string &value) FB_DIALOG_SECTION;

private:
	std::string myName;
	std::string myValue;
};

class EncodingEntry : public ZLComboOptionEntry {

public:
	EncodingEntry(const std::string &name, ZLStringOption &encodingOption) FB_DIALOG_SECTION;
	~EncodingEntry() FB_DIALOG_SECTION;

	const std::string &name() const FB_DIALOG_SECTION;
	const std::string &initialValue() const FB_DIALOG_SECTION;
	const std::vector<std::string> &values() const FB_DIALOG_SECTION;
	void onAccept(const std::string &value) FB_DIALOG_SECTION;

private:
	std::string myName;
	ZLStringOption &myEncodingOption;
};

class LanguageEntry : public ZLComboOptionEntry {

public:
	LanguageEntry(const std::string &name, ZLStringOption &encodingOption) FB_DIALOG_SECTION;
	~LanguageEntry() FB_DIALOG_SECTION;

	const std::string &name() const FB_DIALOG_SECTION;
	const std::string &initialValue() const FB_DIALOG_SECTION;
	const std::vector<std::string> &values() const FB_DIALOG_SECTION;
	void onAccept(const std::string &value) FB_DIALOG_SECTION;

private:
	std::string myName;
	ZLStringOption &myLanguageOption;
};

#endif /* __INFOOPTIONS_H__ */
