/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTEXTSTYLE_H__
#define __ZLTEXTSTYLE_H__

#include <map>
#include <string>

#include <shared_ptr.h>

#include <ZLOptions.h>

#include <ZLTextKind.h>
#include <ZLTextAlignmentType.h>
#include <ZLTextParagraph.h>

class ZLTextStyle {

protected:
	ZLTextStyle();

public:
	virtual ~ZLTextStyle();

	virtual bool isDecorated() const = 0;

	virtual const std::string &fontFamily() const = 0;
	virtual int fontSize() const = 0;

	virtual bool bold() const = 0;
	virtual bool italic() const = 0;

	virtual ZLColor color() const = 0;

	virtual short spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const = 0;
	virtual short spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const = 0;
	virtual short lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const = 0;
	virtual short lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const = 0;
	virtual short firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const = 0;
	virtual int verticalShift() const = 0;
	virtual ZLTextAlignmentType alignment() const = 0;
	virtual double lineSpace() const = 0;

	virtual bool allowHyphenations() const = 0;

	int lineSpacePercent() const;
};

typedef shared_ptr<ZLTextStyle> ZLTextStylePtr;

class ZLTextBaseStyle : public ZLTextStyle {

public:
	ZLTextBaseStyle(const std::string &fontFamily, int fontSize);

	bool isDecorated() const;

	const std::string &fontFamily() const;

	int fontSize() const;
	bool bold() const;
	bool italic() const;

	ZLColor color() const;

	short spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const;
	short spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const;
	short lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const;
	int verticalShift() const;

	ZLTextAlignmentType alignment() const;

	double lineSpace() const;

	bool allowHyphenations() const;

public:
	ZLColorOption BackgroundColorOption;
	ZLColorOption SelectionBackgroundColorOption;
	ZLColorOption SelectedTextColorOption;
	ZLColorOption RegularTextColorOption;
	ZLColorOption TreeLinesColorOption;

	bool hasHyperlinkColorOption(const std::string &type);
	ZLColorOption &hyperlinkColorOption(const std::string &type);

	ZLBooleanOption AutoHyphenationOption;

	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeOption;
	ZLBooleanOption BoldOption;
	ZLBooleanOption ItalicOption;
	ZLIntegerOption AlignmentOption;
	ZLDoubleOption LineSpaceOption;
	ZLIntegerOption LineSpacePercentOption;

private:
	std::map<std::string,shared_ptr<ZLColorOption> > myHyperlinkColorOptions;
};

class ZLTextStyleDecoration {

public:
	ZLTextStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int verticalShift, ZLBoolean3 allowHyphenations);
	virtual ~ZLTextStyleDecoration();

	virtual bool isFullDecoration() const;

	virtual ZLTextStylePtr createDecoratedStyle(const ZLTextStylePtr base) const;

	const std::string &name() const;

	const std::string &hyperlinkStyle() const;
	void setHyperlinkStyle(const std::string &hyperlinkStyle);

public:
	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeDeltaOption;
	ZLBoolean3Option BoldOption;
	ZLBoolean3Option ItalicOption;

	ZLIntegerOption VerticalShiftOption;

	ZLBoolean3Option AllowHyphenationsOption;

private:
	std::string myName;

	std::string myHyperlinkStyle;
};

class ZLTextFullStyleDecoration : public ZLTextStyleDecoration {

public:
	ZLTextFullStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, short spaceBefore, short spaceAfter, short lineStartIndent, short lineEndIndent, short firstLineIndentDelta, int verticalShift, ZLTextAlignmentType alignment, double lineSpace, ZLBoolean3 allowHyphenations);
	~ZLTextFullStyleDecoration();

	virtual bool isFullDecoration() const;

	ZLTextStylePtr createDecoratedStyle(const ZLTextStylePtr base) const;

public:
	ZLIntegerRangeOption SpaceBeforeOption;
	ZLIntegerRangeOption SpaceAfterOption;
	ZLIntegerRangeOption LineStartIndentOption;
	ZLIntegerRangeOption LineEndIndentOption;
	ZLIntegerRangeOption FirstLineIndentDeltaOption;

	ZLIntegerOption AlignmentOption;

	ZLDoubleOption LineSpaceOption;
	ZLIntegerOption LineSpacePercentOption;
};

class ZLTextDecoratedStyle : public ZLTextStyle {

protected:
	ZLTextDecoratedStyle(const ZLTextStylePtr base);

public:
	virtual ~ZLTextDecoratedStyle();

	bool isDecorated() const;
	const ZLTextStylePtr base() const;

private:
	const ZLTextStylePtr myBase;
};

class ZLTextStyleEntry;

class ZLTextForcedStyle : public ZLTextDecoratedStyle {

public:
	ZLTextForcedStyle(ZLTextStylePtr base, const ZLTextStyleEntry &entry);
	~ZLTextForcedStyle();

