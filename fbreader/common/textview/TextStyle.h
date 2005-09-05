/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __TEXTSTYLE_H__
#define __TEXTSTYLE_H__

#include <map>
#include <string>

#include <abstract/shared_ptr.h>

#include <abstract/ZLOptions.h>

#include "../model/TextKind.h"
#include "../model/AlignmentType.h"

class TextStyle {

public:
	static const std::string DefaultFontName;

	static ZLIntegerRangeOption LeftMarginOption;
	static ZLIntegerRangeOption RightMarginOption;
	static ZLIntegerRangeOption TopMarginOption;
	static ZLIntegerRangeOption BottomMarginOption;

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

	ZLIntegerRangeOption &fontSizeOption() const TEXT_STYLE_SECTION;
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
	ZLIntegerRangeOption *myFontSizeOption;
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

	ZLStringOption &fontFamilyOption() TEXT_STYLE_SECTION;
	ZLIntegerRangeOption &fontSizeDeltaOption() TEXT_STYLE_SECTION;
	ZLBoolean3Option &boldOption() TEXT_STYLE_SECTION;
	ZLBoolean3Option &italicOption() TEXT_STYLE_SECTION;
	ZLIntegerOption &verticalShiftOption() TEXT_STYLE_SECTION;
	ZLBoolean3Option &allowHyphenationsOption() TEXT_STYLE_SECTION;
	const ZLStringOption &fontFamilyOption() const TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &fontSizeDeltaOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &boldOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &italicOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &verticalShiftOption() const TEXT_STYLE_SECTION;
	const ZLBoolean3Option &allowHyphenationsOption() const TEXT_STYLE_SECTION;

	bool isHyperlinkStyle() const TEXT_STYLE_SECTION;
	void setHyperlinkStyle() TEXT_STYLE_SECTION;

private:
	std::string myName;

	ZLStringOption myFontFamilyOption;
	ZLIntegerRangeOption myFontSizeDeltaOption;
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

	ZLIntegerRangeOption &spaceBeforeOption() TEXT_STYLE_SECTION;
	ZLIntegerRangeOption &spaceAfterOption() TEXT_STYLE_SECTION;
	ZLIntegerRangeOption &leftIndentOption() TEXT_STYLE_SECTION;
	ZLIntegerRangeOption &rightIndentOption() TEXT_STYLE_SECTION;
	ZLIntegerRangeOption &firstLineIndentDeltaOption() TEXT_STYLE_SECTION;
	ZLIntegerOption &alignmentOption() TEXT_STYLE_SECTION;
	ZLDoubleOption &lineSpaceOption() TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &spaceBeforeOption() const TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &spaceAfterOption() const TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &leftIndentOption() const TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &rightIndentOption() const TEXT_STYLE_SECTION;
	const ZLIntegerRangeOption &firstLineIndentDeltaOption() const TEXT_STYLE_SECTION;
	const ZLIntegerOption &alignmentOption() const TEXT_STYLE_SECTION;
	const ZLDoubleOption &lineSpaceOption() const TEXT_STYLE_SECTION;

private:
	ZLIntegerRangeOption mySpaceBeforeOption;
	ZLIntegerRangeOption mySpaceAfterOption;
	ZLIntegerRangeOption myLeftIndentOption;
	ZLIntegerRangeOption myRightIndentOption;
	ZLIntegerRangeOption myFirstLineIndentDeltaOption;

	ZLIntegerOption myAlignmentOption;

	ZLDoubleOption myLineSpaceOption;
};

class DecoratedTextStyle : public TextStyle {

protected:
	DecoratedTextStyle(const TextStylePtr base) TEXT_STYLE_SECTION;

public:
	virtual ~DecoratedTextStyle() TEXT_STYLE_SECTION;

	bool isDecorated() const TEXT_STYLE_SECTION;
	const TextStylePtr base() const TEXT_STYLE_SECTION;

private:
	const TextStylePtr myBase;
};

class ForcedControlEntry;

class ForcedTextStyle : public DecoratedTextStyle {

public:
	ForcedTextStyle(TextStylePtr base, const ForcedControlEntry &entry) TEXT_STYLE1_SECTION;
	~ForcedTextStyle() TEXT_STYLE1_SECTION;

	const std::string &fontFamily() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;

	bool bold() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	ZLColor color() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;
	AlignmentType alignment() const TEXT_STYLE_SECTION;
	double lineSpace() const TEXT_STYLE_SECTION;

