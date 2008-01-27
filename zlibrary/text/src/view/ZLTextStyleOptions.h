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

#ifndef __ZLTEXTSTYLEOPTIONS_H__
#define __ZLTEXTSTYLEOPTIONS_H__

#include <string>

#include <ZLResource.h>
#include <ZLOptionEntry.h>
#include <optionEntries/ZLFontFamilyOptionEntry.h>

class ZLPaintContext;

class ZLTextFontFamilyWithBaseOptionEntry : public ZLFontFamilyOptionEntry {

private:
	static std::vector<std::string> ourAllFamilies;

public:
	ZLTextFontFamilyWithBaseOptionEntry(ZLStringOption &option, const ZLResource &resource, const ZLPaintContext &context);

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	const ZLResource &myResource;
};

// TODO: remove deprecated class
class ZLTextLineSpacingOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> ourAllValues;
	static std::vector<std::string> ourAllValuesPlusBase;

public:
	ZLTextLineSpacingOptionEntry(ZLDoubleOption &option, const ZLResource &resource, bool allowBase);
	~ZLTextLineSpacingOptionEntry();

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	const ZLResource &myResource;
	ZLDoubleOption &myOption;
	bool myAllowBase;
};

class ZLTextLineSpaceOptionEntry : public ZLComboOptionEntry {

private:
	static std::vector<std::string> ourAllValues;
	static std::vector<std::string> ourAllValuesPlusBase;

public:
	ZLTextLineSpaceOptionEntry(ZLIntegerOption &option, const ZLResource &resource, bool allowBase);
	~ZLTextLineSpaceOptionEntry();

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	const ZLResource &myResource;
	ZLIntegerOption &myOption;
	bool myAllowBase;
};

class ZLTextAlignmentOptionEntry : public ZLComboOptionEntry {

private:
	std::vector<std::string> &values4() const;
	std::vector<std::string> &values5() const;

private:
	static std::vector<std::string> ourValues4;
	static std::vector<std::string> ourValues5;
	
public:
	ZLTextAlignmentOptionEntry(ZLIntegerOption &option, const ZLResource &resource, bool allowUndefined);
	~ZLTextAlignmentOptionEntry();

	const std::vector<std::string> &values() const;
	const std::string &initialValue() const;

	void onAccept(const std::string &value);

private:
	const ZLResource &myResource;
	ZLIntegerOption &myOption;
	bool myAllowUndefined;
};

#endif /* __ZLTEXTSTYLEOPTIONS_H__ */
