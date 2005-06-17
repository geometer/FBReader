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

#include <abstract/shared_ptr.h>

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
	static ZLIntegerOption LeftMarginOption;
	static ZLIntegerOption RightMarginOption;
	static ZLIntegerOption TopMarginOption;
	static ZLIntegerOption BottomMarginOption;

	static ZLColorOption BackgroundColorOption;
	static ZLColorOption SelectedTextColorOption;
	static ZLColorOption RegularTextColorOption;
	static ZLColorOption HyperlinkTextColorOption;

protected:
	TextStyle() TEXT_STYLE_SECTION;

public:
	virtual ~TextStyle() TEXT_STYLE_SECTION;

	virtual bool isDecorated() const TEXT_STYLE_SECTION = 0;

	virtual const std::string &fontFamily() const TEXT_STYLE_SECTION = 0;
	virtual int fontSize() const TEXT_STYLE_SECTION = 0;

	virtual bool bold() const TEXT_STYLE_SECTION = 0;
	virtual bool italic() const TEXT_STYLE_SECTION = 0;

	virtual ZLColor color() const TEXT_STYLE_SECTION = 0;

	virtual int spaceBefore() const TEXT_STYLE_SECTION = 0;
	virtual int spaceAfter() const TEXT_STYLE_SECTION = 0;
	virtual int leftIndent() const TEXT_STYLE_SECTION = 0;
	virtual int rightIndent() const TEXT_STYLE_SECTION = 0;
	virtual int firstLineIndentDelta() const TEXT_STYLE_SECTION = 0;
	virtual int verticalShift() const TEXT_STYLE_SECTION = 0;
	virtual AlignmentType alignment() const TEXT_STYLE_SECTION = 0;
	virtual double lineSpace() const TEXT_STYLE_SECTION = 0;

	virtual bool allowHyphenations() const TEXT_STYLE_SECTION = 0;
};

typedef shared_ptr<TextStyle> TextStylePtr;

class BaseTextStyle : public TextStyle {

public:
	BaseTextStyle(const std::string &fontFamily, int fontSize) TEXT_STYLE_SECTION;
	~BaseTextStyle() TEXT_STYLE_SECTION;

	bool isDecorated() const TEXT_STYLE_SECTION;

	ZLStringOption &fontFamilyOption() const TEXT_STYLE_SECTION;
	const std::string &fontFamily() const TEXT_STYLE_SECTION;

	ZLIntegerOption &fontSizeOption() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;
	ZLBooleanOption &boldOption() const TEXT_STYLE_SECTION;
	bool bold() const TEXT_STYLE_SECTION;
	ZLBooleanOption &italicOption() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	ZLColor color() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;

	ZLIntegerOption &alignmentOption() const TEXT_STYLE_SECTION;
	AlignmentType alignment() const TEXT_STYLE_SECTION;

	ZLDoubleOption &lineSpaceOption() const TEXT_STYLE_SECTION;
	double lineSpace() const TEXT_STYLE_SECTION;

	bool allowHyphenations() const TEXT_STYLE_SECTION;

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
	TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations) TEXT_STYLE_SECTION;
	virtual ~TextStyleDecoration() TEXT_STYLE_SECTION;

	virtual bool isFullDecoration() const TEXT_STYLE_SECTION;

	virtual TextStylePtr createDecoratedStyle(const TextStylePtr base) const TEXT_STYLE_SECTION;

	const std::string &name() const TEXT_STYLE_SECTION;

	const ZLStringOption &fontFamilyOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &fontSizeDeltaOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &boldOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &italicOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &verticalShiftOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &allowHyphenationsOption() const TEXT_STYLE_SECTION;

	bool isHyperlinkStyle() const TEXT_STYLE_SECTION;
	void setHyperlinkStyle() TEXT_STYLE_SECTION;

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
	FullTextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations) TEXT_STYLE_SECTION;
	~FullTextStyleDecoration() TEXT_STYLE_SECTION;

	virtual bool isFullDecoration() const TEXT_STYLE_SECTION;

	TextStylePtr createDecoratedStyle(const TextStylePtr base) const TEXT_STYLE_SECTION;

	const ZLIntegerOption &spaceBeforeOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &spaceAfterOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &leftIndentOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &rightIndentOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &firstLineIndentDeltaOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &alignmentOption() const TEXT_STYLE_SECTION;
	const ZLDoubleOption &lineSpaceOption() const TEXT_STYLE_SECTION;

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
	DecoratedTextStyle(const TextStylePtr base) TEXT_STYLE_SECTION;
	virtual const TextStyleDecoration &decoration() const TEXT_STYLE_SECTION = 0;

