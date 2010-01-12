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

#ifndef __ENCODINGOPTIONENTRY_H__
#define __ENCODINGOPTIONENTRY_H__

#include <map>

#include <ZLOptionEntry.h>

class AbstractEncodingEntry : public ZLComboOptionEntry {

public:
	AbstractEncodingEntry(const std::string &currentValue);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

	virtual void onAcceptValue(const std::string &value) = 0;

private:
	std::vector<std::string> mySetNames;
	std::map<std::string,std::vector<std::string> > myValues;
	mutable std::map<std::string,std::string> myInitialValues;
	std::map<std::string,std::string> myValueByName;
	std::string myInitialSetName;

friend class EncodingSetEntry;
};

class EncodingEntry : public AbstractEncodingEntry {

public:
	EncodingEntry(ZLStringOption &encodingOption);

	void onAcceptValue(const std::string &value);

private:
	ZLStringOption &myEncodingOption;
};

class EncodingSetEntry : public ZLComboOptionEntry {

public:
	EncodingSetEntry(AbstractEncodingEntry &encodingEntry);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string&) {}
	void onValueSelected(int index);

private:
	AbstractEncodingEntry &myEncodingEntry;
};

#endif /* __ENCODINGOPTIONENTRY_H__ */
