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

#ifndef __CONTENTSVIEW_H__
#define __CONTENTSVIEW_H__

#include "../textview/TextView.h"
#include "../textview/ParagraphCursor.h"

class FBReader;

class ContentsView : public TextView {

public:
	ContentsView(FBReader &reader, PaintContext &context) FB_VIEW_SECTION;
	~ContentsView() FB_VIEW_SECTION;
	const std::string &caption() const FB_VIEW_SECTION;
	void setCaption(const std::string &caption) FB_VIEW_SECTION;

	void setModel(const TextModel *model, const std::string &name) FB_VIEW_SECTION;
	void saveState() FB_VIEW_SECTION;

	bool isEmpty() const FB_VIEW_SECTION;

	bool onStylusPress(int x, int y) FB_VIEW_SECTION;

private:
	FBReader &myReader;
	std::string myCaption;
};

inline ContentsView::~ContentsView() {}
inline const std::string &ContentsView::caption() const { return myCaption; }
inline void ContentsView::setCaption(const std::string &caption) { myCaption = caption; }

#endif /* __CONTENTSVIEW_H__ */
