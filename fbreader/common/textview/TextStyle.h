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

#ifndef __TEXTSTYLE_H__
#define __TEXTSTYLE_H__

#include <map>
#include <string>

#include <abstract/ZLOptions.h>

#include "../model/TextKind.h"

enum AlignmentType {
	ALIGN_UNDEFINED = 0,
	ALIGN_LEFT = 1,
	ALIGN_RIGHT = 2,
	ALIGN_CENTER = 3,
	ALIGN_JUSTIFY = 4
};

class TextStyle {

protected:
	TextStyle() {}
	virtual ~TextStyle() {}

public:
	virtual const std::string fontFamily() const = 0;
	virtual int fontSize() const = 0;

	virtual bool bold() const = 0;
	virtual bool italic() const = 0;

	virtual int spaceBefore() const = 0;
	virtual int spaceAfter() const = 0;
	virtual int leftIndent() const = 0;
	virtual int rightIndent() const = 0;
	virtual int firstLineIndentDelta() const = 0;
	virtual int verticalShift() const = 0;
	virtual AlignmentType alignment() const = 0;
	virtual double lineSpace() const = 0;

	virtual bool allowHyphenations() const = 0;
};

class BaseTextStyle : public TextStyle {

public:
	BaseTextStyle(const std::string &fontFamily, int fontSize);
	~BaseTextStyle();

	ZLStringOption &fontFamilyOption() const { return *myFontFamilyOption; }
	const std::string fontFamily() const { return fontFamilyOption().value(); }

	ZLIntegerOption &fontSizeOption() const { return *myFontSizeOption; }
	int fontSize() const { return fontSizeOption().value(); }
	ZLBooleanOption &boldOption() const { return *myBoldOption; }
	bool bold() const { return boldOption().value(); }
	ZLBooleanOption &italicOption() const { return *myItalicOption; }
	bool italic() const { return italicOption().value(); }

	int spaceBefore() const { return 0; }
	int spaceAfter() const { return 0; }
	int leftIndent() const { return 0; }
	int rightIndent() const { return 0; }
	int firstLineIndentDelta() const { return 0; }
	int verticalShift() const { return 0; }

	ZLIntegerOption &alignmentOption() const { return *myAlignmentOption; }
	AlignmentType alignment() const { return (AlignmentType)alignmentOption().value(); }

	ZLDoubleOption &lineSpaceOption() const { return *myLineSpaceOption; }
	double lineSpace() const { return lineSpaceOption().value(); }

	bool allowHyphenations() const { return true; }

private:
	ZLStringOption *myFontFamilyOption;
	ZLIntegerOption *myFontSizeOption;
	ZLBooleanOption *myBoldOption;
	ZLBooleanOption *myItalicOption;
	ZLIntegerOption *myAlignmentOption;
	ZLDoubleOption *myLineSpaceOption;
};

class TextStyleDecoration {

public:
	TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations);

	const std::string &name() const { return myName; }

	const ZLStringOption &fontFamilyOption() const { return myFontFamilyOption; }
	const ZLIntegerOption &fontSizeDeltaOption() const { return myFontSizeDeltaOption; }
	const ZLBoolean3Option &boldOption() const { return myBoldOption; }
	const ZLBoolean3Option &italicOption() const { return myItalicOption; }
	const ZLIntegerOption &spaceBeforeOption() const { return mySpaceBeforeOption; }
	const ZLIntegerOption &spaceAfterOption() const { return mySpaceAfterOption; }
	const ZLIntegerOption &leftIndentOption() const { return myLeftIndentOption; }
	const ZLIntegerOption &rightIndentOption() const { return myRightIndentOption; }
	const ZLIntegerOption &firstLineIndentDeltaOption() const { return myFirstLineIndentDeltaOption; }
	const ZLIntegerOption &verticalShiftOption() const { return myVerticalShiftOption; }
	const ZLIntegerOption &alignmentOption() const { return myAlignmentOption; }
	const ZLDoubleOption &lineSpaceOption() const { return myLineSpaceOption; }
	const ZLBoolean3Option &allowHyphenationsOption() const { return myAllowHyphenationsOption; }

private:
	std::string myName;

	ZLStringOption myFontFamilyOption;
	ZLIntegerOption myFontSizeDeltaOption;
	ZLBoolean3Option myBoldOption;
	ZLBoolean3Option myItalicOption;

	ZLIntegerOption mySpaceBeforeOption;
	ZLIntegerOption mySpaceAfterOption;
	ZLIntegerOption myLeftIndentOption;
	ZLIntegerOption myRightIndentOption;
	ZLIntegerOption myFirstLineIndentDeltaOption;
	ZLIntegerOption myVerticalShiftOption;

	ZLIntegerOption myAlignmentOption;

	ZLDoubleOption myLineSpaceOption;

	ZLBoolean3Option myAllowHyphenationsOption;
};

class DecoratedTextStyle : public TextStyle {

public:
	DecoratedTextStyle(const TextStyle &base, const TextStyleDecoration &decoration) : myBase(base), myDecoration(decoration) {}

	const TextStyle &base() const { return myBase; }

	const std::string fontFamily() const;
	int fontSize() const;
	bool bold() const;
	bool italic() const;

	int spaceBefore() const { return myDecoration.spaceBeforeOption().value(); }
	int spaceAfter() const { return myDecoration.spaceAfterOption().value(); }
	int leftIndent() const { return myBase.leftIndent() + myDecoration.leftIndentOption().value(); }
	int rightIndent() const { return myBase.rightIndent() + myDecoration.rightIndentOption().value(); }
	int firstLineIndentDelta() const;
	int verticalShift() const { return myBase.verticalShift() + myDecoration.verticalShiftOption().value(); }

	AlignmentType alignment() const;
	bool allowHyphenations() const;

	double lineSpace() const { double space = myDecoration.lineSpaceOption().value(); return (space == 0) ? myBase.lineSpace() : space; }

private:
	const TextStyle &myBase;
	const TextStyleDecoration &myDecoration;
};

class TextStyleCollection {

public:
	static TextStyleCollection &instance() {
		if (ourInstance == 0) {
			ourInstance = new TextStyleCollection();
		}
		return *ourInstance;
	}

	BaseTextStyle &baseStyle() const { return *myBaseStyle; }
	const TextStyleDecoration *decoration(TextKind kind) const;

private:
	TextStyleCollection();
	~TextStyleCollection();

	void registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShiftOption, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations);
	void registerPartialStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations);

private:
	static TextStyleCollection *ourInstance;
	
private:
	BaseTextStyle *myBaseStyle;
	std::map<TextKind,TextStyleDecoration*> myDecorationMap;
};

#endif /* __TEXTSTYLE_H__ */
