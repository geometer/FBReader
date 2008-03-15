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

#ifndef __ZLTEXTVIEW_H__
#define __ZLTEXTVIEW_H__

#include <vector>
#include <set>
#include <string>
#include <algorithm>

#include <ZLOptions.h>
#include <ZLView.h>
#include <ZLDialogManager.h>

#include <ZLTextWord.h>
#include <ZLTextStyle.h>
#include <ZLTextParagraphCursor.h>
#include <ZLTextSelectionModel.h>
#include <ZLTextArea.h>

class ZLTextModel;
class ZLTextMark;

class ZLTextLineInfo;
class ZLTextLineInfoPtr;
struct ZLTextTreeNodeInfo;
struct ZLTextElementArea;

class ZLTextView : public ZLView {

public:
	enum ScrollingMode {
		NO_OVERLAPPING,
		KEEP_LINES,
		SCROLL_LINES,
		SCROLL_PERCENTAGE
	};

public:
	class PositionIndicator {

	public:
		PositionIndicator(ZLTextView &textView, const ZLTextPositionIndicatorInfo &info);
		virtual ~PositionIndicator();

		virtual void draw();
		bool onStylusPress(int x, int y);

	protected:
		const ZLTextView &textView() const;
		ZLPaintContext &context() const;
		int top() const;
		int bottom() const;
		int left() const;
		int right() const;

		const std::vector<size_t> &textSize() const;
		size_t startTextIndex() const;
		size_t endTextIndex() const;

	private:
		void drawExtraText(const std::string &text);
		std::string textPositionString() const;
		std::string timeString() const;
		size_t sizeOfParagraph(size_t paragraphNumber) const;
		size_t sizeOfTextBeforeParagraph(size_t paragraphNumber) const;
		size_t sizeOfTextBeforeCursor() const;

	private:
		ZLTextView &myTextView;
		const ZLTextPositionIndicatorInfo &myInfo;
		int myExtraWidth;
	};

	friend class ZLTextView::PositionIndicator;
	
private:
	class ViewStyle {

	public:
		ViewStyle(shared_ptr<ZLPaintContext> context);
		~ViewStyle();
		void setPaintContext(shared_ptr<ZLPaintContext> context);

		void reset();
		void setTextStyle(const ZLTextStylePtr style);
		void applyControl(const ZLTextControlElement &control);
		void applyControl(const ZLTextForcedControlElement &control);
		void applyControls(const ZLTextWordCursor &begin, const ZLTextWordCursor &end);

		const ZLPaintContext &context() const;
		const ZLTextStylePtr textStyle() const;
		int elementWidth(const ZLTextElement &element, unsigned int charNumber) const;
		int elementHeight(const ZLTextElement &element) const;
		int elementDescent(const ZLTextElement &element) const;

		int wordWidth(const ZLTextWord &word, int start = 0, int length = -1, bool addHyphenationSign = false) const;

	private:
		ZLTextStylePtr myTextStyle;
		shared_ptr<ZLPaintContext> myContext;
		mutable int myWordHeight;
	};

protected:
	ZLTextView(ZLApplication &application, shared_ptr<ZLPaintContext> context = 0);
	virtual ~ZLTextView();
	void setPaintContext(shared_ptr<ZLPaintContext> context);
	virtual shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo() const = 0;

public:
	void clearCaches();

	void gotoPage(size_t index);
	size_t pageNumber() const;

	void scrollPage(bool forward, ScrollingMode mode, unsigned int value);
	void scrollToStartOfText();
	void scrollToEndOfText();

	void gotoMark(ZLTextMark mark);
	virtual void gotoParagraph(int num, bool last = false);
	void gotoPosition(int paragraphNumber, int wordNumber, int charNumber);

	const ZLTextWordCursor &startCursor() const;
	const ZLTextWordCursor &endCursor() const;

	virtual void setModel(shared_ptr<ZLTextModel> model);
	const shared_ptr<ZLTextModel> model() const;

	bool hasMultiSectionModel() const;
	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly);
	bool canFindNext() const;
	void findNext();
	bool canFindPrevious() const;
	void findPrevious();

	void highlightParagraph(int paragraphNumber);

	ZLTextSelectionModel &selectionModel();
	void copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const;
	
protected:
	bool onStylusPress(int x, int y);
	bool onStylusMovePressed(int x, int y);
	bool onStylusRelease(int x, int y);
	void activateSelection(int x, int y);

	virtual void paint();

	int paragraphIndexByCoordinate(int y) const;
	const ZLTextElementArea *elementByCoordinates(int x, int y) const;

	void rebuildPaintInfo(bool strong);
	virtual void preparePaintInfo();

	void setStartCursor(ZLTextParagraphCursorPtr cursor);

	bool empty() const;

	virtual shared_ptr<PositionIndicator> createPositionIndicator(const ZLTextPositionIndicatorInfo&);

	virtual int leftMargin() const = 0;
	virtual int rightMargin() const = 0;
	virtual int topMargin() const = 0;
	virtual int bottomMargin() const = 0;
	virtual bool isSelectionEnabled() const = 0;

private:
	void moveStartCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);
	void moveEndCursor(int paragraphNumber, int wordNumber = 0, int charNumber = 0);

	void clear();

	int areaLength(const ZLTextParagraphCursor &paragraph, const ZLTextElementArea &area, int toCharNumber);
	ZLTextLineInfoPtr processTextLine(const ZLTextWordCursor &start, const ZLTextWordCursor &end);
	void prepareTextLine(const ZLTextLineInfo &info);
	void drawTextLine(const ZLTextLineInfo &info, size_t from, size_t to);
	void drawWord(int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign);
	void drawString(int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift);
	void drawTreeLines(const ZLTextTreeNodeInfo &info, int height, int vSpaceAfter);

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
	int textAreaHeight() const;

private:
	shared_ptr<ZLTextModel> myModel;

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

	int myOldWidth, myOldHeight;

	ZLTextElementMap myTextElementMap;
	ZLTextTreeNodeMap myTreeNodeMap;

	std::vector<size_t> myTextSize;
	std::vector<size_t> myTextBreaks;

	ViewStyle myStyle;
	ZLTextSelectionModel mySelectionModel;

	shared_ptr<PositionIndicator> myPositionIndicator;

	bool myTreeStateIsFrozen;

friend class ZLTextSelectionModel;
};

inline ZLTextView::ViewStyle::~ViewStyle() {}
inline const ZLPaintContext &ZLTextView::ViewStyle::context() const { return *myContext; }
inline const ZLTextStylePtr ZLTextView::ViewStyle::textStyle() const { return myTextStyle; }

inline bool ZLTextView::empty() const { return myPaintState == NOTHING_TO_PAINT; }
inline const ZLTextWordCursor &ZLTextView::startCursor() const { return myStartCursor; }
inline const ZLTextWordCursor &ZLTextView::endCursor() const { return myEndCursor; }
inline const shared_ptr<ZLTextModel> ZLTextView::model() const { return myModel; }
inline ZLTextSelectionModel &ZLTextView::selectionModel() { return mySelectionModel; }

inline int ZLTextView::viewWidth() const {
	return std::max(myStyle.context().width() - leftMargin() - rightMargin(), 1);
}

inline int ZLTextView::viewHeight() const {
	return std::max(myStyle.context().height() - topMargin() - bottomMargin(), 1);
}

#endif /* __ZLTEXTVIEW_H__ */
