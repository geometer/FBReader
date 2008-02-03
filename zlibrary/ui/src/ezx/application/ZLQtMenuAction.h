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

#ifndef __ZLQTMENUACTION_H__
#define __ZLQTMENUACTION_H__

#include <qaction.h>

#include <ZLApplication.h>

class ZLQtApplicationWindow;

class ZLQtMenuAction : public QAction {
	Q_OBJECT

public:
	ZLQtMenuAction(ZLQtApplicationWindow &window, const ZLApplication::Menubar::PlainItem &item);

private slots:
	void doSlot();

private:
	const ZLQtApplicationWindow &myWindow;
	const std::string &myActionId;
};

#endif /* __ZLQTMENUACTION_H__ */