	bool allowHyphenations() const TEXT_STYLE_SECTION;

private:
	const ForcedControlEntry &myEntry;
};

class PartialDecoratedTextStyle : public DecoratedTextStyle {

private:
	PartialDecoratedTextStyle(const TextStylePtr base, const TextStyleDecoration &decoration) TEXT_STYLE1_SECTION;
	friend TextStylePtr TextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const;

public:
	virtual ~PartialDecoratedTextStyle() TEXT_STYLE1_SECTION;
	const std::string &fontFamily() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;
	bool bold() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	ZLColor color() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;

	AlignmentType alignment() const TEXT_STYLE_SECTION;
	bool allowHyphenations() const TEXT_STYLE_SECTION;

	double lineSpace() const TEXT_STYLE_SECTION;

private:
	const TextStyleDecoration &myDecoration;
};

class FullDecoratedTextStyle : public DecoratedTextStyle {

private:
	FullDecoratedTextStyle(const TextStylePtr base, const FullTextStyleDecoration &decoration) TEXT_STYLE1_SECTION;
	friend TextStylePtr FullTextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const;

public:
	~FullDecoratedTextStyle() TEXT_STYLE1_SECTION;
	const std::string &fontFamily() const TEXT_STYLE_SECTION;
	int fontSize() const TEXT_STYLE_SECTION;
	bool bold() const TEXT_STYLE_SECTION;
	bool italic() const TEXT_STYLE_SECTION;

	ZLColor color() const TEXT_STYLE_SECTION;

	int spaceBefore() const TEXT_STYLE_SECTION;
	int spaceAfter() const TEXT_STYLE_SECTION;
	int leftIndent() const TEXT_STYLE_SECTION;
	int rightIndent() const TEXT_STYLE_SECTION;
	int firstLineIndentDelta() const TEXT_STYLE_SECTION;
	int verticalShift() const TEXT_STYLE_SECTION;

	AlignmentType alignment() const TEXT_STYLE_SECTION;
	bool allowHyphenations() const TEXT_STYLE_SECTION;

	double lineSpace() const TEXT_STYLE_SECTION;

private:
	const FullTextStyleDecoration &myDecoration;
};

class TextStyleCollection {

public:
	static TextStyleCollection &instance() TEXT_STYLE_SECTION;
	static void deleteInstance() TEXT_STYLE_SECTION;

