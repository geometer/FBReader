/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __BOOKTEXTVIEW_H__
#define __BOOKTEXTVIEW_H__

#include <deque>

#include "FBView.h"

class BookTextView : public FBView {

public:
	BookTextView(FBReader &reader, ZLPaintContext &context);
	~BookTextView();
	const std::string &caption() const;
	void setCaption(const std::string &caption);

	void setModel(shared_ptr<TextModel> model, const std::string &name);
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

	void preparePaintInfo();

private:
	typedef std::pair<int,int> Position;
	typedef std::deque<Position> PositionStack;
	PositionStack myPositionStack;
	unsigned int myCurrentPointInStack;
	unsigned int myMaxStackSize;

	std::string myCaption;
	bool myLockUndoStackChanges;
};

inline const std::string &BookTextView::caption() const { return myCaption; }
inline void BookTextView::setCaption(const std::string &caption) { myCaption = caption; }
inline void BookTextView::preparePaintInfo() {
	TextView::preparePaintInfo();
	saveState();
}

#endif /* __BOOKTEXTVIEW_H__ */
