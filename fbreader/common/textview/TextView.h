/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#include <vector>
#include <set>
#include <string>

#include <abstract/ZLOptions.h>
#include <abstract/ZLView.h>

#include "TextElement.h"
#include "Word.h"
#include "TextStyle.h"
#include "ParagraphCursor.h"

class TextModel;
class Paragraph;
class TextMark;

class TextView : public ZLView {

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
		PositionIndicator(TextView &textView);

		void draw();
		bool onStylusPress(int x, int y);

	private:
		TextView &myTextView;
	};

	friend class TextView::PositionIndicator;
	
private:
	class ViewStyle {

	public:
		ViewStyle(ZLPaintContext &context);
		~ViewStyle();

		void reset();
		void setStyle(const TextStylePtr style);
		void applyControl(const ControlElement &control);
		void applyControl(const ForcedControlElement &control);
		void applyControls(const WordCursor &begin, const WordCursor &end);

		const ZLPaintContext &context() const;
		const TextStylePtr style() const;
		int elementWidth(const TextElement &element, unsigned int charNumber) const;
		int elementHeight(const TextElement &element) const;
		int textAreaHeight() const;

		int wordWidth(const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) const;

	private:
		TextStylePtr myStyle;
		ZLPaintContext &myContext;
		mutable int myWordHeight;
	};

	struct LineInfo {
		LineInfo(const WordCursor &word, TextStylePtr style);
		~LineInfo();

		bool operator < (const LineInfo &info) const;

		WordCursor Start;
		WordCursor End;
		bool IsVisible;
		int Width;
		int Height;
		int SpaceCounter;
		TextStylePtr StartStyle;
	};

protected:
	struct ParagraphPosition {
		int ParagraphNumber;
		int YStart, YEnd;
		ParagraphPosition(int paragraphNumber, int yStart, int yEnd);
		~ParagraphPosition();

		struct RangeChecker {
			RangeChecker(int y) : myY(y) {}
			bool operator()(const ParagraphPosition &position) const {
				return (myY >= position.YStart) && (myY <= position.YEnd);
			}

			int myY;
		};
	};

	struct TextElementPosition {
		int ParagraphNumber, TextElementNumber;
		TextElement::Kind Kind;
		int XStart, XEnd, YStart, YEnd;
		TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd);
		~TextElementPosition();

		struct RangeChecker {
			RangeChecker(int x, int y) : myX(x), myY(y) {}
			bool operator()(const TextElementPosition &position) const {
				return
					(myX >= position.XStart) && (myX <= position.XEnd) &&
					(myY >= position.YStart) && (myY <= position.YEnd);
			}

			int myX, myY;
		};
	};

protected:
	TextView(ZLPaintContext &context);
	virtual ~TextView();

public:
	void clearCaches();

	void scrollPage(bool forward, ScrollingMode mode, unsigned int value);
	void scrollToHome();
	void scrollToStartOfText();
	void scrollToEndOfText();

	void gotoMark(TextMark mark);
	virtual void gotoParagraph(int num, bool last = false);

	const WordCursor &startCursor() const;
	const WordCursor &endCursor() const;

	virtual void setModel(const TextModel *model, const std::string &name);

	bool hasMultiSectionModel() const;
	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly);
	bool canFindNext() const;
	void findNext();
	bool canFindPrevious() const;
	void findPrevious();

	bool onStylusPress(int x, int y);

protected:
	virtual void paint();

	const std::string &fileName() const;
	const TextModel *model() const;

	const ParagraphPosition *paragraphByCoordinate(int y) const;
	const TextElementPosition *elementByCoordinates(int x, int y) const;

	void rebuildPaintInfo(bool strong);
	void preparePaintInfo();

	void setStartCursor(ParagraphCursor *cursor);
	void moveStartCursor(int paragraphNumber, int wordNumber, int charNumber);
	void moveStartCursor(int paragraphNumber);
	void moveEndCursor(int paragraphNumber, int wordNumber, int charNumber);
	void moveEndCursor(int paragraphNumber);

	bool empty() const;

	void selectParagraph(int paragraphNumber);

private:
	void clear();

	LineInfo processTextLine(const WordCursor &start, const WordCursor &end);
	void drawTextLine(const LineInfo &info);
	void drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign);
	void drawString(int x, int y, const char *str, int len, const Word::WordMark *mark, int shift);
	void drawTreeNode(TreeElement::TreeElementKind kind, int height);

	bool pageIsEmpty() const;
	WordCursor findLineFromStart(unsigned int overlappingValue) const;
	WordCursor findLineFromEnd(unsigned int overlappingValue) const;
	WordCursor findPercentFromStart(unsigned int percent) const;

	enum SizeUnit {
		PIXEL_UNIT,
		LINE_UNIT
	};
	int infoSize(const LineInfo &info, SizeUnit unit);
	int paragraphSize(const WordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit);
	void skip(WordCursor &paragraph, SizeUnit unit, int size);
	WordCursor findStart(const WordCursor &end, SizeUnit unit, int textHeight);

	WordCursor buildInfos(const WordCursor &start);

	std::vector<size_t>::const_iterator nextBreakIterator() const;

private:
	const TextModel *myModel;
	std::string myFileName;

	enum {
		NOTHING_TO_PAINT,
		READY,
		START_IS_KNOWN,
		END_IS_KNOWN,
		TO_SCROLL_FORWARD,
		TO_SCROLL_BACKWARD
	} myPaintState;
	WordCursor myStartCursor;
	WordCursor myEndCursor;
	std::vector<LineInfo> myLineInfos;
	std::set<LineInfo> myLineInfoCache;

	ScrollingMode myScrollingMode;
	unsigned int myOverlappingValue;

	int myOldWidth, myOldHeight;

	std::vector<ParagraphPosition> myParagraphMap;
	std::vector<TextElementPosition> myTextElementMap;

	std::vector<size_t> myTextSize;
	std::vector<size_t> myTextBreaks;

	ViewStyle myStyle;

	PositionIndicator myPositionIndicator;
};

inline TextView::ViewStyle::~ViewStyle() {}
inline const ZLPaintContext &TextView::ViewStyle::context() const { return myContext; }
inline const TextStylePtr TextView::ViewStyle::style() const { return myStyle; }

inline TextView::LineInfo::LineInfo(const WordCursor &word, TextStylePtr style) : Start(word), End(word), IsVisible(false), Width(0), Height(0), SpaceCounter(0), StartStyle(style) {}
inline TextView::LineInfo::~LineInfo() {}
inline bool TextView::LineInfo::operator < (const LineInfo &info) const { return Start < info.Start; }

inline TextView::ParagraphPosition::ParagraphPosition(int paragraphNumber, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), YStart(yStart), YEnd(yEnd) {}
inline TextView::ParagraphPosition::~ParagraphPosition() {}

inline TextView::TextElementPosition::TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), TextElementNumber(textElementNumber), Kind(kind), XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}
inline TextView::TextElementPosition::~TextElementPosition() {}

inline bool TextView::empty() const { return myPaintState == NOTHING_TO_PAINT; }
inline const WordCursor &TextView::startCursor() const { return myStartCursor; }
inline const WordCursor &TextView::endCursor() const { return myEndCursor; }
inline const std::string &TextView::fileName() const { return myFileName; }
inline const TextModel *TextView::model() const { return myModel; }
inline int TextView::infoSize(const LineInfo &info, SizeUnit unit) {
	return (unit == PIXEL_UNIT) ? info.Height : (info.IsVisible ? 1 : 0);
}

#endif /* __TEXTVIEW_H__ */
