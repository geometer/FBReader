/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>

#include "ZLTextWord.h"

ZLTextWord::ZLTextWord(const char *data, unsigned short size, size_t paragraphOffset) : Data(data), Size(size), Length(ZLUnicodeUtil::utf8Length(Data, size)), ParagraphOffset(paragraphOffset), myMark(0), myWidth(-1) {
}

ZLTextWord::~ZLTextWord() {
	if (myMark != 0) {
		delete myMark;
	}
}

void ZLTextWord::addMark(int start, int len) {
	Mark *existingMark = myMark;
	Mark *mark = new Mark(start, len);

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

ZLTextWord::Mark::~Mark() {
	if (myNext != 0) {
		delete myNext;
	}
}
