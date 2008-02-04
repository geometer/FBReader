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

#ifndef __ZLQTOPTIONVIEWHOLDER_H__
#define __ZLQTOPTIONVIEWHOLDER_H__

#include <string>

#include <qwidget.h>

#include <shared_ptr.h>

class ZLOptionView;
class ZLOptionEntry;
class ZLQtOptionView;

class ZLQtOptionViewHolder {

public:
	virtual ~ZLQtOptionViewHolder();

	virtual void attachWidget(ZLOptionView &view, QWidget *widget) = 0;
	virtual void attachWidgets(ZLOptionView &view, QWidget *widget0, int weight0, QWidget *widget1, int weight1) = 0;
	virtual QWidget *widget() = 0;
	virtual QWidget *parentWidget() = 0;

protected:
	ZLOptionView *createViewByEntry(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
};

inline ZLQtOptionViewHolder::~ZLQtOptionViewHolder() {}

#endif /* __ZLQTOPTIONVIEWHOLDER_H__ */
