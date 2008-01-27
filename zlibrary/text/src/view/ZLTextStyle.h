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

#ifndef __ZLTEXTSTYLE_H__
#define __ZLTEXTSTYLE_H__

#include <map>
#include <string>

#include <shared_ptr.h>

#include <ZLOptions.h>

#include <ZLTextKind.h>
#include <ZLTextAlignmentType.h>

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

	virtual int spaceBefore() const = 0;
	virtual int spaceAfter() const = 0;
	virtual int leftIndent() const = 0;
	virtual int rightIndent() const = 0;
	virtual int firstLineIndentDelta() const = 0;
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

	int spaceBefore() const;
	int spaceAfter() const;
	int leftIndent() const;
	int rightIndent() const;
	int firstLineIndentDelta() const;
	int verticalShift() const;

	ZLTextAlignmentType alignment() const;

	double lineSpace() const;

	bool allowHyphenations() const;

public:
	ZLColorOption BackgroundColorOption;
	ZLColorOption SelectionBackgroundColorOption;
	ZLColorOption SelectedTextColorOption;
	ZLColorOption RegularTextColorOption;
	ZLColorOption InternalHyperlinkTextColorOption;
	ZLColorOption ExternalHyperlinkTextColorOption;
	ZLColorOption TreeLinesColorOption;

	ZLBooleanOption AutoHyphenationOption;

	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeOption;
	ZLBooleanOption BoldOption;
	ZLBooleanOption ItalicOption;
	ZLIntegerOption AlignmentOption;
	ZLDoubleOption LineSpaceOption;
	ZLIntegerOption LineSpacePercentOption;
};

class ZLTextStyleDecoration {

public:
	ZLTextStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int verticalShift, ZLBoolean3 allowHyphenations);
	virtual ~ZLTextStyleDecoration();

	virtual bool isFullDecoration() const;

	virtual ZLTextStylePtr createDecoratedStyle(const ZLTextStylePtr base) const;

	const std::string &name() const;

	enum HyperlinkStyle {
		NONE,
		INTERNAL,
		EXTERNAL
	};
	HyperlinkStyle hyperlinkStyle() const;
	void setHyperlinkStyle(HyperlinkStyle hyperlinkStyle);

public:
	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeDeltaOption;
	ZLBoolean3Option BoldOption;
	ZLBoolean3Option ItalicOption;

	ZLIntegerOption VerticalShiftOption;

	ZLBoolean3Option AllowHyphenationsOption;

private:
	std::string myName;

	HyperlinkStyle myHyperlinkStyle;
};

class ZLTextFullStyleDecoration : public ZLTextStyleDecoration {

public:
	ZLTextFullStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, ZLTextAlignmentType alignment, double lineSpace, ZLBoolean3 allowHyphenations);
	~ZLTextFullStyleDecoration();

	virtual bool isFullDecoration() const;

	ZLTextStylePtr createDecoratedStyle(const ZLTextStylePtr base) const;

public:
	ZLIntegerRangeOption SpaceBeforeOption;
	ZLIntegerRangeOption SpaceAfterOption;
	ZLIntegerRangeOption LeftIndentOption;
	ZLIntegerRangeOption RightIndentOption;
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

class ZLTextForcedControlEntry;

class ZLTextForcedStyle : public ZLTextDecoratedStyle {

public:
	ZLTextForcedStyle(ZLTextStylePtr base, const ZLTextForcedControlEntry &entry);
	~ZLTextForcedStyle();

	const std::string &fontFamily() const;
	int fontSize() const;

	bool bold() const;
	bool italic() const;

	ZLColor color() const;

	int spaceBefore() const;
	int spaceAfter() const;
	int leftIndent() const;
	int rightIndent() const;
	int firstLineIndentDelta() const;
	int verticalShift() const;
	ZLTextAlignmentType alignment() const;
	double lineSpace() const;

	bool allowHyphenations() const;

private:
	const ZLTextForcedControlEntry &myEntry;
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

	int spaceBefore() const;
	int spaceAfter() const;
	int leftIndent() const;
	int rightIndent() const;
	int firstLineIndentDelta() const;
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

	int spaceBefore() const;
	int spaceAfter() const;
	int leftIndent() const;
	int rightIndent() const;
	int firstLineIndentDelta() const;
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

