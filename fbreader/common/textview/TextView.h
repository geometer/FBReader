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
		ViewStyle(PaintContext &context);

		void reset();
		void applyControl(const ControlElement &control, bool revert);
		void applyControls(const WordCursor &begin, const WordCursor &end, bool revert);

		const PaintContext &context() const { return myContext; }
		const TextStyle &style() const { return *myStyle; }
		int elementWidth(const WordCursor &cursor) const;
		int elementHeight(const WordCursor &cursor) const;
		int textAreaHeight() const;

	private:
		const TextStyle *myStyle;
		PaintContext &myContext;
	};

	class LineProcessor {

	public:
		LineProcessor(ViewStyle &style) : myStyle(style) {}
		WordCursor process(const WordCursor &start, const WordCursor &end);

		int width() const { return myWidth; }
		int height() const { return myHeight; }
		int spaceCounter() const { return mySpaceCounter; }

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
	virtual void paint();

	void scrollPageBackward();
	void scrollPageForward();
	void gotoMark(TextMark mark);
	virtual void gotoParagraph(int num, bool last = false);

	virtual void setModel(const TextModel *model, const std::string &name);
	virtual void saveState();

	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward);
	bool canFindNext() const;
	void findNext();
	bool canFindPrevious() const;
	void findPrevious();

	bool onStylusPress(int x, int y);

protected:
	TextView(PaintContext &context);
	virtual ~TextView();

	virtual const std::string paragraphOptionName() const = 0;
	virtual const std::string wordOptionName() const = 0;
	virtual const std::string charOptionName() const = 0;

private:
	void clear();

	int paragraphHeight(const ParagraphCursor &paragraph, bool beforeCurrentPosition);
	void skip(ParagraphCursor &paragraph, int height);
	void drawParagraph(ParagraphCursor &paragraph);

	void drawTreeNode(TreeElement::TreeElementKind kind);

protected:
	const TextModel *myModel;
	std::string myName;

	ParagraphCursor* myFirstParagraphCursor;
	ParagraphCursor* myLastParagraphCursor;

protected:
	struct ParagraphPosition {
		int ParagraphNumber;
		int YStart, YEnd;
		ParagraphPosition(int paragraphNumber, int yStart, int yEnd) {
			ParagraphNumber = paragraphNumber;

			YStart = yStart;
			YEnd = yEnd;
		}
	};

	struct TextElementPosition {
		int ParagraphNumber, TextElementNumber;
		TextElement::Kind Kind;
		int XStart, XEnd, YStart, YEnd;
		TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) {
			ParagraphNumber = paragraphNumber;
			TextElementNumber = textElementNumber;
			Kind = kind;

			XStart = xStart;
			XEnd = xEnd;
			YStart = yStart;
			YEnd = yEnd;
		}
	};

	const ParagraphPosition *paragraphByCoordinate(int y) const;
	const TextElementPosition *elementByCoordinates(int x, int y) const;

private:
	std::vector<ParagraphPosition> myParagraphMap;
	std::vector<TextElementPosition> myTextElementMap;

	ViewStyle myStyle;
	LineProcessor myLineProcessor;
};

#endif /* __TEXTVIEW_H__ */
