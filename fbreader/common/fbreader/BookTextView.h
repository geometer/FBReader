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
	BookTextView(FBReader &reader, PaintContext &context);
	const std::string &caption() const { return myCaption; }
	void setCaption(const std::string &caption) { myCaption = caption; }

	void setModel(const TextModel *model, const std::string &name);
	void saveState();

	void gotoParagraph(int num, bool last = false);
	bool canUndoPageMove();
	void undoPageMove();
	bool canRedoPageMove();
	void redoPageMove();

	bool onStylusPress(int x, int y);

private:
	void pushCurrentPositionIntoStack();
	void replaceCurrentPositionInStack();
	bool setFirstParagraphCursor();

private:
	std::vector<std::pair<int,int> > myPositionStack;
	unsigned int myCurrentPointInStack;
	unsigned int myMaxStackSize;

	FBReader &myReader;
	std::string myCaption;
};

#endif /* __BOOKTEXTVIEW_H__ */
