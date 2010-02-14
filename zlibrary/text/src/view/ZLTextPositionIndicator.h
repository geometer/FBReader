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

#ifndef __ZLTEXTPOSITIONINDICATOR_H__
#define __ZLTEXTPOSITIONINDICATOR_H__

#include <ZLTextView.h>

class ZLTextView::PositionIndicator {

public:
	PositionIndicator(ZLTextView &textView, const ZLTextPositionIndicatorInfo &info);
	virtual ~PositionIndicator();

	virtual void draw();
	bool isResponsibleFor(int x, int y);
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
	size_t sizeOfTextBeforeCursor(const ZLTextWordCursor &cursor) const;

private:
	ZLTextView &myTextView;
	const ZLTextPositionIndicatorInfo &myInfo;
	int myExtraWidth;

friend class ZLTextView;
};

#endif /* __ZLTEXTPOSITIONINDICATOR_H__ */
