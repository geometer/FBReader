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

#ifndef __CONTENTSVIEW_H__
#define __CONTENTSVIEW_H__

#include "FBView.h"

class ContentsView : public FBView {

public:
	ContentsView(FBReader &reader, shared_ptr<ZLPaintContext> context);
	~ContentsView();

	bool isEmpty() const;
	size_t currentTextViewParagraph(bool includeStart = true) const;
	void gotoReference();

private:
	bool _onStylusPress(int x, int y);
};

#endif /* __CONTENTSVIEW_H__ */
