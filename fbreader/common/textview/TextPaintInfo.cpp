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

#include "TextView.h"

TextPaintInfo::TextPaintInfo() : myPaintState(NOTHING_TO_PAINT) {
}

TextPaintInfo::~TextPaintInfo() {
}

void TextPaintInfo::clear() {
	myStartCursor = 0;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = NOTHING_TO_PAINT;
}

void TextPaintInfo::rebuild(bool strong) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	myLineInfos.clear();
	if (!myStartCursor.isNull()) {
		if (strong) {
			myStartCursor.rebuild();
		}
		myEndCursor = 0;
		myPaintState = START_IS_KNOWN;
	} else {
		if (strong) {
			myEndCursor.rebuild();
		}
		myStartCursor = 0;
		myPaintState = END_IS_KNOWN;
	}
}

void TextPaintInfo::setStartCursor(ParagraphCursor *cursor) {
	myStartCursor = cursor;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = myStartCursor.isNull() ? NOTHING_TO_PAINT : START_IS_KNOWN;
}

void TextPaintInfo::moveStartCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myStartCursor.isNull()) {
		myStartCursor = myEndCursor;
	}
	myStartCursor.moveToParagraph(paragraphNumber);
	myStartCursor.moveTo(wordNumber, charNumber);
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void TextPaintInfo::moveStartCursor(int paragraphNumber, bool start) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myStartCursor.isNull()) {
		myStartCursor = myEndCursor;
	}
	myStartCursor.moveToParagraph(paragraphNumber);
	if (start) {
		myStartCursor.moveToParagraphStart();
	} else {
		myStartCursor.moveToParagraphEnd();
	}
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void TextPaintInfo::moveEndCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myEndCursor.isNull()) {
		myEndCursor = myStartCursor;
	}
	myEndCursor.moveToParagraph(paragraphNumber);
	myEndCursor.moveTo(wordNumber, charNumber);
	myStartCursor = 0;
	myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

void TextPaintInfo::moveEndCursor(int paragraphNumber, bool start) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myEndCursor.isNull()) {
		myEndCursor = myStartCursor;
	}
	myEndCursor.moveToParagraph(paragraphNumber);
	if (start) {
		myEndCursor.moveToParagraphStart();
	} else {
		myEndCursor.moveToParagraphEnd();
	}
	myStartCursor = 0;
	myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

void TextPaintInfo::scrollPageBackward() {
	if (!myStartCursor.isNull()) {
		if (!myStartCursor.paragraphCursor().isFirst() || !myStartCursor.isStartOfParagraph()) {
			myEndCursor = myStartCursor;
			myStartCursor = 0;
			myPaintState = END_IS_KNOWN;
		}
	}
}

void TextPaintInfo::scrollPageForward() {
	if (!myEndCursor.isNull()) {
		if (!myEndCursor.paragraphCursor().isLast() || !myEndCursor.isEndOfParagraph()) {
			myStartCursor = myEndCursor;
			myEndCursor = 0;
			myPaintState = START_IS_KNOWN;
		}
	}
}

void TextPaintInfo::prepare(TextView &textView) {
	textView.context().setLeftMargin(TextStyle::LeftMarginOption.value());
	textView.context().setRightMargin(TextStyle::RightMarginOption.value());
	textView.context().setTopMargin(TextStyle::TopMarginOption.value());
	textView.context().setBottomMargin(TextStyle::BottomMarginOption.value());

	switch (myPaintState) {
		case NOTHING_TO_PAINT:
			return;
		case READY:
			return;
		default:
		{
			myLineInfos.clear();
			if (myStartCursor.isNull()) {
				if (myEndCursor.isNull()) {
					return;
				}
				myStartCursor = myEndCursor;
				myEndCursor = 0;
				int height = textView.paragraphHeight(myStartCursor, true);
				bool positionChanged = !myStartCursor.isStartOfParagraph();
				myStartCursor.moveToParagraphStart();
				const int textAreaHeight = textView.myStyle.textAreaHeight();
				while (height < textAreaHeight) {
					if (positionChanged && myStartCursor.paragraphCursor().isEndOfSection()) {
						break;
					}
					if (!myStartCursor.previousParagraph()) {
						break;
					}
					if (!myStartCursor.paragraphCursor().isEndOfSection()) {
						positionChanged = true;
					}
					height += textView.paragraphHeight(myStartCursor, false);
				}
				textView.skip(myStartCursor, height - textAreaHeight);
			}

			myEndCursor = myStartCursor;

			int textAreaHeight = textView.myStyle.textAreaHeight();
			do {
				LineInfo info(myEndCursor, textView.myStyle.style());
				WordCursor paragraphEnd = myEndCursor;
				paragraphEnd.moveToParagraphEnd();
				WordCursor start = myEndCursor;
				start.moveToParagraphStart();

				textView.myStyle.reset();
				textView.myStyle.applyControls(start, info.Start);

				while (!info.End.isEndOfParagraph()) {
					info = textView.processTextLine(info.End, paragraphEnd);
					textAreaHeight -= info.Height;
					if (textAreaHeight < 0) {
						break;
					}
					myEndCursor = info.End;
					myLineInfos.push_back(info);
				}
			} while (myEndCursor.isEndOfParagraph() && myEndCursor.nextParagraph() && !myEndCursor.paragraphCursor().isEndOfSection());
			myPaintState = READY;
		}
	}
}
