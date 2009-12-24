/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLMirroredPaintContext.h>

#include <ZLTextModel.h>

#include "ZLTextArea.h"
#include "ZLTextLineInfo.h"

ZLTextArea::ZLTextArea(ZLPaintContext &context) : myContext(context), myWidth(0), myHeight(0), myIsRtl(false) {
}

ZLTextArea::~ZLTextArea() {
}

void ZLTextArea::setModel(shared_ptr<ZLTextModel> model) {
	clear();

	myModel = model;
	if (!model.isNull()) {
		myIsRtl = model->isRtl();
		if (myIsRtl) {
			myMirroredContext = new ZLMirroredPaintContext(myContext);
		} else {
			myMirroredContext.reset();
		}
		myStartCursor = ZLTextParagraphCursor::cursor(*model);
		myEndCursor = 0;
	}
}

void ZLTextArea::clear() {
	myStartCursor = 0;
	myEndCursor = 0;

	myLineInfos.clear();
}
