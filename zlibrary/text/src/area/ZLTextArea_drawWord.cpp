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

#include <algorithm>

#include <ZLUnicodeUtil.h>

#include "ZLTextArea.h"
#include "ZLTextAreaStyle.h"

void ZLTextArea::drawString(Style &style, int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift, bool rtl) {
	x += myHOffset;
	y += myVOffset;

	context().setColor(myProperties.color(style.textStyle()->colorStyle()));
	if (mark == 0) {
		context().drawString(x, y, str, len, rtl);
	} else {
		bool revert = rtl != isRtl();
		if (revert) {
			x += context().stringWidth(str, len, rtl);
		}
		int pos = 0;
		for (; (mark != 0) && (pos < len); mark = mark->next()) {
			int markStart = mark->start() - shift;
			int markLen = mark->length();

			if (markStart < pos) {
				markLen += markStart - pos;
				markStart = pos;
			}

			if (markLen <= 0) {
				continue;
			}

			if (markStart > pos) {
				int endPos = std::min(markStart, len);
				if (revert) {
					x -= context().stringWidth(str + pos, endPos - pos, rtl);
				}
				context().drawString(x, y, str + pos, endPos - pos, rtl);
				if (!revert) {
					x += context().stringWidth(str + pos, endPos - pos, rtl);
				}
			}
			if (markStart < len) {
				context().setColor(myProperties.color(ZLTextStyle::HIGHLIGHTED_TEXT));
				{
					int endPos = std::min(markStart + markLen, len);
					if (revert) {
						x -= context().stringWidth(str + markStart, endPos - markStart, rtl);
					}
					context().drawString(x, y, str + markStart, endPos - markStart, rtl);
					if (!revert) {
						x += context().stringWidth(str + markStart, endPos - markStart, rtl);
					}
				}
				context().setColor(myProperties.color(style.textStyle()->colorStyle()));
			}
			pos = markStart + markLen;
		}

		if (pos < len) {
			if (revert) {
				x -= context().stringWidth(str + pos, len - pos, rtl);
			}
			context().drawString(x, y, str + pos, len - pos, rtl);
		}
	}
}

void ZLTextArea::drawWord(Style &style, int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign) {
	if ((start == 0) && (length == -1)) {
		drawString(style, x, y, word.Data, word.Size, word.mark(), 0, word.BidiLevel % 2 == 1);
	} else {
		int startPos = ZLUnicodeUtil::length(word.Data, start);
		int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
		if (!addHyphenationSign) {
			drawString(style, x, y, word.Data + startPos, endPos - startPos, word.mark(), startPos, word.BidiLevel % 2 == 1);
		} else {
			std::string substr;
			substr.append(word.Data + startPos, endPos - startPos);
			substr += '-';
			drawString(style, x, y, substr.data(), substr.length(), word.mark(), startPos, word.BidiLevel % 2 == 1);
		}
	}
}
