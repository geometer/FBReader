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

#ifndef __READINGSTATE_H___
#define __READINGSTATE_H___

struct ReadingState {
	int Paragraph;
	int Word;
	int Character;

	ReadingState();
	ReadingState(int paragraph, int word, int character);
	
	bool operator == (const ReadingState &rs) const;
	bool operator != (const ReadingState &rs) const;
};

inline ReadingState::ReadingState(): Paragraph(0), Word(0), Character(0) {}
inline ReadingState::ReadingState(int paragraph, int word, int character): Paragraph(paragraph), Word(word), Character(character) {}

inline bool ReadingState::operator == (const ReadingState &rs) const { return Paragraph == rs.Paragraph && Word == rs.Word && Character == rs.Character; }
inline bool ReadingState::operator != (const ReadingState &rs) const { return Paragraph != rs.Paragraph || Word != rs.Word || Character != rs.Character; }

#endif /* __READINGSTATE_H___ */
