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

#ifndef __ZLTEXTAREA_H__
#define __ZLTEXTAREA_H__

#include <vector>
#include <set>

#include <shared_ptr.h>

#include <ZLTextParagraphCursor.h>
#include <ZLTextRectangle.h>

class ZLPaintContext;
class ZLMirroredPaintContext;

class ZLTextModel;

class ZLTextLineInfoPtr;

class ZLTextArea {

public:
	class Style;

public:
	ZLTextArea(ZLPaintContext &context);
	~ZLTextArea();

public:
	ZLPaintContext &context() const;
	void setSize(size_t width, size_t height);
	size_t width() const;
	size_t height() const;

	void setModel(shared_ptr<ZLTextModel> model);
	shared_ptr<ZLTextModel> model() const;
	bool isRtl() const;
	int realX(int X) const;

	void clear();
	const ZLTextWordCursor &startCursor() const;
	const ZLTextWordCursor &endCursor() const;
	bool isEmpty() const;

public:
	ZLTextLineInfoPtr processTextLine(ZLTextArea::Style &style, const ZLTextWordCursor &start, const ZLTextWordCursor &end);

private:
	ZLPaintContext &myContext;
	shared_ptr<ZLMirroredPaintContext> myMirroredContext;
	size_t myWidth;
	size_t myHeight;

	shared_ptr<ZLTextModel> myModel;

public:
	ZLTextWordCursor myStartCursor;
	ZLTextWordCursor myEndCursor;

	std::vector<ZLTextLineInfoPtr> myLineInfos;
	std::set<ZLTextLineInfoPtr> myLineInfoCache;

	ZLTextElementMap myTextElementMap;
	std::vector<ZLTextElementMap> myTextElementsToRevert;
	ZLTextTreeNodeMap myTreeNodeMap;
};

inline ZLPaintContext &ZLTextArea::context() const { return myMirroredContext.isNull() ? myContext : (ZLPaintContext&)*myMirroredContext; }
inline void ZLTextArea::setSize(size_t width, size_t height) { myWidth = width; myHeight = height; }
inline size_t ZLTextArea::width() const { return myWidth; }
inline size_t ZLTextArea::height() const { return myHeight; }

inline shared_ptr<ZLTextModel> ZLTextArea::model() const { return myModel; }
inline bool ZLTextArea::isRtl() const { return !myMirroredContext.isNull(); }

inline const ZLTextWordCursor &ZLTextArea::startCursor() const { return myStartCursor; }
inline const ZLTextWordCursor &ZLTextArea::endCursor() const { return myEndCursor; }
inline bool ZLTextArea::isEmpty() const { return myStartCursor.isNull() && myEndCursor.isNull(); }

#endif /* __ZLTEXTAREA_H__ */
