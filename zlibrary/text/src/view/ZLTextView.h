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

#ifndef __ZLTEXTVIEW_H__
#define __ZLTEXTVIEW_H__

#include <vector>
#include <set>
#include <string>
#include <algorithm>

#include <ZLTime.h>
#include <ZLView.h>
#include <ZLDialogManager.h>

#include <ZLTextWord.h>
#include <ZLTextPositionIndicatorInfo.h>
#include <ZLTextParagraphCursor.h>
#include <ZLTextSelectionModel.h>
#include <ZLTextRectangle.h>
#include <ZLTextParagraph.h>
#include <ZLTextArea.h>

class ZLTextModel;
class ZLTextMark;

class ZLTextLineInfo;
class ZLTextLineInfoPtr;
struct ZLTextTreeNodeInfo;

class ZLTextView : public ZLView {

public:
	static const int DOUBLE_CLICK_DELAY;

public:
	enum ScrollingMode {
		NO_OVERLAPPING,
		KEEP_LINES,
		SCROLL_LINES,
		SCROLL_PERCENTAGE
	};

public:
	class PositionIndicator;

public:
	static const std::string TYPE_ID;

protected:
	ZLTextView(ZLPaintContext &context);
	virtual ~ZLTextView();
	virtual shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo() const = 0;

private:
	const std::string &typeId() const;

public:
	const ZLTextArea &textArea() const;

	void clearCaches();
	void forceScrollbarUpdate();

	void gotoPage(size_t index);
	size_t pageIndex();
	size_t pageNumber() const;

	void scrollPage(bool forward, ScrollingMode mode, unsigned int value);
	void scrollToStartOfText();
	void scrollToEndOfText();

	void gotoMark(ZLTextMark mark);
	virtual void gotoParagraph(int num, bool end = false);
	void gotoPosition(int paragraphNumber, int wordNumber, int charNumber);

	const ZLTextWordCursor &startCursor() const;
	const ZLTextWordCursor &endCursor() const;

	virtual void setModel(shared_ptr<ZLTextModel> model);

	bool hasMultiSectionModel() const;
	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly);
	bool canFindNext() const;
	void findNext();
	bool canFindPrevious() const;
	void findPrevious();

	void highlightParagraph(int paragraphNumber);

	ZLTextSelectionModel &selectionModel();
	const ZLTextSelectionModel &selectionModel() const;
	void copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const;

	virtual bool isSelectionEnabled() const = 0;

protected:
	bool onStylusPress(int x, int y);
	bool onStylusMove(int x, int y);
	bool onStylusMovePressed(int x, int y);
	bool onStylusRelease(int x, int y);
	virtual bool onStylusClick(int x, int y, int count);
	void onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to);
	void onScrollbarStep(Direction direction, int steps);
	void onScrollbarPageStep(Direction direction, int steps);
	void activateSelection(int x, int y);

	virtual void paint();

	const ZLTextElementRectangle *elementByCoordinates(int x, int y) const;
	int paragraphIndexByCoordinates(int x, int y) const;

	void rebuildPaintInfo(bool strong);
	virtual void preparePaintInfo();

	void setStartCursor(ZLTextParagraphCursorPtr cursor);

	bool empty() const;

	virtual shared_ptr<PositionIndicator> createPositionIndicator(const ZLTextPositionIndicatorInfo&);

	virtual int leftMargin() const = 0;
	virtual int rightMargin() const = 0;
	virtual int topMargin() const = 0;
	virtual int bottomMargin() const = 0;
	virtual ZLColor color(const std::string &colorStyle = std::string()) const = 0;
	virtual shared_ptr<ZLTextStyle> baseStyle() const = 0;

