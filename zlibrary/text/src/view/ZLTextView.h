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

#ifndef __ZLTEXTVIEW_H__
#define __ZLTEXTVIEW_H__

#include <vector>
#include <set>
#include <string>
#include <algorithm>

#include <ZLTime.h>
#include <ZLView.h>

#include <ZLTextWord.h>
#include <ZLTextPositionIndicatorInfo.h>
#include <ZLTextParagraphCursor.h>
#include <ZLTextRectangle.h>
#include <ZLTextParagraph.h>
#include <ZLTextAreaController.h>

class ZLRunnable;

class ZLTextModel;
class ZLTextMark;

class ZLTextLineInfo;
class ZLTextLineInfoPtr;
class ZLTextSelectionModel;

class ZLTextView : public ZLView, public ZLTextArea::Properties {

public:
	class PositionIndicator;

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLTextView(ZLPaintContext &context);
	virtual ~ZLTextView();
	virtual shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo() const = 0;

private:
	const ZLTypeId &typeId() const;

public:
	const ZLTextArea &textArea() const;
	ZLTextSelectionModel &selectionModel();

	void clearCaches();
	void forceScrollbarUpdate();

	void gotoPage(size_t index);
	size_t pageIndex();
	size_t pageNumber() const;

	void scrollPage(bool forward, ZLTextAreaController::ScrollingMode mode, unsigned int value);
	void scrollToStartOfText();
	void scrollToEndOfText();

	void gotoMark(ZLTextMark mark);
	virtual void gotoParagraph(int num, bool end = false);
	void gotoPosition(int paragraphNumber, int wordNumber, int charNumber);

	virtual void setModel(shared_ptr<ZLTextModel> model);

	bool hasMultiSectionModel() const;
	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly);
	bool canFindNext() const;
	void findNext();
	bool canFindPrevious() const;
	void findPrevious();

	void highlightParagraph(int paragraphNumber);

protected:
	bool onStylusPress(int x, int y);
	bool onStylusMove(int x, int y);
	bool onStylusMovePressed(int x, int y);
	bool onStylusRelease(int x, int y);
	virtual bool onStylusClick(int x, int y, int count);
	virtual int doubleClickDelay() const = 0;

	void onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to);
	void onScrollbarStep(Direction direction, int steps);
	void onScrollbarPageStep(Direction direction, int steps);
	void activateSelection(int x, int y);

	virtual void paint();

	virtual void preparePaintInfo();

	virtual shared_ptr<PositionIndicator> createPositionIndicator(const ZLTextPositionIndicatorInfo&);

	virtual int leftMargin() const = 0;
	virtual int rightMargin() const = 0;
	virtual int topMargin() const = 0;
	virtual int bottomMargin() const = 0;

private:
	void clear();

	std::vector<size_t>::const_iterator nextBreakIterator() const;

	shared_ptr<ZLTextView::PositionIndicator> positionIndicator();

	void gotoCharIndex(size_t charIndex);

	void startSelectionScrolling(bool forward);
	void stopSelectionScrolling();

private:
	ZLTextAreaController myTextAreaController;

	std::vector<size_t> myTextSize;
	std::vector<size_t> myTextBreaks;

	shared_ptr<ZLRunnable> mySelectionScroller;

	shared_ptr<PositionIndicator> myPositionIndicator;

	bool myTreeStateIsFrozen;
	bool myDoUpdateScrollbar;

	struct DoubleClickInfo {
		DoubleClickInfo(const ZLTextView &view);
		void update(int x, int y, bool press);

		int Count;
		ZLTime Time;
		int X;
		int Y;

	private:
		const ZLTextView &myView;
	} myDoubleClickInfo;
};

inline const ZLTextArea &ZLTextView::textArea() const { return myTextAreaController.area(); }

#endif /* __ZLTEXTVIEW_H__ */
