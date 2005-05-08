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

#ifndef __BOOKTEXTVIEW_H__
#define __BOOKTEXTVIEW_H__

#include "../textview/TextView.h"

class FBReader;

class BookTextView : public TextView {

public:
	BookTextView(FBReader &reader, PaintContext &context) FB_VIEW_SECTION;
	~BookTextView() FB_VIEW_SECTION;
	const std::string &caption() const FB_VIEW_SECTION;
	void setCaption(const std::string &caption) FB_VIEW_SECTION;

	void setModel(const TextModel *model, const std::string &name) FB_VIEW_SECTION;
	void saveState() FB_VIEW_SECTION;

	void gotoParagraph(int num, bool last = false) FB_VIEW_SECTION;
	bool canUndoPageMove() FB_VIEW_SECTION;
	void undoPageMove() FB_VIEW_SECTION;
	bool canRedoPageMove() FB_VIEW_SECTION;
	void redoPageMove() FB_VIEW_SECTION;

	bool onStylusPress(int x, int y) FB_VIEW_SECTION;

private:
	void pushCurrentPositionIntoStack() FB_VIEW_SECTION;
	void replaceCurrentPositionInStack() FB_VIEW_SECTION;
	bool setFirstParagraphCursor() FB_VIEW_SECTION;

private:
	std::vector<std::pair<int,int> > myPositionStack;
	unsigned int myCurrentPointInStack;
	unsigned int myMaxStackSize;

	FBReader &myReader;
	std::string myCaption;
};

inline const std::string &BookTextView::caption() const { return myCaption; }
inline void BookTextView::setCaption(const std::string &caption) { myCaption = caption; }

#endif /* __BOOKTEXTVIEW_H__ */
