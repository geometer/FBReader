/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLCOCOAVIEWWIDGET_H__
#define __ZLCOCOAVIEWWIDGET_H__

#include "../../../../core/src/view/ZLViewWidget.h"
#include <ZLApplication.h>

class ZLCocoaViewWidget : public ZLViewWidget {

public:
	ZLCocoaViewWidget(ZLApplication *application, ZLView::Angle initialAngle);
	~ZLCocoaViewWidget();

	int width() const;
	int height() const;
	void doPaint();

private:
	void trackStylus(bool track);
	void repaint();

	void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
	void setScrollbarPlacement(ZLView::Direction direction, bool standard);
	void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);
};

#endif /* __ZLCOCOAVIEWWIDGET_H__ */
