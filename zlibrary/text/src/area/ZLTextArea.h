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

class ZLTextLineInfo;
class ZLTextLineInfoPtr;
struct ZLTextTreeNodeInfo;
class ZLTextSelectionModel;

class ZLTextArea {

public:
	class Style;

	class Properties {

	public:
		~Properties();
		virtual shared_ptr<ZLTextStyle> baseStyle() const = 0;
		virtual ZLColor color(const std::string &style = std::string()) const = 0;
		virtual bool isSelectionEnabled() const = 0;
	};

public:
	ZLTextArea(ZLPaintContext &context, const Properties &properties);
	~ZLTextArea();

public:
	ZLPaintContext &context() const;
	void setOffsets(size_t hOffset, size_t vOffset);
	void setSize(size_t width, size_t height);
	size_t hOffset() const;
	size_t vOffset() const;
	size_t width() const;
	size_t height() const;

	void setModel(shared_ptr<ZLTextModel> model);
	shared_ptr<ZLTextModel> model() const;
	bool isRtl() const;
	int realX(int X) const;

	const ZLTextWordCursor &startCursor() const;
	const ZLTextWordCursor &endCursor() const;
	bool isEmpty() const;

	const ZLTextElementRectangle *elementByCoordinates(int x, int y, bool absolute = true) const;
	const ZLTextTreeNodeRectangle *treeNodeByCoordinates(int x, int y, bool absolute = true) const;
	int paragraphIndexByCoordinates(int x, int y, bool absolute = true) const;

	ZLTextSelectionModel &selectionModel();

	void paint();

private:
	void clear();

	ZLTextLineInfoPtr processTextLine(Style &style, const ZLTextWordCursor &start, const ZLTextWordCursor &end);

	void prepareTextLine(Style &style, const ZLTextLineInfo &info, int y);

	void drawTextLine(Style &style, const ZLTextLineInfo &info, int y, size_t from, size_t to);
	void drawWord(Style &style, int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign);
	void drawString(Style &style, int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift, bool rtl);
	void drawSelectionRectangle(int left, int top, int right, int bottom);
	void drawTreeLines(const ZLTextTreeNodeInfo &info, int x, int y, int height, int vSpaceAfter);

	int rectangleBound(Style &style, const ZLTextParagraphCursor &paragraph, const ZLTextElementRectangle &rectangle, int toCharNumber, bool mainDir);
	void addRectangleToTextMap(Style &style, const ZLTextElementRectangle &rectangle);
	void flushRevertedElements(unsigned char bidiLevel);

private:
	ZLPaintContext &myContext;
	const Properties &myProperties;
	shared_ptr<ZLMirroredPaintContext> myMirroredContext;
	size_t myHOffset;
	size_t myVOffset;
	size_t myWidth;
	size_t myHeight;

	shared_ptr<ZLTextModel> myModel;

public: // TODO: change to private
	ZLTextWordCursor myStartCursor;
	ZLTextWordCursor myEndCursor;

private:
	std::vector<ZLTextLineInfoPtr> myLineInfos;
	std::set<ZLTextLineInfoPtr> myLineInfoCache;

	ZLTextElementMap myTextElementMap;
	std::vector<ZLTextElementMap> myTextElementsToRevert;
	ZLTextTreeNodeMap myTreeNodeMap;

	shared_ptr<ZLTextSelectionModel> mySelectionModel;

friend class ZLTextAreaController;
friend class ZLTextSelectionModel;
};

inline ZLTextArea::Properties::~Properties() {}

inline ZLPaintContext &ZLTextArea::context() const { return myMirroredContext.isNull() ? myContext : (ZLPaintContext&)*myMirroredContext; }
inline void ZLTextArea::setSize(size_t width, size_t height) { myWidth = width; myHeight = height; }
inline size_t ZLTextArea::width() const { return myWidth; }
inline size_t ZLTextArea::height() const { return myHeight; }
inline void ZLTextArea::setOffsets(size_t hOffset, size_t vOffset) { myHOffset = hOffset; myVOffset = vOffset; }
inline size_t ZLTextArea::hOffset() const { return myHOffset; }
inline size_t ZLTextArea::vOffset() const { return myVOffset; }

inline shared_ptr<ZLTextModel> ZLTextArea::model() const { return myModel; }
inline bool ZLTextArea::isRtl() const { return !myMirroredContext.isNull(); }

inline const ZLTextWordCursor &ZLTextArea::startCursor() const { return myStartCursor; }
inline const ZLTextWordCursor &ZLTextArea::endCursor() const { return myEndCursor; }
inline bool ZLTextArea::isEmpty() const { return myStartCursor.isNull() && myEndCursor.isNull(); }

#endif /* __ZLTEXTAREA_H__ */