private:
	int lineStartMargin() const;
	int lineEndMargin() const;
	int visualX(int logicalX) const;

	void moveStartCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);
	void moveEndCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);

	void clear();

	int rectangleBound(ZLTextArea::Style &style, const ZLTextParagraphCursor &paragraph, const ZLTextElementRectangle &rectangle, int toCharNumber, bool mainDir);
	ZLTextLineInfoPtr processTextLine(ZLTextArea::Style &style, const ZLTextWordCursor &start, const ZLTextWordCursor &end);
	void prepareTextLine(ZLTextArea::Style &style, const ZLTextLineInfo &info, int y);
	void drawTextLine(ZLTextArea::Style &style, const ZLTextLineInfo &info, int y, size_t from, size_t to);
	void drawSelectionRectangle(int left, int top, int right, int bottom);
	void drawWord(ZLTextArea::Style &style, int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign);
	void drawString(ZLTextArea::Style &style, int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift, bool rtl);
	void drawTreeLines(const ZLTextTreeNodeInfo &info, int x, int y, int height, int vSpaceAfter);

	bool pageIsEmpty() const;
	ZLTextWordCursor findLineFromStart(unsigned int overlappingValue) const;
	ZLTextWordCursor findLineFromEnd(unsigned int overlappingValue) const;
	ZLTextWordCursor findPercentFromStart(unsigned int percent) const;

	enum SizeUnit {
		PIXEL_UNIT,
		LINE_UNIT
	};
	int infoSize(const ZLTextLineInfo &info, SizeUnit unit);
	int paragraphSize(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit);
	void skip(ZLTextWordCursor &paragraph, SizeUnit unit, int size);
	ZLTextWordCursor findStart(const ZLTextWordCursor &end, SizeUnit unit, int textHeight);

	ZLTextWordCursor buildInfos(const ZLTextWordCursor &start);

	std::vector<size_t>::const_iterator nextBreakIterator() const;

	shared_ptr<ZLTextView::PositionIndicator> positionIndicator();

	int viewWidth() const;
	int viewHeight() const;
	int textHeight() const;

	void addRectangleToTextMap(ZLTextArea::Style &style, const ZLTextElementRectangle &rectangle);
	void flushRevertedElements(unsigned char bidiLevel);

	void gotoCharIndex(size_t charIndex);

private:
	ZLTextArea myTextArea;

	enum {
		NOTHING_TO_PAINT,
		READY,
		START_IS_KNOWN,
		END_IS_KNOWN,
		TO_SCROLL_FORWARD,
		TO_SCROLL_BACKWARD
	} myPaintState;
	ZLTextWordCursor myStartCursor;
	ZLTextWordCursor myEndCursor;
	std::vector<ZLTextLineInfoPtr> myLineInfos;
	std::set<ZLTextLineInfoPtr> myLineInfoCache;

	ScrollingMode myScrollingMode;
	unsigned int myOverlappingValue;

	ZLTextElementMap myTextElementMap;
	std::vector<ZLTextElementMap> myTextElementsToRevert;
	ZLTextTreeNodeMap myTreeNodeMap;

	std::vector<size_t> myTextSize;
	std::vector<size_t> myTextBreaks;

	ZLTextSelectionModel mySelectionModel;

	shared_ptr<PositionIndicator> myPositionIndicator;

	bool myTreeStateIsFrozen;
	bool myDoUpdateScrollbar;

	struct DoubleClickInfo {
		DoubleClickInfo();
		void update(int x, int y, bool press);

		int Count;
		ZLTime Time;
		int X;
		int Y;
	} myDoubleClickInfo;

friend class ZLTextSelectionModel;
};

inline const ZLTextArea &ZLTextView::textArea() const { return myTextArea; }

inline bool ZLTextView::empty() const { return myPaintState == NOTHING_TO_PAINT; }
inline const ZLTextWordCursor &ZLTextView::startCursor() const { return myStartCursor; }
inline const ZLTextWordCursor &ZLTextView::endCursor() const { return myEndCursor; }
inline ZLTextSelectionModel &ZLTextView::selectionModel() { return mySelectionModel; }
inline const ZLTextSelectionModel &ZLTextView::selectionModel() const { return mySelectionModel; }

inline int ZLTextView::viewWidth() const {
	return std::max(context().width() - leftMargin() - rightMargin(), 1);
}

inline int ZLTextView::viewHeight() const {
	return std::max(context().height() - topMargin() - bottomMargin(), 1);
}

inline int ZLTextView::visualX(int logicalX) const {
	return myTextArea.isRtl() ? context().width() - logicalX - 1 : logicalX;
}

inline int ZLTextView::lineStartMargin() const {
	return myTextArea.isRtl() ? rightMargin() : leftMargin();
}

inline int ZLTextView::lineEndMargin() const {
	return myTextArea.isRtl() ? leftMargin() : rightMargin();
}

#endif /* __ZLTEXTVIEW_H__ */
