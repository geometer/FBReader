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
	StringInfoEntry(const std::string &name, const std::string &value);
	~StringInfoEntry();
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	std::string myValue;
};

class EncodingEntry : public ZLComboOptionEntry {

public:
	EncodingEntry(const std::string &name, ZLStringOption &encodingOption);
	~EncodingEntry();

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	ZLStringOption &myEncodingOption;
};

class LanguageEntry : public ZLComboOptionEntry {

public:
	LanguageEntry(const std::string &name, ZLStringOption &encodingOption);
	~LanguageEntry();

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	ZLStringOption &myLanguageOption;
};

#endif /* __INFOOPTIONS_H__ */
