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

#ifndef __ZLTEXTLINEINFO_H__
#define __ZLTEXTLINEINFO_H__

#include <vector>

#include <shared_ptr.h>

#include <ZLTextStyle.h>
#include <ZLTextParagraphCursor.h>

struct ZLTextTreeNodeInfo {
	bool IsLeaf;
	bool IsOpen;
	bool IsFirstLine;
	int ParagraphNumber;
	std::vector<bool> VerticalLinesStack;
};

struct ZLTextLineInfo {
	ZLTextLineInfo(const ZLTextWordCursor &word, ZLTextStylePtr style);

	ZLTextWordCursor Start;
	ZLTextWordCursor RealStart;
	ZLTextWordCursor End;
	bool IsVisible;
	int LeftIndent;
	int Width;
	int Height;
	int Descent;
	int VSpaceAfter;
	int SpaceCounter;
	ZLTextStylePtr StartStyle;
	shared_ptr<ZLTextTreeNodeInfo> NodeInfo;

private:
	/* copy constructor & assignment are disabled */
	ZLTextLineInfo(const ZLTextLineInfo&);
	ZLTextLineInfo &operator = (const ZLTextLineInfo&);
};

class ZLTextLineInfoPtr : public shared_ptr<ZLTextLineInfo> {

public:
	ZLTextLineInfoPtr(ZLTextLineInfo *ptr);

	bool operator < (const ZLTextLineInfoPtr &info) const;
};

inline ZLTextLineInfo::ZLTextLineInfo(const ZLTextWordCursor &word, ZLTextStylePtr style) : Start(word), RealStart(word), End(word), IsVisible(false), LeftIndent(0), Width(0), Height(0), Descent(0), VSpaceAfter(0), SpaceCounter(0), StartStyle(style) {}

inline ZLTextLineInfoPtr::ZLTextLineInfoPtr(ZLTextLineInfo *ptr) : shared_ptr<ZLTextLineInfo>(ptr) {}
inline bool ZLTextLineInfoPtr::operator < (const ZLTextLineInfoPtr &info) const { return (*this)->Start < info->Start; }

#endif /* __ZLTEXTLINEINFO_H__ */
