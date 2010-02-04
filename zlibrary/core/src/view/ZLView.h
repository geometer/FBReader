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

#ifndef __ZLVIEW_H__
#define __ZLVIEW_H__

#include <string>

#include <shared_ptr.h>

#include <ZLColor.h>
#include <ZLTypeId.h>

class ZLViewWidget;
class ZLPaintContext;

class ZLView : public ZLObjectWithRTTI {

public:
	enum Angle {
		DEGREES0 = 0,
		DEGREES90 = 90,
		DEGREES180 = 180,
		DEGREES270 = 270
	};

	enum Direction {
		VERTICAL,
		HORIZONTAL
	};

protected:
	static const ZLTypeId TYPE_ID;

public:
	ZLView(ZLPaintContext &context);
	virtual ~ZLView();

	virtual const std::string &caption() const = 0;
	virtual void paint() = 0;
	ZLPaintContext &context() const;

	/*
	 * returns true iff stylus/finger event was processed
	 */
	virtual bool onStylusPress(int x, int y);
	virtual bool onStylusRelease(int x, int y);
	virtual bool onStylusMove(int x, int y);
	virtual bool onStylusMovePressed(int x, int y);
	virtual bool onFingerTap(int x, int y);

protected:
	const ZLTypeId &typeId() const;

	virtual ZLColor backgroundColor() const = 0;

	void setScrollbarEnabled(Direction direction, bool enabled);
	void setScrollbarParameters(Direction direction, size_t full, size_t from, size_t to);
	virtual void onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to);
	virtual void onScrollbarStep(Direction direction, int steps);
	virtual void onScrollbarPageStep(Direction direction, int steps);

private:
	struct ScrollBarInfo {
		ScrollBarInfo();

		bool Enabled;
		bool StandardLocation;
		size_t Full;
		size_t From;
		size_t To;
	};

	void updateScrollbarState();
	void updateScrollbarPlacement();
	void updateScrollbarParameters();
	void updateScrollbarParameters(Direction direction, const ScrollBarInfo &info, bool invert);

private:
	ZLViewWidget *myViewWidget;
	ZLPaintContext &myContext;
	ScrollBarInfo myVerticalScrollbarInfo;
	ScrollBarInfo myHorizontalScrollbarInfo;

private:
	ZLView(const ZLView&);
	const ZLView &operator=(const ZLView&);

friend class ZLViewWidget;
};

inline ZLPaintContext &ZLView::context() const { return myContext; }

#endif /* __ZLVIEW_H__ */
