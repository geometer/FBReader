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

#include <abstract/ZLUnicodeUtil.h>

#include "Word.h"

Word::Word(const char *data, unsigned short size, size_t paragraphOffset) : myData(data), mySize(size), myMark(0), myParagraphOffset(paragraphOffset), myWidth(-1) {
	myLength = ZLUnicodeUtil::utf8Length(myData, size);
}

void Word::addMark(int start, int len) {
	WordMark *existingMark = myMark;
	WordMark *mark = new WordMark(start, len);

	if ((existingMark == 0) || (existingMark->start() > start)) {
		mark->setNext(existingMark);
		myMark = mark;
	} else {
		while ((existingMark->next() != 0) && (existingMark->next()->start() < start)) {
			existingMark = existingMark->myNext;
		}
		mark->setNext(existingMark->myNext);
		existingMark->setNext(mark);
	}
}