	virtual bool isVisible() const = 0;
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
inline int ZLTextBaseStyle::spaceBefore() const { return 0; }
inline int ZLTextBaseStyle::spaceAfter() const { return 0; }
inline int ZLTextBaseStyle::leftIndent() const { return 0; }
inline int ZLTextBaseStyle::rightIndent() const { return 0; }
inline int ZLTextBaseStyle::firstLineIndentDelta() const { return 0; }
inline int ZLTextBaseStyle::verticalShift() const { return 0; }
inline ZLTextAlignmentType ZLTextBaseStyle::alignment() const { return (ZLTextAlignmentType)AlignmentOption.value(); }
inline double ZLTextBaseStyle::lineSpace() const { return LineSpacePercentOption.value() / 100.0; }
inline bool ZLTextBaseStyle::allowHyphenations() const { return true; }

inline ZLTextStyleDecoration::~ZLTextStyleDecoration() {}
inline bool ZLTextStyleDecoration::isFullDecoration() const { return false; }
inline const std::string &ZLTextStyleDecoration::name() const { return myName; }
inline ZLTextStyleDecoration::HyperlinkStyle ZLTextStyleDecoration::hyperlinkStyle() const { return myHyperlinkStyle; }
inline void ZLTextStyleDecoration::setHyperlinkStyle(HyperlinkStyle hyperlinkStyle) { myHyperlinkStyle = hyperlinkStyle; }

inline ZLTextFullStyleDecoration::~ZLTextFullStyleDecoration() {}
inline bool ZLTextFullStyleDecoration::isFullDecoration() const { return true; }

inline ZLTextDecoratedStyle::ZLTextDecoratedStyle(const ZLTextStylePtr base) : myBase(base) {}
inline ZLTextDecoratedStyle::~ZLTextDecoratedStyle() {}
inline bool ZLTextDecoratedStyle::isDecorated() const { return true; }
inline const ZLTextStylePtr ZLTextDecoratedStyle::base() const { return myBase; }

inline ZLTextForcedStyle::ZLTextForcedStyle(ZLTextStylePtr base, const ZLTextForcedControlEntry &entry) : ZLTextDecoratedStyle(base), myEntry(entry) {}
inline ZLTextForcedStyle::~ZLTextForcedStyle() {}
inline const std::string &ZLTextForcedStyle::fontFamily() const { return base()->fontFamily(); }
inline int ZLTextForcedStyle::fontSize() const { return base()->fontSize(); }
inline bool ZLTextForcedStyle::bold() const { return base()->bold(); }
inline bool ZLTextForcedStyle::italic() const { return base()->italic(); }
inline ZLColor ZLTextForcedStyle::color() const { return base()->color(); }
inline int ZLTextForcedStyle::spaceBefore() const { return base()->spaceBefore(); }
inline int ZLTextForcedStyle::spaceAfter() const { return base()->spaceAfter(); }
inline int ZLTextForcedStyle::firstLineIndentDelta() const { return base()->firstLineIndentDelta(); }
inline int ZLTextForcedStyle::verticalShift() const { return base()->verticalShift(); }
inline double ZLTextForcedStyle::lineSpace() const { return base()->lineSpace(); }
inline bool ZLTextForcedStyle::allowHyphenations() const { return base()->allowHyphenations(); }

inline ZLTextPartialDecoratedStyle::ZLTextPartialDecoratedStyle(const ZLTextStylePtr base, const ZLTextStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
inline ZLTextPartialDecoratedStyle::~ZLTextPartialDecoratedStyle() {}
inline int ZLTextPartialDecoratedStyle::spaceBefore() const { return base()->spaceBefore(); }
inline int ZLTextPartialDecoratedStyle::spaceAfter() const { return base()->spaceAfter(); }
inline int ZLTextPartialDecoratedStyle::leftIndent() const { return base()->leftIndent(); }
inline int ZLTextPartialDecoratedStyle::rightIndent() const { return base()->rightIndent(); }
inline int ZLTextPartialDecoratedStyle::firstLineIndentDelta() const { return base()->firstLineIndentDelta(); }
inline int ZLTextPartialDecoratedStyle::verticalShift() const { return base()->verticalShift() + myDecoration.VerticalShiftOption.value(); }
inline ZLTextAlignmentType ZLTextPartialDecoratedStyle::alignment() const { return base()->alignment(); }
inline double ZLTextPartialDecoratedStyle::lineSpace() const { return base()->lineSpace(); }

inline ZLTextFullDecoratedStyle::ZLTextFullDecoratedStyle(const ZLTextStylePtr base, const ZLTextFullStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
inline ZLTextFullDecoratedStyle::~ZLTextFullDecoratedStyle() {}
inline int ZLTextFullDecoratedStyle::spaceBefore() const { return myDecoration.SpaceBeforeOption.value(); }
inline int ZLTextFullDecoratedStyle::spaceAfter() const { return myDecoration.SpaceAfterOption.value(); }
inline int ZLTextFullDecoratedStyle::leftIndent() const { return base()->leftIndent() + myDecoration.LeftIndentOption.value(); }
inline int ZLTextFullDecoratedStyle::rightIndent() const { return base()->rightIndent() + myDecoration.RightIndentOption.value(); }
inline int ZLTextFullDecoratedStyle::verticalShift() const { return base()->verticalShift() + myDecoration.VerticalShiftOption.value(); }
inline double ZLTextFullDecoratedStyle::lineSpace() const {
	const int spacing = myDecoration.LineSpacePercentOption.value();
	return (spacing == -1) ? base()->lineSpace() : (spacing / 100.0);
}

inline ZLTextStylePtr ZLTextStyleCollection::baseStylePtr() const { return myBaseStyle; }
inline ZLTextBaseStyle &ZLTextStyleCollection::baseStyle() const { return (ZLTextBaseStyle&)*myBaseStyle; }

#endif /* __ZLTEXTSTYLE_H__ */
