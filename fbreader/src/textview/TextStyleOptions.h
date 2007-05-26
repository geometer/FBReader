/*
 * FBReader -- electronic book reader
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

#ifndef __TEXTSTYLEOPTIONS_H__
#define __TEXTSTYLEOPTIONS_H__

#include <string>

#include <ZLOptionEntry.h>
#include <optionEntries/ZLFontFamilyOptionEntry.h>

class ZLPaintContext;

class FontFamilyWithBaseOptionEntry : public ZLFontFamilyOptionEntry {

private:
	static std::vector<std::string> ourAllFamilies;

public:
	FontFamilyWithBaseOptionEntry(ZLStringOption &option, const ZLPaintContext &context);

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);
};

class LineSpacingOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> ourAllValues;
	static std::vector<std::string> ourAllValuesPlusBase;

public:
	LineSpacingOptionEntry(ZLDoubleOption &option, bool allowBase);
	~LineSpacingOptionEntry();

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	ZLDoubleOption &myOption;
	bool myAllowBase;
};

class AlignmentOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> &values4();
	static std::vector<std::string> &values5();

private:
	static std::vector<std::string> ourValues4;
	static std::vector<std::string> ourValues5;
	
public:
	AlignmentOptionEntry(ZLIntegerOption &option, bool allowUndefined);
	~AlignmentOptionEntry();

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	ZLIntegerOption &myOption;
	bool myAllowUndefined;
};

#endif /* __TEXTSTYLEOPTIONS_H__ */