	const std::string &fontFamily() const;
	int fontSize() const;

	bool bold() const;
	bool italic() const;

	ZLColor color() const;

	short spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const;
	short spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const;
	short lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const;
	int verticalShift() const;
	ZLTextAlignmentType alignment() const;
	double lineSpace() const;

	bool allowHyphenations() const;

private:
	const ZLTextStyleEntry &myEntry;
};

class ZLTextPartialDecoratedStyle : public ZLTextDecoratedStyle {

private:
	ZLTextPartialDecoratedStyle(const ZLTextStylePtr base, const ZLTextStyleDecoration &decoration);
	friend ZLTextStylePtr ZLTextStyleDecoration::createDecoratedStyle(const ZLTextStylePtr base) const;

public:
	virtual ~ZLTextPartialDecoratedStyle();
	const std::string &fontFamily() const;
	int fontSize() const;
	bool bold() const;
	bool italic() const;

	ZLColor color() const;

	short spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const;
	short spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const;
	short lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const;
	int verticalShift() const;

	ZLTextAlignmentType alignment() const;
	bool allowHyphenations() const;

	double lineSpace() const;

private:
	const ZLTextStyleDecoration &myDecoration;
};

class ZLTextFullDecoratedStyle : public ZLTextDecoratedStyle {

private:
	ZLTextFullDecoratedStyle(const ZLTextStylePtr base, const ZLTextFullStyleDecoration &decoration);
	friend ZLTextStylePtr ZLTextFullStyleDecoration::createDecoratedStyle(const ZLTextStylePtr base) const;

public:
	~ZLTextFullDecoratedStyle();
	const std::string &fontFamily() const;
	int fontSize() const;
	bool bold() const;
	bool italic() const;

	ZLColor color() const;

	short spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const;
	short spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const;
	short lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const;
	short firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const;
	int verticalShift() const;

	ZLTextAlignmentType alignment() const;
	bool allowHyphenations() const;

	double lineSpace() const;

private:
	const ZLTextFullStyleDecoration &myDecoration;
};

class ZLTextPositionIndicatorInfo {

protected:
	ZLTextPositionIndicatorInfo();

public:
	virtual ~ZLTextPositionIndicatorInfo();

	enum Type {
		OS_SCROLLBAR = 0,
		FB_INDICATOR = 1,
		NONE = 2
	};

	virtual Type type() const = 0;
	virtual bool isSensitive() const = 0;
	virtual bool isTextPositionShown() const = 0;
	virtual bool isTimeShown() const = 0;
	virtual ZLColor color() const = 0;
	virtual int height() const = 0;
	virtual int offset() const = 0;
	virtual int fontSize() const = 0;

private:
	ZLTextPositionIndicatorInfo(const ZLTextPositionIndicatorInfo&);
	const ZLTextPositionIndicatorInfo operator = (const ZLTextPositionIndicatorInfo&);
};

class ZLTextStyleCollection {

public:
	static ZLTextStyleCollection &instance();
	static void deleteInstance();

	ZLTextStylePtr baseStylePtr() const;
	ZLTextBaseStyle &baseStyle() const;
	ZLTextStyleDecoration *decoration(ZLTextKind kind) const;

	ZLBooleanOption OverrideSpecifiedFontsOption;

private:
	ZLTextStyleCollection();
	~ZLTextStyleCollection();

private:
	static ZLTextStyleCollection *ourInstance;
	
private:
	ZLTextStylePtr myBaseStyle;
	std::map<ZLTextKind,ZLTextStyleDecoration*> myDecorationMap;

friend class ZLTextStyleReader;
};

inline ZLTextStyle::ZLTextStyle() {}
inline ZLTextStyle::~ZLTextStyle() {}

inline int ZLTextStyle::lineSpacePercent() const { return (int)(lineSpace() * 100); }

inline bool ZLTextBaseStyle::isDecorated() const { return false; }
inline const std::string &ZLTextBaseStyle::fontFamily() const { return FontFamilyOption.value(); }
inline int ZLTextBaseStyle::fontSize() const { return FontSizeOption.value(); }
inline bool ZLTextBaseStyle::bold() const { return BoldOption.value(); }
inline bool ZLTextBaseStyle::italic() const { return ItalicOption.value(); }
inline short ZLTextBaseStyle::spaceBefore(const ZLTextStyleEntry::Metrics&) const { return 0; }
inline short ZLTextBaseStyle::spaceAfter(const ZLTextStyleEntry::Metrics&) const { return 0; }
inline short ZLTextBaseStyle::lineStartIndent(const ZLTextStyleEntry::Metrics&, bool) const { return 0; }
inline short ZLTextBaseStyle::lineEndIndent(const ZLTextStyleEntry::Metrics&, bool) const { return 0; }
inline short ZLTextBaseStyle::firstLineIndentDelta(const ZLTextStyleEntry::Metrics&) const { return 0; }
inline int ZLTextBaseStyle::verticalShift() const { return 0; }
inline ZLTextAlignmentType ZLTextBaseStyle::alignment() const { return (ZLTextAlignmentType)AlignmentOption.value(); }
inline double ZLTextBaseStyle::lineSpace() const { return LineSpacePercentOption.value() / 100.0; }
inline bool ZLTextBaseStyle::allowHyphenations() const { return true; }

