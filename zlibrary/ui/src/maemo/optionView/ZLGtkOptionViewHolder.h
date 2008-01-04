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

#ifndef __ZLGTKOPTIONVIEWHOLDER_H__
#define __ZLGTKOPTIONVIEWHOLDER_H__

#include <string>

#include <gtk/gtkwidget.h>

#include <shared_ptr.h>

class ZLOptionView;
class ZLOptionEntry;
class ZLGtkOptionView;

class ZLGtkOptionViewHolder {

public:
	virtual ~ZLGtkOptionViewHolder();

	virtual void attachWidget(ZLOptionView &view, GtkWidget *widget) = 0;
	virtual void attachWidgets(ZLOptionView &view, GtkWidget *widget0, int weight0, GtkWidget *widget1, int weight1) = 0;

	ZLOptionView *createViewByEntry(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
};

inline ZLGtkOptionViewHolder::~ZLGtkOptionViewHolder() {}

#endif /* __ZLGTKOPTIONVIEWHOLDER_H__ */
