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

#ifndef __ZLVIEWWIDGET_H__
#define __ZLVIEWWIDGET_H__

#include <string>

#include "ZLView.h"

class ZLView;

class ZLViewWidget {

protected:
	ZLViewWidget(ZLView::Angle initialAngle);

public:
	virtual ~ZLViewWidget();
	void setView(shared_ptr<ZLView> view);
	shared_ptr<ZLView> view() const;

	virtual void trackStylus(bool track) = 0;

	void rotate(ZLView::Angle rotation);
	ZLView::Angle rotation() const;

	virtual void setScrollbarEnabled(ZLView::Direction direction, bool enabled) = 0;
	virtual void setScrollbarPlacement(ZLView::Direction direction, bool standard) = 0;
	virtual void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) = 0;
	void onScrollbarMoved(ZLView::Direction direction, size_t full, size_t from, size_t to);
	void onScrollbarStep(ZLView::Direction direction, int steps);
	void onScrollbarPageStep(ZLView::Direction direction, int steps);

private:
	void correctDirection(ZLView::Direction &direction, bool &invert);

protected:
	virtual void repaint() = 0;

private:
	shared_ptr<ZLView> myView;
	ZLView::Angle myRotation;

friend class ZLApplication;
};

inline ZLViewWidget::ZLViewWidget(ZLView::Angle initialAngle) : myView(0), myRotation(initialAngle) {}
inline ZLViewWidget::~ZLViewWidget() {}
inline shared_ptr<ZLView> ZLViewWidget::view() const { return myView; }

#endif /* __ZLVIEWWIDGET_H__ */
