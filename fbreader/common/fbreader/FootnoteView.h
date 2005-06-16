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

#ifndef __FOOTNOTEVIEW_H__
#define __FOOTNOTEVIEW_H__

#include "../textview/TextView.h"

class FootnoteView : public TextView {

public:
	FootnoteView(ZLPaintContext &context) FB_VIEW_SECTION;
	~FootnoteView() FB_VIEW_SECTION;
	const std::string &caption() const FB_VIEW_SECTION;
	void setCaption(const std::string &caption) FB_VIEW_SECTION;

private:
	std::string myCaption;
};

inline FootnoteView::FootnoteView(ZLPaintContext &context) : TextView(context) {}
inline FootnoteView::~FootnoteView() {}
inline const std::string &FootnoteView::caption() const { return myCaption; }
inline void FootnoteView::setCaption(const std::string &caption) { myCaption = caption; }

#endif /* __FOOTNOTEVIEW_H__ */
