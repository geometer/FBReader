/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLOPTIONVIEW_H__
#define __ZLOPTIONVIEW_H__

class ZLOptionEntry;

class ZLOptionView {

public:
	ZLOptionView(ZLOptionEntry *option);
	virtual ~ZLOptionView();
	virtual void setVisible(bool visible) = 0;
	virtual void setActive(bool active) = 0;
	// TODO: change to pure virtual
	virtual void reset() {}

	// TODO: change to non-virtual
	virtual void onAccept() const = 0;

protected:
	ZLOptionEntry *myOption;
};

#endif /* __ZLOPTIONVIEW_H__ */
