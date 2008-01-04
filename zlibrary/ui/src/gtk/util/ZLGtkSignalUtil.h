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

#ifndef __ZLGTKSIGNALUTIL_H__
#define __ZLGTKSIGNALUTIL_H__

#include <vector>
#include <map>

#include <gtk/gtk.h>

class ZLGtkSignalUtil {

public:
	static void connectSignal(GtkObject *object, const char *name, GtkSignalFunc function, void *data);
	static void removeAllSignals();

private:
	static std::vector<std::pair<GtkObject*,int> > ourConnectedSignals;

private:
	ZLGtkSignalUtil();
};

#endif /* __ZLGTKSIGNALUTIL_H__ */
