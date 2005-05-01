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

public:
	static ZLColorOption SelectedTextColorOption;
	static ZLColorOption RegularTextColorOption;
	static ZLColorOption HyperlinkTextColorOption;

protected:
	TextStyle() {}
	virtual ~TextStyle() {}

public:
	virtual bool isDecorated() const = 0;

	virtual const std::string &fontFamily() const = 0;
	virtual int fontSize() const = 0;

	virtual bool bold() const = 0;
	virtual bool italic() const = 0;

	virtual ZLColor color() const = 0;

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

	bool isDecorated() const { return false; }

	ZLStringOption &fontFamilyOption() const { return *myFontFamilyOption; }
	const std::string &fontFamily() const { return fontFamilyOption().value(); }

	ZLIntegerOption &fontSizeOption() const { return *myFontSizeOption; }
	int fontSize() const { return fontSizeOption().value(); }
	ZLBooleanOption &boldOption() const { return *myBoldOption; }
	bool bold() const { return boldOption().value(); }
	ZLBooleanOption &italicOption() const { return *myItalicOption; }
	bool italic() const { return italicOption().value(); }

	ZLColor color() const;

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
	TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations);
	virtual ~TextStyleDecoration() {}

	virtual bool isFullDecoration() const { return false; }

	virtual TextStyle *createDecoratedStyle(const TextStyle &base) const;

	const std::string &name() const { return myName; }

	const ZLStringOption &fontFamilyOption() const { return myFontFamilyOption; }
	const ZLIntegerOption &fontSizeDeltaOption() const { return myFontSizeDeltaOption; }
	const ZLBoolean3Option &boldOption() const { return myBoldOption; }
	const ZLBoolean3Option &italicOption() const { return myItalicOption; }
	const ZLIntegerOption &verticalShiftOption() const { return myVerticalShiftOption; }
	const ZLBoolean3Option &allowHyphenationsOption() const { return myAllowHyphenationsOption; }

	bool isHyperlinkStyle() const { return myIsHyperlinkStyle; }
	void setHyperlinkStyle() { myIsHyperlinkStyle = true; }

private:
	std::string myName;

	ZLStringOption myFontFamilyOption;
	ZLIntegerOption myFontSizeDeltaOption;
	ZLBoolean3Option myBoldOption;
	ZLBoolean3Option myItalicOption;

	ZLIntegerOption myVerticalShiftOption;

	ZLBoolean3Option myAllowHyphenationsOption;

	bool myIsHyperlinkStyle;
};

class FullTextStyleDecoration : public TextStyleDecoration {

public:
	FullTextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations);

	virtual bool isFullDecoration() const { return true; }

	TextStyle *createDecoratedStyle(const TextStyle &base) const;

	const ZLIntegerOption &spaceBeforeOption() const { return mySpaceBeforeOption; }
	const ZLIntegerOption &spaceAfterOption() const { return mySpaceAfterOption; }
	const ZLIntegerOption &leftIndentOption() const { return myLeftIndentOption; }
	const ZLIntegerOption &rightIndentOption() const { return myRightIndentOption; }
	const ZLIntegerOption &firstLineIndentDeltaOption() const { return myFirstLineIndentDeltaOption; }
	const ZLIntegerOption &alignmentOption() const { return myAlignmentOption; }
	const ZLDoubleOption &lineSpaceOption() const { return myLineSpaceOption; }

private:
	ZLIntegerOption mySpaceBeforeOption;
	ZLIntegerOption mySpaceAfterOption;
	ZLIntegerOption myLeftIndentOption;
	ZLIntegerOption myRightIndentOption;
	ZLIntegerOption myFirstLineIndentDeltaOption;

	ZLIntegerOption myAlignmentOption;

	ZLDoubleOption myLineSpaceOption;
};

class DecoratedTextStyle : public TextStyle {

protected:
	DecoratedTextStyle(const TextStyle &base) : myBase(base) {}
	virtual const TextStyleDecoration &decoration() const = 0;

public:
	virtual ~DecoratedTextStyle() {}

	bool isDecorated() const { return true; }
	const TextStyle &base() const { return myBase; }

	ZLColor color() const;

private:
	const TextStyle &myBase;
};

class PartialDecoratedTextStyle : public DecoratedTextStyle {

private:
	PartialDecoratedTextStyle(const TextStyle &base, const TextStyleDecoration &decoration) : DecoratedTextStyle(base), myDecoration(decoration) {}
	friend TextStyle *TextStyleDecoration::createDecoratedStyle(const TextStyle &base) const;

public:
	const std::string &fontFamily() const;
	int fontSize() const;
	bool bold() const;
	bool italic() const;

	int spaceBefore() const { return base().spaceBefore(); }
	int spaceAfter() const { return base().spaceAfter(); }
	int leftIndent() const { return base().leftIndent(); }
	int rightIndent() const { return base().rightIndent(); }
	int firstLineIndentDelta() const { return base().firstLineIndentDelta(); }
	int verticalShift() const { return base().verticalShift() + myDecoration.verticalShiftOption().value(); }

	AlignmentType alignment() const { return base().alignment(); }
	bool allowHyphenations() const;

	double lineSpace() const { return base().lineSpace(); }

	const TextStyleDecoration &decoration() const { return myDecoration; }

private:
	const TextStyleDecoration &myDecoration;
};

class FullDecoratedTextStyle : public DecoratedTextStyle {

private:
	FullDecoratedTextStyle(const TextStyle &base, const FullTextStyleDecoration &decoration) : DecoratedTextStyle(base), myDecoration(decoration) {}
	friend TextStyle *FullTextStyleDecoration::createDecoratedStyle(const TextStyle &base) const;

public:
	const std::string &fontFamily() const;
	int fontSize() const;
	bool bold() const;
	bool italic() const;

	int spaceBefore() const { return myDecoration.spaceBeforeOption().value(); }
	int spaceAfter() const { return myDecoration.spaceAfterOption().value(); }
	int leftIndent() const { return base().leftIndent() + myDecoration.leftIndentOption().value(); }
	int rightIndent() const { return base().rightIndent() + myDecoration.rightIndentOption().value(); }
	int firstLineIndentDelta() const;
	int verticalShift() const { return base().verticalShift() + myDecoration.verticalShiftOption().value(); }

	AlignmentType alignment() const;
	bool allowHyphenations() const;

	double lineSpace() const { double space = myDecoration.lineSpaceOption().value(); return (space == 0) ? base().lineSpace() : space; }

	const TextStyleDecoration &decoration() const { return myDecoration; }

private:
	const FullTextStyleDecoration &myDecoration;
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

	void registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShiftOption, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations, bool isHyperlink);
	void registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations, bool isHyperlink);

private:
	static TextStyleCollection *ourInstance;
	
private:
	BaseTextStyle *myBaseStyle;
	std::map<TextKind,TextStyleDecoration*> myDecorationMap;
};

#endif /* __TEXTSTYLE_H__ */