public:
	virtual ~DecoratedTextStyle() TEXT_STYLE_SECTION;

	bool isDecorated() const TEXT_STYLE_SECTION;
	const TextStylePtr &base() const TEXT_STYLE_SECTION;

	ZLColor color() const TEXT_STYLE_SECTION;

private:
	const TextStylePtr myBase;
};

class PartialDecoratedTextStyle : public DecoratedTextStyle {

private:
	PartialDecoratedTextStyle(const TextStylePtr base, const TextStyleDecoration &decoration) TEXT_STYLE_SECTION;
	friend TextStylePtr TextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const;

public:
	virtual ~PartialDecoratedTextStyle() TEXT_STYLE_SECTION;
	const std::string &fontFamily() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;
	bool bold() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;

	AlignmentType alignment() const TEXT_STYLE_SECTION;
	bool allowHyphenations() const TEXT_STYLE_SECTION;

	double lineSpace() const TEXT_STYLE_SECTION;

	const TextStyleDecoration &decoration() const TEXT_STYLE_SECTION;

private:
	const TextStyleDecoration &myDecoration;
};

class FullDecoratedTextStyle : public DecoratedTextStyle {

private:
	FullDecoratedTextStyle(const TextStylePtr base, const FullTextStyleDecoration &decoration) TEXT_STYLE_SECTION;
	friend TextStylePtr FullTextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const;

public:
	~FullDecoratedTextStyle() TEXT_STYLE_SECTION;
	const std::string &fontFamily() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;
	bool bold() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;

	AlignmentType alignment() const TEXT_STYLE_SECTION;
	bool allowHyphenations() const TEXT_STYLE_SECTION;

	double lineSpace() const TEXT_STYLE_SECTION;

	const TextStyleDecoration &decoration() const TEXT_STYLE_SECTION;

private:
	const FullTextStyleDecoration &myDecoration;
};

class TextStyleCollection {

public:
	static TextStyleCollection &instance() TEXT_STYLE_SECTION;
	static void deleteInstance() TEXT_STYLE_SECTION;

	TextStylePtr baseStyle() const TEXT_STYLE_SECTION;
	const TextStyleDecoration *decoration(TextKind kind) const TEXT_STYLE_SECTION;

private:
	TextStyleCollection() TEXT_STYLE_SECTION;
	~TextStyleCollection() TEXT_STYLE_SECTION;

	void registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShiftOption, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations, bool isHyperlink) TEXT_STYLE_SECTION;
	void registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations, bool isHyperlink) TEXT_STYLE_SECTION;

private:
	static TextStyleCollection *ourInstance;
	
private:
	TextStylePtr myBaseStyle;
	std::map<TextKind,TextStyleDecoration*> myDecorationMap;
};

inline TextStyle::TextStyle() {}
inline TextStyle::~TextStyle() {}

inline bool BaseTextStyle::isDecorated() const { return false; }
inline ZLStringOption &BaseTextStyle::fontFamilyOption() const { return *myFontFamilyOption; }
inline const std::string &BaseTextStyle::fontFamily() const { return fontFamilyOption().value(); }
inline ZLIntegerOption &BaseTextStyle::fontSizeOption() const { return *myFontSizeOption; }
inline int BaseTextStyle::fontSize() const { return fontSizeOption().value(); }
inline ZLBooleanOption &BaseTextStyle::boldOption() const { return *myBoldOption; }
inline bool BaseTextStyle::bold() const { return boldOption().value(); }
inline ZLBooleanOption &BaseTextStyle::italicOption() const { return *myItalicOption; }
inline bool BaseTextStyle::italic() const { return italicOption().value(); }
inline int BaseTextStyle::spaceBefore() const { return 0; }
inline int BaseTextStyle::spaceAfter() const { return 0; }
inline int BaseTextStyle::leftIndent() const { return 0; }
inline int BaseTextStyle::rightIndent() const { return 0; }
inline int BaseTextStyle::firstLineIndentDelta() const { return 0; }
inline int BaseTextStyle::verticalShift() const { return 0; }
inline ZLIntegerOption &BaseTextStyle::alignmentOption() const { return *myAlignmentOption; }
inline AlignmentType BaseTextStyle::alignment() const { return (AlignmentType)alignmentOption().value(); }
inline ZLDoubleOption &BaseTextStyle::lineSpaceOption() const { return *myLineSpaceOption; }
inline double BaseTextStyle::lineSpace() const { return lineSpaceOption().value(); }
inline bool BaseTextStyle::allowHyphenations() const { return true; }

