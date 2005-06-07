/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>

#include "../view/View.h"
#include "TextElement.h"
#include "Word.h"

class TextModel;
class Paragraph;
class ParagraphCursor;
class WordCursor;
class TextStyle;
class TextMark;

class TextView : public View {

private:
	class ViewStyle {

	public:
		ViewStyle(PaintContext &context) VIEW_SECTION;
		~ViewStyle() VIEW_SECTION;

		void reset() VIEW_SECTION;
		void applyControl(const ControlElement &control, bool revert) VIEW_SECTION;
		void applyControls(const WordCursor &begin, const WordCursor &end, bool revert) VIEW_SECTION;

		const PaintContext &context() const VIEW_SECTION;
		const TextStyle &style() const VIEW_SECTION;
		int elementWidth(const WordCursor &cursor) const VIEW_SECTION;
		int elementHeight(const WordCursor &cursor) const VIEW_SECTION;
		int textAreaHeight() const VIEW_SECTION;

		int wordWidth(const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) const VIEW_SECTION;

	private:
		const TextStyle *myStyle;
		PaintContext &myContext;
	};

	class LineProcessor {

	public:
		LineProcessor(ViewStyle &style) VIEW_SECTION;
		~LineProcessor() VIEW_SECTION;
		WordCursor process(const WordCursor &start, const WordCursor &end) VIEW_SECTION;

		int width() const VIEW_SECTION;
		int height() const VIEW_SECTION;
		int spaceCounter() const VIEW_SECTION;

	private:
		ViewStyle &myStyle;
		int myWidth;
		int myHeight;
		int mySpaceCounter;
	};

public:
	static ZLColorOption TreeLinesColorOption;
	static ZLBooleanOption AutoHyphenationOption;

	static ZLBooleanOption ShowPositionIndicatorOption;
	static ZLColorOption PositionIndicatorColorOption;
	static ZLIntegerOption PositionIndicatorHeightOption;
	static ZLIntegerOption PositionIndicatorOffsetOption;
	
public:
	void paint(bool doPaint) VIEW_SECTION;
	virtual void paint() VIEW_SECTION;

	void scrollPageBackward() VIEW_SECTION;
	void scrollPageForward() VIEW_SECTION;
	void gotoMark(TextMark mark) VIEW_SECTION;
	virtual void gotoParagraph(int num, bool last = false) VIEW_SECTION;

	virtual void setModel(const TextModel *model, const std::string &name) VIEW_SECTION;

	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) VIEW_SECTION;
	bool canFindNext() const VIEW_SECTION;
	void findNext() VIEW_SECTION;
	bool canFindPrevious() const VIEW_SECTION;
	void findPrevious() VIEW_SECTION;

	bool onStylusPress(int x, int y) VIEW_SECTION;

protected:
	TextView(PaintContext &context) VIEW_SECTION;
	virtual ~TextView() VIEW_SECTION;

private:
	void clear() VIEW_SECTION;

	int paragraphHeight(const ParagraphCursor &paragraph, bool beforeCurrentPosition) VIEW_SECTION;
	void skip(ParagraphCursor &paragraph, int height) VIEW_SECTION;
	void drawParagraph(ParagraphCursor &paragraph, bool doPaint) VIEW_SECTION;
	void drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) VIEW_SECTION;
	void drawString(int x, int y, const char *str, int len, const Word::WordMark *mark, int shift) VIEW_SECTION;
	void drawTreeNode(TreeElement::TreeElementKind kind) VIEW_SECTION;

protected:
	const TextModel *myModel;
	std::string myName;

	ParagraphCursor* myFirstParagraphCursor;
	ParagraphCursor* myLastParagraphCursor;

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

	const ParagraphPosition *paragraphByCoordinate(int y) const VIEW_SECTION;
	const TextElementPosition *elementByCoordinates(int x, int y) const VIEW_SECTION;

private:
	std::vector<ParagraphPosition> myParagraphMap;
	std::vector<TextElementPosition> myTextElementMap;

	ViewStyle myStyle;
	LineProcessor myLineProcessor;
};

inline TextView::ViewStyle::~ViewStyle() {}
inline const PaintContext &TextView::ViewStyle::context() const { return myContext; }
inline const TextStyle &TextView::ViewStyle::style() const { return *myStyle; }

inline TextView::LineProcessor::LineProcessor(ViewStyle &style) : myStyle(style) {}
inline TextView::LineProcessor::~LineProcessor() {}
inline int TextView::LineProcessor::width() const { return myWidth; }
inline int TextView::LineProcessor::height() const { return myHeight; }
inline int TextView::LineProcessor::spaceCounter() const { return mySpaceCounter; }

inline void TextView::paint() { paint(true); }

inline TextView::ParagraphPosition::ParagraphPosition(int paragraphNumber, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), YStart(yStart), YEnd(yEnd) {}
inline TextView::ParagraphPosition::~ParagraphPosition() {}

inline TextView::TextElementPosition::TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), TextElementNumber(textElementNumber), Kind(kind), XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}
inline TextView::TextElementPosition::~TextElementPosition() {}

#endif /* __TEXTVIEW_H__ */
