/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __W32WIDGET_H__
#define __W32WIDGET_H__

#include <vector>

#include <windows.h>

#include <shared_ptr.h>

class W32ControlCollection;

class W32Widget {

public:
	struct Size {
		short Width;
		short Height;

		Size() : Width(0), Height(0) {}
		Size(short w, short h) : Width(w), Height(h) {}

		bool operator == (const Size &s) const { return (Width == s.Width) && (Height == s.Height); }
		bool operator != (const Size &s) const { return !operator == (s); }
	};

protected:
	W32Widget();

public:
	virtual ~W32Widget();

	virtual void allocate(WORD *&p, short &id) const = 0;
	virtual int allocationSize() const = 0;
	virtual void setVisible(bool visible) = 0;
	virtual bool isVisible() const = 0;
	virtual int controlNumber() const = 0;
	virtual Size minimumSize() const = 0;
	virtual void setPosition(int x, int y, Size size) = 0;
	virtual void init(HWND parent, W32ControlCollection *collection) = 0;

private:
	W32Widget(const W32Widget&);
	const W32Widget &operator = (const W32Widget&);
};

typedef shared_ptr<W32Widget> W32WidgetPtr;
typedef std::vector<W32WidgetPtr> W32WidgetList;

#endif /* __W32WIDGET_H__ */