inline TextStyleDecoration::~TextStyleDecoration() {}
inline bool TextStyleDecoration::isFullDecoration() const { return false; }
inline const std::string &TextStyleDecoration::name() const { return myName; }
inline const ZLStringOption &TextStyleDecoration::fontFamilyOption() const { return myFontFamilyOption; }
inline const ZLIntegerOption &TextStyleDecoration::fontSizeDeltaOption() const { return myFontSizeDeltaOption; }
inline const ZLBoolean3Option &TextStyleDecoration::boldOption() const { return myBoldOption; }
inline const ZLBoolean3Option &TextStyleDecoration::italicOption() const { return myItalicOption; }
inline const ZLIntegerOption &TextStyleDecoration::verticalShiftOption() const { return myVerticalShiftOption; }
inline const ZLBoolean3Option &TextStyleDecoration::allowHyphenationsOption() const { return myAllowHyphenationsOption; }
inline bool TextStyleDecoration::isHyperlinkStyle() const { return myIsHyperlinkStyle; }
inline void TextStyleDecoration::setHyperlinkStyle() { myIsHyperlinkStyle = true; }

inline FullTextStyleDecoration::~FullTextStyleDecoration() {}
inline bool FullTextStyleDecoration::isFullDecoration() const { return true; }
inline const ZLIntegerOption &FullTextStyleDecoration::spaceBeforeOption() const { return mySpaceBeforeOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::spaceAfterOption() const { return mySpaceAfterOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::leftIndentOption() const { return myLeftIndentOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::rightIndentOption() const { return myRightIndentOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::firstLineIndentDeltaOption() const { return myFirstLineIndentDeltaOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::alignmentOption() const { return myAlignmentOption; }
inline const ZLDoubleOption &FullTextStyleDecoration::lineSpaceOption() const { return myLineSpaceOption; }

inline DecoratedTextStyle::DecoratedTextStyle(const TextStylePtr base) : myBase(base) {}
inline DecoratedTextStyle::~DecoratedTextStyle() {}
inline bool DecoratedTextStyle::isDecorated() const { return true; }
inline const TextStylePtr &DecoratedTextStyle::base() const { return myBase; }

inline PartialDecoratedTextStyle::PartialDecoratedTextStyle(const TextStylePtr base, const TextStyleDecoration &decoration) : DecoratedTextStyle(base), myDecoration(decoration) {}
inline PartialDecoratedTextStyle::~PartialDecoratedTextStyle() {}
inline int PartialDecoratedTextStyle::spaceBefore() const { return base()->spaceBefore(); }
inline int PartialDecoratedTextStyle::spaceAfter() const { return base()->spaceAfter(); }
inline int PartialDecoratedTextStyle::leftIndent() const { return base()->leftIndent(); }
inline int PartialDecoratedTextStyle::rightIndent() const { return base()->rightIndent(); }
inline int PartialDecoratedTextStyle::firstLineIndentDelta() const { return base()->firstLineIndentDelta(); }
inline int PartialDecoratedTextStyle::verticalShift() const { return base()->verticalShift() + myDecoration.verticalShiftOption().value(); }
inline AlignmentType PartialDecoratedTextStyle::alignment() const { return base()->alignment(); }
inline double PartialDecoratedTextStyle::lineSpace() const { return base()->lineSpace(); }
inline const TextStyleDecoration &PartialDecoratedTextStyle::decoration() const { return myDecoration; }

inline FullDecoratedTextStyle::FullDecoratedTextStyle(const TextStylePtr base, const FullTextStyleDecoration &decoration) : DecoratedTextStyle(base), myDecoration(decoration) {}
inline FullDecoratedTextStyle::~FullDecoratedTextStyle() {}
inline int FullDecoratedTextStyle::spaceBefore() const { return myDecoration.spaceBeforeOption().value(); }
inline int FullDecoratedTextStyle::spaceAfter() const { return myDecoration.spaceAfterOption().value(); }
inline int FullDecoratedTextStyle::leftIndent() const { return base()->leftIndent() + myDecoration.leftIndentOption().value(); }
inline int FullDecoratedTextStyle::rightIndent() const { return base()->rightIndent() + myDecoration.rightIndentOption().value(); }
inline int FullDecoratedTextStyle::verticalShift() const { return base()->verticalShift() + myDecoration.verticalShiftOption().value(); }
inline double FullDecoratedTextStyle::lineSpace() const { double space = myDecoration.lineSpaceOption().value(); return (space == 0) ? base()->lineSpace() : space; }
inline const TextStyleDecoration &FullDecoratedTextStyle::decoration() const { return myDecoration; }

inline TextStylePtr TextStyleCollection::baseStyle() const { return myBaseStyle; }

#endif /* __TEXTSTYLE_H__ */
