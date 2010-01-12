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

#ifndef __ZLTEXTAREASTYLE_H__
#define __ZLTEXTAREASTYLE_H__

#include <ZLTextParagraphCursor.h>
#include <ZLTextArea.h>

class ZLTextStyle;

class ZLTextArea::Style {

public:
	Style(const ZLTextArea &area, shared_ptr<ZLTextStyle> style);

	void setTextStyle(shared_ptr<ZLTextStyle> style, unsigned char bidiLevel);

private:
	void applyControl(const ZLTextControlElement &control);
	void applyControl(const ZLTextStyleElement &control);
	void increaseBidiLevel();
	void decreaseBidiLevel();

public:
	void applySingleControl(const ZLTextElement &element);
	void applyControls(const ZLTextWordCursor &begin, const ZLTextWordCursor &end);

	shared_ptr<ZLTextStyle> textStyle() const;
	int elementWidth(const ZLTextElement &element, unsigned int charNumber, const ZLTextStyleEntry::Metrics &metrics) const;
	int elementHeight(const ZLTextElement &element, const ZLTextStyleEntry::Metrics &metrics) const;
	int elementDescent(const ZLTextElement &element) const;

	int wordWidth(const ZLTextWord &word, int start = 0, int length = -1, bool addHyphenationSign = false) const;

	unsigned char bidiLevel() const;

private:
	const ZLTextArea &myArea;
	mutable shared_ptr<ZLTextStyle> myTextStyle;
	mutable unsigned char myBidiLevel;
	mutable int myWordHeight;
};

inline unsigned char ZLTextArea::Style::bidiLevel() const { return myBidiLevel; }
inline shared_ptr<ZLTextStyle> ZLTextArea::Style::textStyle() const { return myTextStyle; }

#endif /* __ZLTEXTAREASTYLE_H__ */
