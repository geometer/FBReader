/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTEXTSTYLECOLLECTION_H__
#define __ZLTEXTSTYLECOLLECTION_H__

#include <map>

#include <ZLOptions.h>
#include <ZLTextStyle.h>

class ZLTextStyleDecoration {

public:
	ZLTextStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int verticalShift, ZLBoolean3 allowHyphenations);
	virtual ~ZLTextStyleDecoration();

	virtual bool isFullDecoration() const;

	virtual shared_ptr<ZLTextStyle> createDecoratedStyle(const shared_ptr<ZLTextStyle> base) const;

	const std::string &name() const;

	const std::string &colorStyle() const;
	void setColorStyle(const std::string &colorStyle);

public:
	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeDeltaOption;
	ZLBoolean3Option BoldOption;
	ZLBoolean3Option ItalicOption;

	ZLIntegerOption VerticalShiftOption;

	ZLBoolean3Option AllowHyphenationsOption;

private:
	std::string myName;

	std::string myColorStyle;
};

class ZLTextFullStyleDecoration : public ZLTextStyleDecoration {

public:
	ZLTextFullStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, short spaceBefore, short spaceAfter, short lineStartIndent, short lineEndIndent, short firstLineIndentDelta, int verticalShift, ZLTextAlignmentType alignment, double lineSpace, ZLBoolean3 allowHyphenations);
	~ZLTextFullStyleDecoration();

	virtual bool isFullDecoration() const;

	shared_ptr<ZLTextStyle> createDecoratedStyle(const shared_ptr<ZLTextStyle> base) const;

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

class ZLTextStyleCollection {

public:
	static ZLTextStyleCollection &Instance();
	static void deleteInstance();

	ZLTextStyleDecoration *decoration(ZLTextKind kind) const;

	ZLBooleanOption AutoHyphenationOption;
	ZLBooleanOption OverrideSpecifiedFontsOption;

private:
	ZLTextStyleCollection();
	~ZLTextStyleCollection();

private:
	static ZLTextStyleCollection *ourInstance;
	
private:
	std::map<ZLTextKind,ZLTextStyleDecoration*> myDecorationMap;

friend class ZLTextStyleReader;
};

inline ZLTextStyleDecoration::~ZLTextStyleDecoration() {}
inline bool ZLTextStyleDecoration::isFullDecoration() const { return false; }
inline const std::string &ZLTextStyleDecoration::name() const { return myName; }

inline ZLTextFullStyleDecoration::~ZLTextFullStyleDecoration() {}
inline bool ZLTextFullStyleDecoration::isFullDecoration() const { return true; }

#endif /* __ZLTEXTSTYLECOLLECTION_H__ */
