/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLTEXTLINEINFO_H__
#define __ZLTEXTLINEINFO_H__

#include <vector>

#include <shared_ptr.h>

#include <ZLTextStyle.h>
#include <ZLTextParagraphCursor.h>

struct TreeNodeInfo {
	bool IsLeaf;
	bool IsOpen;
	bool IsFirstLine;
	int ParagraphNumber;
	std::vector<bool> VerticalLinesStack;
};

struct LineInfo {
	LineInfo(const WordCursor &word, ZLTextStylePtr style);

	WordCursor Start;
	WordCursor RealStart;
	WordCursor End;
	bool IsVisible;
	int LeftIndent;
	int Width;
	int Height;
	int Descent;
	int VSpaceAfter;
	int SpaceCounter;
	ZLTextStylePtr StartStyle;
	shared_ptr<TreeNodeInfo> NodeInfo;

private:
	/* copy constructor & assignment are disabled */
	LineInfo(const LineInfo&);
	LineInfo &operator = (const LineInfo&);
};

class LineInfoPtr : public shared_ptr<LineInfo> {

public:
	LineInfoPtr(LineInfo *ptr);

	bool operator < (const LineInfoPtr &info) const;
};

inline LineInfo::LineInfo(const WordCursor &word, ZLTextStylePtr style) : Start(word), RealStart(word), End(word), IsVisible(false), LeftIndent(0), Width(0), Height(0), Descent(0), VSpaceAfter(0), SpaceCounter(0), StartStyle(style) {}

inline LineInfoPtr::LineInfoPtr(LineInfo *ptr) : shared_ptr<LineInfo>(ptr) {}
inline bool LineInfoPtr::operator < (const LineInfoPtr &info) const { return (*this)->Start < info->Start; }

#endif /* __ZLTEXTLINEINFO_H__ */
