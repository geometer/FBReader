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

#ifndef __ENCODINGOPTIONENTRY_H__
#define __ENCODINGOPTIONENTRY_H__

#include <map>

#include <ZLOptionEntry.h>

class EncodingEntry : public ZLComboOptionEntry {

public:
	EncodingEntry(ZLStringOption &encodingOption);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

private:
	std::vector<std::string> mySetNames;
	std::map<std::string,std::vector<std::string> > myValues;
	mutable std::map<std::string,std::string> myInitialValues;
	std::map<std::string,std::string> myValueByName;
	ZLStringOption &myEncodingOption;
	std::string myInitialSetName;

friend class EncodingSetEntry;
};

class EncodingSetEntry : public ZLComboOptionEntry {

public:
	EncodingSetEntry(EncodingEntry &encodingEntry);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string&) {}
	void onValueSelected(int index);

private:
	EncodingEntry &myEncodingEntry;
};

#endif /* __ENCODINGOPTIONENTRY_H__ */
