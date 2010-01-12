/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTPOPUPMENU_H__
#define __ZLQTPOPUPMENU_H__

#include <map>

#include <qpopupmenu.h>

#include <shared_ptr.h>
#include <ZLPopupData.h>

class ZLQtPopupMenu : public QPopupMenu {
	Q_OBJECT

public:
	ZLQtPopupMenu(QWidget *parent);
	void reset(shared_ptr<ZLPopupData> popupData);

private slots:
	void onActivated(int itemId);

private:
	shared_ptr<ZLPopupData> myPopupData;
	std::map<int,int> myItemIdToIndex;
};

#endif /* __ZLQTPOPUPMENU_H__ */
