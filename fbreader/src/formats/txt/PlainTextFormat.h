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

#ifndef __PLAINTEXTFORMAT_H__
#define __PLAINTEXTFORMAT_H__

#include <ZLInputStream.h>
#include <ZLOptions.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <ZLResource.h>

#include "../FormatPlugin.h"

class PlainTextFormat {

public:
	enum ParagraphBreakType {
		BREAK_PARAGRAPH_AT_NEW_LINE = 1,
		BREAK_PARAGRAPH_AT_EMPTY_LINE = 2,
		BREAK_PARAGRAPH_AT_LINE_WITH_INDENT = 4,
	};

	PlainTextFormat(const std::string &fileName);
	~PlainTextFormat() {}

	bool initialized() const { return InitializedOption.value(); }
	int breakType() const { return BreakTypeOption.value(); }
	int ignoredIndent() const { return IgnoredIndentOption.value(); }
	int emptyLinesBeforeNewSection() const { return EmptyLinesBeforeNewSectionOption.value(); }
	bool createContentsTable() const { return CreateContentsTableOption.value(); }

private:
	ZLBooleanOption InitializedOption;
	ZLIntegerOption BreakTypeOption;
	ZLIntegerRangeOption IgnoredIndentOption;
	ZLIntegerRangeOption EmptyLinesBeforeNewSectionOption;
	ZLBooleanOption CreateContentsTableOption;

friend class PlainTextInfoPage;
friend class PlainTextFormatDetector;
};

class PlainTextInfoPage : public FormatInfoPage {

public:
	PlainTextInfoPage(ZLOptionsDialog &dialog, const std::string &fileName, const ZLResourceKey &key, bool showContentsEntry);
	~PlainTextInfoPage();

private:
	PlainTextFormat myFormat;

	ZLSimpleSpinOptionEntry *myIgnoredIndentEntry;
	ZLSimpleSpinOptionEntry *myEmptyLinesBeforeNewSectionEntry;

friend class BreakTypeOptionEntry;
friend class CreateContentsTableOptionEntry;
};

class PlainTextFormatDetector {

public:
	PlainTextFormatDetector() {}
	~PlainTextFormatDetector() {}

	void detect(ZLInputStream &stream, PlainTextFormat &format);
};

class BreakTypeOptionEntry : public ZLComboOptionEntry {

public:
	BreakTypeOptionEntry(PlainTextInfoPage &page, ZLIntegerOption &breakTypeOption);
	~BreakTypeOptionEntry();

	int initialIndex() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

private:
	PlainTextInfoPage &myPage;
	ZLIntegerOption &myBreakTypeOption;
};

class CreateContentsTableOptionEntry : public ZLSimpleBooleanOptionEntry {

public:
	CreateContentsTableOptionEntry(PlainTextInfoPage &page, ZLBooleanOption &option);
	~CreateContentsTableOptionEntry();
	void onStateChanged(bool state);

private:
	PlainTextInfoPage &myPage;
};

#endif /* __PLAINTEXTFORMAT_H__ */
