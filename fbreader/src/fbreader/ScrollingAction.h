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

#ifndef __SCROLLINGACTION_H__
#define __SCROLLINGACTION_H__

#include <ZLApplication.h>
#include <ZLTextAreaController.h>
#include <ZLBlockTreeView.h>

class ScrollingAction : public ZLApplication::Action {

protected:
	ScrollingAction(
		ZLTextAreaController::ScrollingMode textScrollingMode,
		ZLBlockTreeView::ScrollingMode blockScrollingMode,
		bool forward
	);

public:
	virtual size_t textOptionValue() const = 0;
	virtual int scrollingDelay() const;
	bool isEnabled() const;
	bool useKeyDelay() const;
	void run();

private:
	const ZLTextAreaController::ScrollingMode myTextScrollingMode;
	const ZLBlockTreeView::ScrollingMode myBlockScrollingMode;
	const bool myForward;
};

class LineScrollingAction : public ScrollingAction {

public:
	LineScrollingAction(bool forward);

private:
	int scrollingDelay() const;
	size_t textOptionValue() const;
};

class PageScrollingAction : public ScrollingAction {

public:
	PageScrollingAction(bool forward);

private:
	int scrollingDelay() const;
	size_t textOptionValue() const;
};

class MouseWheelScrollingAction : public ScrollingAction {

public:
	MouseWheelScrollingAction(bool forward);

private:
	size_t textOptionValue() const;
};

class TapScrollingAction : public ScrollingAction {

public:
	TapScrollingAction(bool forward);

private:
	size_t textOptionValue() const;
	bool isEnabled() const;
};

#endif /* __SCROLLINGACTION_H__ */
