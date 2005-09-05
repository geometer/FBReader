/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __OPTIONSPAGE_H__
#define __OPTIONSPAGE_H__

#include <map>

#include <abstract/ZLOptionEntry.h>

class ZLOptionsDialogTab;

class OptionsPage {

protected:
	class ComboOptionEntry : public ZLComboOptionEntry {

	public:
		ComboOptionEntry(OptionsPage &page, const std::string &name, const std::string &initialValue) FB_DIALOG_SECTION;
		~ComboOptionEntry() FB_DIALOG_SECTION;
		const std::string &name() const FB_DIALOG_SECTION;
		const std::string &initialValue() const FB_DIALOG_SECTION;
		const std::vector<std::string> &values() const FB_DIALOG_SECTION;
		void onAccept(const std::string&) FB_DIALOG_SECTION;
		void onValueChange(const std::string &selectedValue) FB_DIALOG_SECTION;
		void addValue(const std::string &value) FB_DIALOG_SECTION;

	private:
		OptionsPage &myPage;
		std::string myName;
		std::vector<std::string> myValues;
		std::string myInitialValue;
	};

public:
	OptionsPage() FB_DIALOG_SECTION;
	virtual ~OptionsPage() FB_DIALOG_SECTION;

protected:
	void registerEntry(ZLOptionsDialogTab *tab, ZLOptionEntry *entry, const std::string &name) FB_DIALOG_SECTION;
	void registerEntries(ZLOptionsDialogTab *tab, ZLOptionEntry *entry0, ZLOptionEntry *entry1, const std::string &name) FB_DIALOG_SECTION;
	
protected:
	ComboOptionEntry *myComboEntry;

private:
	std::map<ZLOptionEntry*,std::string> myEntries;

friend class ComboOptionEntry;
};

inline OptionsPage::ComboOptionEntry::ComboOptionEntry(OptionsPage &page, const std::string &name, const std::string &initialValue) : myPage(page), myName(name), myInitialValue(initialValue) {}
inline OptionsPage::ComboOptionEntry::~ComboOptionEntry() {}
inline const std::string &OptionsPage::ComboOptionEntry::name() const { return myName; }
inline const std::string &OptionsPage::ComboOptionEntry::initialValue() const { return myInitialValue; }
inline const std::vector<std::string> &OptionsPage::ComboOptionEntry::values() const { return myValues; }
inline void OptionsPage::ComboOptionEntry::onAccept(const std::string&) {}
inline void OptionsPage::ComboOptionEntry::addValue(const std::string &value) { myValues.push_back(value); }

inline OptionsPage::OptionsPage() {}
inline OptionsPage::~OptionsPage() {}

#endif /* __OPTIONSPAGE_H__ */
