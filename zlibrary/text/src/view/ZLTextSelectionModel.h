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

#ifndef __ZLTEXTSELECTIONMODEL_H__
#define __ZLTEXTSELECTIONMODEL_H__

#include <map>
#include <set>
#include <string>

#include <shared_ptr.h>
#include <ZLRunnable.h>

#include "ZLTextParagraphCursor.h"

class ZLTextView;
class ZLApplication;

class ZLTextSelectionModel {

public:
	ZLTextSelectionModel(ZLTextView &view, ZLApplication &application);

	void activate(int x, int y);
	bool extendTo(int x, int y);
	void update();
	void deactivate();
	void clear();

	const std::string &getText() const;
	bool isEmpty() const;

public:
	struct BoundElement {
		bool Exists;
		int ParagraphNumber;
		int TextElementNumber;
		size_t CharNumber;

		bool operator == (const BoundElement &element) const;
		bool operator != (const BoundElement &element) const;
	};

private:
	struct Bound {
		BoundElement Before;
		BoundElement After;

		bool operator < (const Bound &bound) const;
	};

public:
	std::pair<BoundElement,BoundElement> range() const;

private:
	void setBound(Bound &bound, int x, int y);
	void startSelectionScrolling(bool forward);
	void stopSelectionScrolling();

	void scrollAndExtend();

private:
	ZLTextView &myView;
	ZLApplication &myApplication;
	bool myIsActive;
	bool myIsEmpty;

	Bound myFirstBound;
	Bound mySecondBound;

	shared_ptr<ZLRunnable> mySelectionScroller;
	bool myDoUpdate;
	int myStoredX;
	int myStoredY;

	mutable std::set<ZLTextParagraphCursorPtr> myCursors;
	mutable std::string myText;
	mutable bool myTextIsUpToDate;

friend class ZLTextSelectionScroller;
};

class ZLTextSelectionScroller : public ZLRunnable {

public:
	enum Direction {
		SCROLL_FORWARD,
		SCROLL_BACKWARD,
		DONT_SCROLL
	};

public:
	ZLTextSelectionScroller(ZLTextSelectionModel &selectionModel);
	void setDirection(Direction direction);
	Direction direction() const;

private:
	void run();

private:
	ZLTextSelectionModel &mySelectionModel;
	Direction myDirection;
};

inline ZLTextSelectionScroller::Direction ZLTextSelectionScroller::direction() const {
	return myDirection;
}

#endif /* __ZLTEXTSELECTIONMODEL_H__ */
