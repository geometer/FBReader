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

#ifndef __TEXTSTYLEOPTIONS_H__
#define __TEXTSTYLEOPTIONS_H__

#include <string>

#include <abstract/ZLOptionEntry.h>

class PaintContext;

class FontFamilyOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> ourAllFamilies;
	static std::vector<std::string> ourAllFamiliesPlusBase;

public:
	FontFamilyOptionEntry(const ZLStringOption &option, const PaintContext &context, bool allowBase);

	const std::string &name() const;
	const std::vector<std::string> &values() const { return myAllowBase ? ourAllFamiliesPlusBase : ourAllFamilies; }
	const std::string &initialValue() const;

	void onAccept(const std::string &value) const;

private:
	const ZLStringOption &myOption;
	bool myAllowBase;
};

class LineSpacingOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> ourAllValues;
	static std::vector<std::string> ourAllValuesPlusBase;

public:
	LineSpacingOptionEntry(const ZLDoubleOption &option, bool allowBase);

	const std::string &name() const;
	const std::vector<std::string> &values() const { return myAllowBase ? ourAllValuesPlusBase : ourAllValues; }
	const std::string &initialValue() const;

	void onAccept(const std::string &value) const;

private:
	const ZLDoubleOption &myOption;
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
	AlignmentOptionEntry(const ZLIntegerOption &option, bool allowUndefined) : myOption(option), myAllowUndefined(allowUndefined) {}

	const std::string &name() const;
	const std::vector<std::string> &values() const { return myAllowUndefined ? values5() : values4(); }
	const std::string &initialValue() const;

	void onAccept(const std::string &value) const;

private:
	const ZLIntegerOption &myOption;
	bool myAllowUndefined;
};

#endif /* __TEXTSTYLEOPTIONS_H__ */
