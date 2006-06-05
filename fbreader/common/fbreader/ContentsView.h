/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __CONTENTSVIEW_H__
#define __CONTENTSVIEW_H__

#include "../textview/TextView.h"
#include "../textview/ParagraphCursor.h"

class FBReader;

class ContentsView : public TextView {

public:
	ContentsView(FBReader &reader, ZLPaintContext &context);
	~ContentsView();
	const std::string &caption() const;
	void setCaption(const std::string &caption);

	void setModel(const TextModel *model, const std::string &name);
	void saveState();

	bool isEmpty() const;

	bool onStylusPress(int x, int y);

	void gotoReference();

private:
	FBReader &myReader;
	std::string myCaption;
};

inline ContentsView::~ContentsView() {}
inline const std::string &ContentsView::caption() const { return myCaption; }
inline void ContentsView::setCaption(const std::string &caption) { myCaption = caption; }

#endif /* __CONTENTSVIEW_H__ */
