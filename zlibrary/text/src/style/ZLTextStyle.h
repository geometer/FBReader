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

#ifndef __ZLTEXTSTYLE_H__
#define __ZLTEXTSTYLE_H__

#include <string>

#include <shared_ptr.h>

#include <ZLTextAlignmentType.h>
#include <ZLTextParagraph.h>

class ZLTextStyle {

public:
	static const std::string REGULAR_TEXT;
	static const std::string SELECTION_BACKGROUND;
	static const std::string HIGHLIGHTED_TEXT;
	static const std::string TREE_LINES;

protected:
	ZLTextStyle();

public:
	virtual ~ZLTextStyle();

	virtual bool isDecorated() const = 0;

	virtual const std::string &fontFamily() const = 0;
	virtual int fontSize() const = 0;

	virtual bool bold() const = 0;
	virtual bool italic() const = 0;

	virtual const std::string &colorStyle() const = 0;

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

inline ZLTextStyle::ZLTextStyle() {}
inline ZLTextStyle::~ZLTextStyle() {}

inline int ZLTextStyle::lineSpacePercent() const { return (int)(lineSpace() * 100); }

#endif /* __ZLTEXTSTYLE_H__ */
