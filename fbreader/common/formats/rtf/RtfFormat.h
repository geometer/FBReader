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

#ifndef __RTFFORMAT_H__
#define __RTFFORMAT_H__

#include <abstract/ZLInputStream.h>
#include <abstract/ZLOptions.h>
#include <abstract/ZLOptionEntry.h>

#include "../FormatPlugin.h"

class RtfFormat {

public:

	RtfFormat(const std::string &fileName);
	~RtfFormat() {}

//	bool createContentsTable() const { return CreateContentsTableOption.value(); }

private:
	ZLBooleanOption ConvertStylesOption;
friend class RtfInfoPage;
};

class RtfInfoPage : public FormatInfoPage {

public:
	RtfInfoPage(ZLOptionsDialog &dialog, const std::string &fileName, const std::string &tabName, bool showContentsEntry);
	~RtfInfoPage();

protected:
	class ComboOptionEntry : public ZLComboOptionEntry {

	public:
		ComboOptionEntry(RtfInfoPage &page, const std::string &name, const std::string &initialValue) FB_DIALOG_SECTION;
		~ComboOptionEntry() FB_DIALOG_SECTION;
		const std::string &name() const FB_DIALOG_SECTION;
		const std::string &initialValue() const FB_DIALOG_SECTION;
		const std::vector<std::string> &values() const FB_DIALOG_SECTION;
		void onAccept(const std::string&) FB_DIALOG_SECTION;
		void onValueChange(const std::string &selectedValue) FB_DIALOG_SECTION;
		void addValue(const std::string &value) FB_DIALOG_SECTION;

	private:
		RtfInfoPage &myPage;
		std::string myName;
		std::vector<std::string> myValues;
		std::string myInitialValue;
	};

private:
	RtfFormat myFormat;
	ZLSimpleBooleanOptionEntry *myConvertStyles;
	ComboOptionEntry *myStyleComboEntry;
};

inline RtfInfoPage::ComboOptionEntry::ComboOptionEntry(RtfInfoPage &page, const std::string &name, const std::string &initialValue) : myPage(page), myName(name), myInitialValue(initialValue) {}
inline RtfInfoPage::ComboOptionEntry::~ComboOptionEntry() {}
inline const std::string &RtfInfoPage::ComboOptionEntry::name() const { return myName; }
inline const std::string &RtfInfoPage::ComboOptionEntry::initialValue() const { return myInitialValue; }
inline const std::vector<std::string> &RtfInfoPage::ComboOptionEntry::values() const { return myValues; }
inline void RtfInfoPage::ComboOptionEntry::onAccept(const std::string&) {}
inline void RtfInfoPage::ComboOptionEntry::addValue(const std::string &value) { myValues.push_back(value); }

#endif /* __RTFFORMAT_H__ */