inline ZLTextStyleDecoration::~ZLTextStyleDecoration() {}
inline bool ZLTextStyleDecoration::isFullDecoration() const { return false; }
inline const std::string &ZLTextStyleDecoration::name() const { return myName; }

inline ZLTextFullStyleDecoration::~ZLTextFullStyleDecoration() {}
inline bool ZLTextFullStyleDecoration::isFullDecoration() const { return true; }

inline ZLTextDecoratedStyle::ZLTextDecoratedStyle(const ZLTextStylePtr base) : myBase(base) {}
inline ZLTextDecoratedStyle::~ZLTextDecoratedStyle() {}
inline bool ZLTextDecoratedStyle::isDecorated() const { return true; }
inline const ZLTextStylePtr ZLTextDecoratedStyle::base() const { return myBase; }

inline ZLTextForcedStyle::ZLTextForcedStyle(ZLTextStylePtr base, const ZLTextStyleEntry &entry) : ZLTextDecoratedStyle(base), myEntry(entry) {}
inline ZLTextForcedStyle::~ZLTextForcedStyle() {}
inline ZLColor ZLTextForcedStyle::color() const { return base()->color(); }
inline int ZLTextForcedStyle::verticalShift() const { return base()->verticalShift(); }
inline double ZLTextForcedStyle::lineSpace() const { return base()->lineSpace(); }
inline bool ZLTextForcedStyle::allowHyphenations() const { return base()->allowHyphenations(); }

inline ZLTextPartialDecoratedStyle::ZLTextPartialDecoratedStyle(const ZLTextStylePtr base, const ZLTextStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
inline ZLTextPartialDecoratedStyle::~ZLTextPartialDecoratedStyle() {}
inline short ZLTextPartialDecoratedStyle::spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const { return base()->spaceBefore(metrics); }
inline short ZLTextPartialDecoratedStyle::spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const { return base()->spaceAfter(metrics); }
inline short ZLTextPartialDecoratedStyle::lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineStartIndent(metrics, rtl); }
inline short ZLTextPartialDecoratedStyle::lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineEndIndent(metrics, rtl); }
inline short ZLTextPartialDecoratedStyle::firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const { return base()->firstLineIndentDelta(metrics); }
inline int ZLTextPartialDecoratedStyle::verticalShift() const { return base()->verticalShift() + myDecoration.VerticalShiftOption.value(); }
inline ZLTextAlignmentType ZLTextPartialDecoratedStyle::alignment() const { return base()->alignment(); }
inline double ZLTextPartialDecoratedStyle::lineSpace() const { return base()->lineSpace(); }

inline ZLTextFullDecoratedStyle::ZLTextFullDecoratedStyle(const ZLTextStylePtr base, const ZLTextFullStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
inline ZLTextFullDecoratedStyle::~ZLTextFullDecoratedStyle() {}
inline short ZLTextFullDecoratedStyle::spaceBefore(const ZLTextStyleEntry::Metrics&) const { return myDecoration.SpaceBeforeOption.value(); }
inline short ZLTextFullDecoratedStyle::spaceAfter(const ZLTextStyleEntry::Metrics&) const { return myDecoration.SpaceAfterOption.value(); }
inline short ZLTextFullDecoratedStyle::lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineStartIndent(metrics, rtl) + myDecoration.LineStartIndentOption.value(); }
inline short ZLTextFullDecoratedStyle::lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineEndIndent(metrics, rtl) + myDecoration.LineEndIndentOption.value(); }
inline int ZLTextFullDecoratedStyle::verticalShift() const { return base()->verticalShift() + myDecoration.VerticalShiftOption.value(); }
inline double ZLTextFullDecoratedStyle::lineSpace() const {
	const int spacing = myDecoration.LineSpacePercentOption.value();
	return (spacing == -1) ? base()->lineSpace() : (spacing / 100.0);
}

inline ZLTextStylePtr ZLTextStyleCollection::baseStylePtr() const { return myBaseStyle; }
inline ZLTextBaseStyle &ZLTextStyleCollection::baseStyle() const { return (ZLTextBaseStyle&)*myBaseStyle; }

#endif /* __ZLTEXTSTYLE_H__ */
