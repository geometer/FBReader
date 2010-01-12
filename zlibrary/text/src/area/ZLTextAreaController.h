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

#ifndef __ZLTEXTAREACONTROLLER_H__
#define __ZLTEXTAREACONTROLLER_H__

#include <vector>
#include <set>

#include <shared_ptr.h>

#include <ZLTextParagraphCursor.h>
#include <ZLTextRectangle.h>

#include "ZLTextArea.h"

class ZLTextAreaController {

public:
	enum ScrollingMode {
		NO_OVERLAPPING,
		KEEP_LINES,
		SCROLL_LINES,
		SCROLL_PERCENTAGE
	};

public:
	ZLTextAreaController(ZLPaintContext &context, const ZLTextArea::Properties &properties);
	~ZLTextAreaController();

	const ZLTextArea &area() const;
	ZLTextArea &area();

	void setModel(shared_ptr<ZLTextModel> model);
	void clear();
	bool preparePaintInfo();
	void rebuildPaintInfo(bool strong);

	void scrollPage(bool forward, ScrollingMode mode, unsigned int value);
	void moveStartCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);
	void moveEndCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);

private:
	enum SizeUnit {
		PIXEL_UNIT,
		LINE_UNIT
	};

	bool visiblePageIsEmpty() const;

	ZLTextWordCursor buildInfos(const ZLTextWordCursor &start);
	ZLTextWordCursor findStart(const ZLTextWordCursor &end, SizeUnit unit, int textHeight);

	ZLTextWordCursor findLineFromStart(unsigned int overlappingValue) const;
	ZLTextWordCursor findLineFromEnd(unsigned int overlappingValue) const;
	ZLTextWordCursor findPercentFromStart(unsigned int percent) const;

	int infoHeight(const ZLTextLineInfo &info, SizeUnit unit);
	int paragraphHeight(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit);
	void skip(ZLTextWordCursor &paragraph, SizeUnit unit, int size);

private:
	ZLTextArea myArea;

	enum {
		NOTHING_TO_PAINT,
		READY,
		START_IS_KNOWN,
		END_IS_KNOWN,
		TO_SCROLL_FORWARD,
		TO_SCROLL_BACKWARD
	} myPaintState;
	ScrollingMode myScrollingMode;
	unsigned int myOverlappingValue;
};

inline const ZLTextArea &ZLTextAreaController::area() const { return myArea; }
inline ZLTextArea &ZLTextAreaController::area() { return myArea; }

#endif /* __ZLTEXTAREACONTROLLER_H__ */