	TextStylePtr baseStyle() const TEXT_STYLE_SECTION;
	TextStyleDecoration *decoration(TextKind kind) const TEXT_STYLE_SECTION;

private:
	TextStyleCollection() TEXT_STYLE1_SECTION;
	~TextStyleCollection() TEXT_STYLE1_SECTION;

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
inline ZLIntegerRangeOption &BaseTextStyle::fontSizeOption() const { return *myFontSizeOption; }
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
inline const ZLIntegerRangeOption &TextStyleDecoration::fontSizeDeltaOption() const { return myFontSizeDeltaOption; }
inline const ZLBoolean3Option &TextStyleDecoration::boldOption() const { return myBoldOption; }
inline const ZLBoolean3Option &TextStyleDecoration::italicOption() const { return myItalicOption; }
inline const ZLIntegerOption &TextStyleDecoration::verticalShiftOption() const { return myVerticalShiftOption; }
inline const ZLBoolean3Option &TextStyleDecoration::allowHyphenationsOption() const { return myAllowHyphenationsOption; }
inline ZLStringOption &TextStyleDecoration::fontFamilyOption() { return myFontFamilyOption; }
inline ZLIntegerRangeOption &TextStyleDecoration::fontSizeDeltaOption() { return myFontSizeDeltaOption; }
inline ZLBoolean3Option &TextStyleDecoration::boldOption() { return myBoldOption; }
inline ZLBoolean3Option &TextStyleDecoration::italicOption() { return myItalicOption; }
inline ZLIntegerOption &TextStyleDecoration::verticalShiftOption() { return myVerticalShiftOption; }
inline ZLBoolean3Option &TextStyleDecoration::allowHyphenationsOption() { return myAllowHyphenationsOption; }
inline bool TextStyleDecoration::isHyperlinkStyle() const { return myIsHyperlinkStyle; }
inline void TextStyleDecoration::setHyperlinkStyle() { myIsHyperlinkStyle = true; }

inline FullTextStyleDecoration::~FullTextStyleDecoration() {}
inline bool FullTextStyleDecoration::isFullDecoration() const { return true; }
inline ZLIntegerRangeOption &FullTextStyleDecoration::spaceBeforeOption() { return mySpaceBeforeOption; }
inline ZLIntegerRangeOption &FullTextStyleDecoration::spaceAfterOption() { return mySpaceAfterOption; }
inline ZLIntegerRangeOption &FullTextStyleDecoration::leftIndentOption() { return myLeftIndentOption; }
inline ZLIntegerRangeOption &FullTextStyleDecoration::rightIndentOption() { return myRightIndentOption; }
inline ZLIntegerRangeOption &FullTextStyleDecoration::firstLineIndentDeltaOption() { return myFirstLineIndentDeltaOption; }
inline ZLIntegerOption &FullTextStyleDecoration::alignmentOption() { return myAlignmentOption; }
inline ZLDoubleOption &FullTextStyleDecoration::lineSpaceOption() { return myLineSpaceOption; }
inline const ZLIntegerRangeOption &FullTextStyleDecoration::spaceBeforeOption() const { return mySpaceBeforeOption; }
inline const ZLIntegerRangeOption &FullTextStyleDecoration::spaceAfterOption() const { return mySpaceAfterOption; }
inline const ZLIntegerRangeOption &FullTextStyleDecoration::leftIndentOption() const { return myLeftIndentOption; }
inline const ZLIntegerRangeOption &FullTextStyleDecoration::rightIndentOption() const { return myRightIndentOption; }
inline const ZLIntegerRangeOption &FullTextStyleDecoration::firstLineIndentDeltaOption() const { return myFirstLineIndentDeltaOption; }
inline const ZLIntegerOption &FullTextStyleDecoration::alignmentOption() const { return myAlignmentOption; }
inline const ZLDoubleOption &FullTextStyleDecoration::lineSpaceOption() const { return myLineSpaceOption; }

inline DecoratedTextStyle::DecoratedTextStyle(const TextStylePtr base) : myBase(base) {}
inline DecoratedTextStyle::~DecoratedTextStyle() {}
inline bool DecoratedTextStyle::isDecorated() const { return true; }
inline const TextStylePtr DecoratedTextStyle::base() const { return myBase; }

inline ForcedTextStyle::ForcedTextStyle(TextStylePtr base, const ForcedControlEntry &entry) : DecoratedTextStyle(base), myEntry(entry) {}
inline ForcedTextStyle::~ForcedTextStyle() {}
inline const std::string &ForcedTextStyle::fontFamily() const { return base()->fontFamily(); }
inline int ForcedTextStyle::fontSize() const { return base()->fontSize(); }
inline bool ForcedTextStyle::bold() const { return base()->bold(); }
inline bool ForcedTextStyle::italic() const { return base()->italic(); }
inline ZLColor ForcedTextStyle::color() const { return base()->color(); }
inline int ForcedTextStyle::spaceBefore() const { return base()->spaceBefore(); }
inline int ForcedTextStyle::spaceAfter() const { return base()->spaceAfter(); }
inline int ForcedTextStyle::firstLineIndentDelta() const { return base()->firstLineIndentDelta(); }
inline int ForcedTextStyle::verticalShift() const { return base()->verticalShift(); }
inline double ForcedTextStyle::lineSpace() const { return base()->lineSpace(); }
inline bool ForcedTextStyle::allowHyphenations() const { return base()->allowHyphenations(); }

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

inline FullDecoratedTextStyle::FullDecoratedTextStyle(const TextStylePtr base, const FullTextStyleDecoration &decoration) : DecoratedTextStyle(base), myDecoration(decoration) {}
inline FullDecoratedTextStyle::~FullDecoratedTextStyle() {}
inline int FullDecoratedTextStyle::spaceBefore() const { return myDecoration.spaceBeforeOption().value(); }
inline int FullDecoratedTextStyle::spaceAfter() const { return myDecoration.spaceAfterOption().value(); }
inline int FullDecoratedTextStyle::leftIndent() const { return base()->leftIndent() + myDecoration.leftIndentOption().value(); }
inline int FullDecoratedTextStyle::rightIndent() const { return base()->rightIndent() + myDecoration.rightIndentOption().value(); }
inline int FullDecoratedTextStyle::verticalShift() const { return base()->verticalShift() + myDecoration.verticalShiftOption().value(); }
inline double FullDecoratedTextStyle::lineSpace() const { double space = myDecoration.lineSpaceOption().value(); return (space == 0) ? base()->lineSpace() : space; }

inline TextStylePtr TextStyleCollection::baseStyle() const { return myBaseStyle; }

#endif /* __TEXTSTYLE_H__ */
