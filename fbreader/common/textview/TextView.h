/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
	static ZLColorOption TreeLinesColorOption;
	static ZLBooleanOption AutoHyphenationOption;

	static ZLBooleanOption ShowPositionIndicatorOption;
	static ZLBooleanOption IsIndicatorSensitiveOption;
	static ZLColorOption PositionIndicatorColorOption;
	static ZLIntegerOption PositionIndicatorHeightOption;
	static ZLIntegerOption PositionIndicatorOffsetOption;

public:
	enum ScrollingMode {
		NO_OVERLAPPING,
		KEEP_LINES,
		SCROLL_LINES,
		SCROLL_PERCENTAGE
	};

private:
	class ViewStyle {

	public:
		ViewStyle(ZLPaintContext &context) VIEW1_SECTION;
		~ViewStyle() VIEW1_SECTION;

		void reset() VIEW_SECTION;
		void setStyle(const TextStylePtr style) VIEW_SECTION;
		void applyControl(const ControlElement &control) VIEW_SECTION;
		void applyControls(const WordCursor &begin, const WordCursor &end) VIEW_SECTION;

		const ZLPaintContext &context() const VIEW_SECTION;
		const TextStylePtr style() const VIEW_SECTION;
		int elementWidth(const TextElement &element, unsigned int charNumber) const VIEW_SECTION;
		int elementHeight(const TextElement &element) const VIEW_SECTION;
		int textAreaHeight() const VIEW_SECTION;

		int wordWidth(const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) const VIEW_SECTION;

	private:
		TextStylePtr myStyle;
		ZLPaintContext &myContext;
		mutable int myWordHeight;
	};

	struct LineInfo {
		LineInfo(const WordCursor &word, TextStylePtr style) VIEW_SECTION;
		~LineInfo() VIEW_SECTION;

		bool operator < (const LineInfo &info) const VIEW_SECTION;

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
		ParagraphPosition(int paragraphNumber, int yStart, int yEnd) VIEW_SECTION;
		~ParagraphPosition() VIEW_SECTION;
	};

	struct TextElementPosition {
		int ParagraphNumber, TextElementNumber;
		TextElement::Kind Kind;
		int XStart, XEnd, YStart, YEnd;
		TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) VIEW_SECTION;
		~TextElementPosition() VIEW_SECTION;
	};

protected:
	TextView(ZLPaintContext &context) VIEW_SECTION;
	virtual ~TextView() VIEW_SECTION;

public:
	void clearCaches() VIEW_SECTION;

	void scrollPage(bool forward, ScrollingMode mode, unsigned int value) VIEW_SECTION;
	void scrollToStartOfText() VIEW1_SECTION;
	void scrollToEndOfText() VIEW1_SECTION;

	void gotoMark(TextMark mark) VIEW1_SECTION;
	virtual void gotoParagraph(int num, bool last = false) VIEW1_SECTION;

	const WordCursor &startCursor() const VIEW_SECTION;
	const WordCursor &endCursor() const VIEW_SECTION;

	virtual void setModel(const TextModel *model, const std::string &name) VIEW1_SECTION;

	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) VIEW1_SECTION;
	bool canFindNext() const VIEW1_SECTION;
	void findNext() VIEW1_SECTION;
	bool canFindPrevious() const VIEW1_SECTION;
	void findPrevious() VIEW1_SECTION;

	bool onStylusPress(int x, int y) VIEW1_SECTION;

protected:
	virtual void paint() VIEW_SECTION;

	const std::string &fileName() const VIEW1_SECTION;
	const TextModel *model() const VIEW1_SECTION;

	const ParagraphPosition *paragraphByCoordinate(int y) const VIEW1_SECTION;
	const TextElementPosition *elementByCoordinates(int x, int y) const VIEW1_SECTION;

	void rebuildPaintInfo(bool strong) VIEW_SECTION;

	void setStartCursor(ParagraphCursor *cursor) VIEW1_SECTION;
	void moveStartCursor(int paragraphNumber, int wordNumber, int charNumber) VIEW1_SECTION;
	void moveStartCursor(int paragraphNumber, bool start) VIEW1_SECTION;
	void moveEndCursor(int paragraphNumber, int wordNumber, int charNumber) VIEW1_SECTION;
	void moveEndCursor(int paragraphNumber, bool start) VIEW1_SECTION;

	bool empty() const VIEW_SECTION;

	void selectParagraph(int paragraphNumber) VIEW1_SECTION;

private:
	void clear() VIEW_SECTION;

	LineInfo processTextLine(const WordCursor &start, const WordCursor &end) VIEW_SECTION;
	void drawTextLine(const LineInfo &info) VIEW_SECTION;
	void drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) VIEW_SECTION;
	void drawString(int x, int y, const char *str, int len, const Word::WordMark *mark, int shift) VIEW_SECTION;
	void drawTreeNode(TreeElement::TreeElementKind kind, int height) VIEW1_SECTION;

	void preparePaintInfo() VIEW_SECTION;

	bool pageIsEmpty() const VIEW_SECTION;
	WordCursor findLineFromStart(unsigned int overlappingValue) const VIEW_SECTION;
	WordCursor findLineFromEnd(unsigned int overlappingValue) const VIEW_SECTION;
	WordCursor findPercentFromStart(unsigned int percent) const VIEW_SECTION;

	enum SizeUnit {
		PIXEL_UNIT,
		LINE_UNIT
	};
	int infoSize(const LineInfo &info, SizeUnit unit) VIEW_SECTION;
	int paragraphSize(const WordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit) VIEW_SECTION;
	void skip(WordCursor &paragraph, SizeUnit unit, int size) VIEW_SECTION;
	WordCursor findStart(const WordCursor &end, SizeUnit unit, int textHeight) VIEW_SECTION;

	WordCursor buildInfos(const WordCursor &start) VIEW_SECTION;

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
#ifndef PALM_TEMPORARY
	std::set<LineInfo> myLineInfoCache;
#endif // PALM_TEMPORARY

	ScrollingMode myScrollingMode;
	unsigned int myOverlappingValue;

	int myOldWidth, myOldHeight;

	std::vector<ParagraphPosition> myParagraphMap;
	std::vector<TextElementPosition> myTextElementMap;

	std::vector<size_t> myTextSize;
	size_t myFullTextSize;

	ViewStyle myStyle;
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
