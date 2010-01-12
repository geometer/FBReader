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

#ifndef __ZLTEXTSTYLEIMPLEMENTATION_H__
#define __ZLTEXTSTYLEIMPLEMENTATION_H__

#include <ZLTextStyle.h>
#include <ZLOptions.h>

class ZLTextDecoratedStyle : public ZLTextStyle {

protected:
	ZLTextDecoratedStyle(const shared_ptr<ZLTextStyle> base);

public:
	virtual ~ZLTextDecoratedStyle();

	bool isDecorated() const;
	const shared_ptr<ZLTextStyle> base() const;

private:
	const shared_ptr<ZLTextStyle> myBase;
};

class ZLTextStyleEntry;

class ZLTextForcedStyle : public ZLTextDecoratedStyle {

public:
	ZLTextForcedStyle(shared_ptr<ZLTextStyle> base, const ZLTextStyleEntry &entry);
	~ZLTextForcedStyle();

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

private:
	const ZLTextStyleEntry &myEntry;
};

class ZLTextPartialDecoratedStyle : public ZLTextDecoratedStyle {

private:
	ZLTextPartialDecoratedStyle(const shared_ptr<ZLTextStyle> base, const ZLTextStyleDecoration &decoration);
	friend shared_ptr<ZLTextStyle> ZLTextStyleDecoration::createDecoratedStyle(const shared_ptr<ZLTextStyle> base) const;

public:
	virtual ~ZLTextPartialDecoratedStyle();
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
	bool allowHyphenations() const;

	double lineSpace() const;

private:
	const ZLTextStyleDecoration &myDecoration;
};

class ZLTextFullDecoratedStyle : public ZLTextDecoratedStyle {

private:
	ZLTextFullDecoratedStyle(const shared_ptr<ZLTextStyle> base, const ZLTextFullStyleDecoration &decoration);
	friend shared_ptr<ZLTextStyle> ZLTextFullStyleDecoration::createDecoratedStyle(const shared_ptr<ZLTextStyle> base) const;

public:
	~ZLTextFullDecoratedStyle();
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
	bool allowHyphenations() const;

	double lineSpace() const;

private:
	const ZLTextFullStyleDecoration &myDecoration;
};

inline ZLTextDecoratedStyle::ZLTextDecoratedStyle(const shared_ptr<ZLTextStyle> base) : myBase(base) {}
inline ZLTextDecoratedStyle::~ZLTextDecoratedStyle() {}
inline bool ZLTextDecoratedStyle::isDecorated() const { return true; }
inline const shared_ptr<ZLTextStyle> ZLTextDecoratedStyle::base() const { return myBase; }

inline ZLTextForcedStyle::ZLTextForcedStyle(shared_ptr<ZLTextStyle> base, const ZLTextStyleEntry &entry) : ZLTextDecoratedStyle(base), myEntry(entry) {}
inline ZLTextForcedStyle::~ZLTextForcedStyle() {}
inline const std::string &ZLTextForcedStyle::colorStyle() const { return base()->colorStyle(); }
inline int ZLTextForcedStyle::verticalShift() const { return base()->verticalShift(); }
inline double ZLTextForcedStyle::lineSpace() const { return base()->lineSpace(); }
inline bool ZLTextForcedStyle::allowHyphenations() const { return base()->allowHyphenations(); }

inline ZLTextPartialDecoratedStyle::ZLTextPartialDecoratedStyle(const shared_ptr<ZLTextStyle> base, const ZLTextStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
inline ZLTextPartialDecoratedStyle::~ZLTextPartialDecoratedStyle() {}
inline short ZLTextPartialDecoratedStyle::spaceBefore(const ZLTextStyleEntry::Metrics &metrics) const { return base()->spaceBefore(metrics); }
inline short ZLTextPartialDecoratedStyle::spaceAfter(const ZLTextStyleEntry::Metrics &metrics) const { return base()->spaceAfter(metrics); }
inline short ZLTextPartialDecoratedStyle::lineStartIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineStartIndent(metrics, rtl); }
inline short ZLTextPartialDecoratedStyle::lineEndIndent(const ZLTextStyleEntry::Metrics &metrics, bool rtl) const { return base()->lineEndIndent(metrics, rtl); }
inline short ZLTextPartialDecoratedStyle::firstLineIndentDelta(const ZLTextStyleEntry::Metrics &metrics) const { return base()->firstLineIndentDelta(metrics); }
inline int ZLTextPartialDecoratedStyle::verticalShift() const { return base()->verticalShift() + myDecoration.VerticalShiftOption.value(); }
inline ZLTextAlignmentType ZLTextPartialDecoratedStyle::alignment() const { return base()->alignment(); }
inline double ZLTextPartialDecoratedStyle::lineSpace() const { return base()->lineSpace(); }

inline ZLTextFullDecoratedStyle::ZLTextFullDecoratedStyle(const shared_ptr<ZLTextStyle> base, const ZLTextFullStyleDecoration &decoration) : ZLTextDecoratedStyle(base), myDecoration(decoration) {}
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

#endif /* __ZLTEXTSTYLEIMPLEMENTATION_H__ */
