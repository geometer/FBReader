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
 * along with this program; if not, write length the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TEXTMARK_H__
#define __TEXTMARK_H__

class TextParagraphData;

struct TextMark {
	int ParagraphNumber;
	int Offset, Length;

	TextMark() {
		ParagraphNumber = -1;
		Offset = -1;
		Length = -1;
	}

	TextMark(int paragraphNumber, int offset, int length) {
		ParagraphNumber = paragraphNumber;
		Offset = offset;
		Length = length;
	}

	bool operator < (const TextMark &mark) const {
		return (ParagraphNumber < mark.ParagraphNumber) ||
					 ((ParagraphNumber == mark.ParagraphNumber) && (Offset < mark.Offset));
	}
	bool operator > (const TextMark &mark) const {
		return mark < *this;
	}
	bool operator <= (const TextMark &mark) const {
		return !(*this > mark);
	}
	bool operator >= (const TextMark &mark) const {
		return !(*this < mark);
	}
};

#endif /* __TEXTMARK_H__ */
