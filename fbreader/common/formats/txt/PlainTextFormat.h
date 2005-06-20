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

#ifndef __PLAINTEXTFORMAT_H__
#define __PLAINTEXTFORMAT_H__

#include <abstract/ZLInputStream.h>
#include <abstract/ZLOptions.h>

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
	ZLIntegerOption IgnoredIndentOption;
	ZLIntegerOption EmptyLinesBeforeNewSectionOption;
	ZLBooleanOption CreateContentsTableOption;

friend class PlainTextInfoPage;
friend class PlainTextFormatDetector;
};

class PlainTextInfoPage : public FormatInfoPage {

public:
	PlainTextInfoPage(ZLOptionsDialog &dialog, const std::string &fileName);
	~PlainTextInfoPage();

private:
	PlainTextFormat myFormat;
};

class PlainTextFormatDetector {

public:
	PlainTextFormatDetector() {}
	~PlainTextFormatDetector() {}

	void detect(ZLInputStream &stream, PlainTextFormat &format);
};

#endif /* __PLAINTEXTFORMAT_H__ */
