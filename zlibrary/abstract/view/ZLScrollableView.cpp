/*
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

#include "ZLScrollableView.h"

void ZLScrollableView::adjustViewSize(int x, int y) {
	int viewWidth = visibleAreaWidth(true);
	int viewWidth1 = visibleAreaWidth(false);
	int viewHeight = visibleAreaHeight(true);
	int viewHeight1 = visibleAreaHeight(false);

	int newWidth;
	int newHeight;
	if ((x <= viewWidth) && (y <= viewHeight)) {
		newWidth = viewWidth;
		newHeight = viewHeight;
	} else if (x <= viewWidth1) {
		newWidth = viewWidth1;
		newHeight = y;
	} else if (y <= viewHeight1) {
		newWidth = x;
		newHeight = viewHeight1;
	} else {
		newWidth = x;
		newHeight = y;
	}
	resizeView(newWidth, newHeight);
}
