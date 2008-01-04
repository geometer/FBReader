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

#ifndef __OPTIONSPAGE_H__
#define __OPTIONSPAGE_H__

#include <map>

#include <ZLOptionEntry.h>

class ZLDialogContent;

class OptionsPage;

class ComboOptionEntry : public ZLComboOptionEntry {

public:
	ComboOptionEntry(OptionsPage &page, const std::string &initialValue);
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string&);
	void onValueSelected(int index);
	void addValue(const std::string &value);

protected:
	OptionsPage &myPage;
	std::vector<std::string> myValues;
	std::string myInitialValue;
};

class OptionsPage {

public:
	virtual ~OptionsPage();

protected:
	OptionsPage();

	void registerEntry(ZLDialogContent &tab, const ZLResourceKey &entryKey, ZLOptionEntry *entry, const std::string &name);
	void registerEntries(ZLDialogContent &tab, const ZLResourceKey &entry0Key, ZLOptionEntry *entry0, const ZLResourceKey &entry1Key, ZLOptionEntry *entry1, const std::string &name);
	
protected:
	ComboOptionEntry *myComboEntry;

private:
	std::map<ZLOptionEntry*,std::string> myEntries;

friend class ComboOptionEntry;
};

inline ComboOptionEntry::ComboOptionEntry(OptionsPage &page, const std::string &initialValue) : myPage(page), myInitialValue(initialValue) {}
inline const std::string &ComboOptionEntry::initialValue() const { return myInitialValue; }
inline const std::vector<std::string> &ComboOptionEntry::values() const { return myValues; }
inline void ComboOptionEntry::onAccept(const std::string&) {}
inline void ComboOptionEntry::addValue(const std::string &value) { myValues.push_back(value); }

inline OptionsPage::OptionsPage() {}
inline OptionsPage::~OptionsPage() {}

#endif /* __OPTIONSPAGE_H__ */
