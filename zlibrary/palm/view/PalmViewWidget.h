/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __PALMVIEWWIDGET_H__
#define __PALMVIEWWIDGET_H__

#include <abstract/ZLView.h>

class PalmViewWidget : public ZLViewWidget {

public:
	PalmViewWidget(UInt16 formId) ZLVIEW_SECTION;
	~PalmViewWidget() ZLVIEW_SECTION;

protected:
	void repaintView() ZLVIEW_SECTION;

public:
	void paintEvent() ZLVIEW_SECTION;
	void trackStylus(bool track) ZLVIEW_SECTION;

private:
	UInt16 myFormId;
	WinHandle myScreenWindow;
	WinHandle myBufferWindow;
};

#endif /* __PALMVIEWWIDGET_H__ */
