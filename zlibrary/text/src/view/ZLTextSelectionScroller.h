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

#ifndef __ZLTEXTSELECTIONSCROLLER_H__
#define __ZLTEXTSELECTIONSCROLLER_H__

#include <ZLRunnable.h>

class ZLTextView;

class ZLTextSelectionScroller : public ZLRunnable {

public:
	enum Direction {
		SCROLL_FORWARD,
		SCROLL_BACKWARD,
		DONT_SCROLL
	};

public:
	ZLTextSelectionScroller(ZLTextView &view);
	void setDirection(Direction direction);
	Direction direction() const;

private:
	void run();

private:
	ZLTextView &myView;
	Direction myDirection;
};

inline ZLTextSelectionScroller::Direction ZLTextSelectionScroller::direction() const {
	return myDirection;
}

#endif /* __ZLTEXTSELECTIONSCROLLER_H__ */
