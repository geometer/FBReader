/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
		ComboOptionEntry(OptionsPage &page, const std::string &name, const std::string &initialValue) : myPage(page), myName(name), myInitialValue(initialValue) {}
		const std::string &name() const { return myName; }
		const std::string initialValue() const { return myInitialValue; }
		const std::vector<std::string> &values() const { return myValues; }
		void onAccept(const std::string&) const {}
		void onValueChange(const std::string &selectedValue);
		void addValue(const std::string &value) { myValues.push_back(value); }

	private:
		OptionsPage &myPage;
		std::string myName;
		std::vector<std::string> myValues;
		std::string myInitialValue;
	};

public:
	OptionsPage() {}
	virtual ~OptionsPage() {}

protected:
	void registerEntry(ZLOptionsDialogTab *tab, ZLOptionEntry *entry, const std::string &name);
	void registerEntries(ZLOptionsDialogTab *tab, ZLOptionEntry *entry0, ZLOptionEntry *entry1, const std::string &name);
	
protected:
	ComboOptionEntry *myComboEntry;

private:
	std::map<ZLOptionEntry*,std::string> myEntries;

friend class ComboOptionEntry;
};

#endif /* __OPTIONSPAGE_H__ */
