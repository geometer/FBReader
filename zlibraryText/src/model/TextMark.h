/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __TEXTMARK_H__
#define __TEXTMARK_H__

class TextParagraphData;

struct TextMark {
	int ParagraphNumber;
	int Offset, Length;

	TextMark();
	TextMark(int paragraphNumber, int offset, int length);
	TextMark(const TextMark &mark);
	const TextMark &operator = (const TextMark &mark);
	~TextMark();

	bool operator < (const TextMark &mark) const;
	bool operator > (const TextMark &mark) const;
	bool operator <= (const TextMark &mark) const;
	bool operator >= (const TextMark &mark) const;
};

inline TextMark::TextMark() : ParagraphNumber(-1), Offset(-1), Length(-1) {}
inline TextMark::TextMark(int paragraphNumber, int offset, int length) : ParagraphNumber(paragraphNumber), Offset(offset), Length(length) {}
inline TextMark::TextMark(const TextMark &mark) : ParagraphNumber(mark.ParagraphNumber), Offset(mark.Offset), Length(mark.Length) {}
inline const TextMark &TextMark::operator = (const TextMark &mark) {
	ParagraphNumber = mark.ParagraphNumber;
	Offset = mark.Offset;
	Length = mark.Length;
	return *this;
}
inline TextMark::~TextMark() {}

inline bool TextMark::operator < (const TextMark &mark) const {
	return (ParagraphNumber < mark.ParagraphNumber) ||
				 ((ParagraphNumber == mark.ParagraphNumber) && (Offset < mark.Offset));
}
inline bool TextMark::operator > (const TextMark &mark) const {
	return mark < *this;
}
inline bool TextMark::operator <= (const TextMark &mark) const {
	return !(*this > mark);
}
inline bool TextMark::operator >= (const TextMark &mark) const {
	return !(*this < mark);
}

#endif /* __TEXTMARK_H__ */
