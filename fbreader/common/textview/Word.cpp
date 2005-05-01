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

Word::Word(const char *utf8String, int len, int startOffset) {
	myStartOffset = startOffset;
	myMark = 0;
	myUtf8Contents.append(utf8String, len);
	myUtf8Length = ZLUnicodeUtil::utf8Length(myUtf8Contents);
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
