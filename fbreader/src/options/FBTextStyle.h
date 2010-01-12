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

#ifndef __FBTEXTSTYLE_H__
#define __FBTEXTSTYLE_H__

#include <ZLTextStyle.h>

class FBTextStyle : public ZLTextStyle {

public:
	static shared_ptr<ZLTextStyle> InstanceAsPtr();
	static FBTextStyle &Instance();

private:
	static shared_ptr<ZLTextStyle> ourInstance;

private:
	FBTextStyle();

public:
	bool isDecorated() const;

	const std::string &fontFamily() const;

	int fontSize() const;
	bool bold() const;
	bool italic() const;

	const std::string &colorStyle() const;

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
	ZLStringOption FontFamilyOption;
	ZLIntegerRangeOption FontSizeOption;
	ZLBooleanOption BoldOption;
	ZLBooleanOption ItalicOption;
	ZLIntegerOption AlignmentOption;
	ZLDoubleOption LineSpaceOption;
	ZLIntegerOption LineSpacePercentOption;
};

#endif /* __FBTEXTSTYLE_H__ */
