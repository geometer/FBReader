/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTSEARCHFIELD_H__
#define __ZLQTSEARCHFIELD_H__

#include <QtCore/QSet>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

#include "QtWaitingSpinner.h"
#include "ZLQtItemsListWidget.h"

class ZLQtSearchField : public QLineEdit {
	Q_OBJECT;
public:	
	ZLQtSearchField(QWidget *parent = 0);
	QtWaitingSpinner *getWaitingIcon();

protected Q_SLOTS:
	void onReturnPressed();

protected:
	void updateSuggestions();
	void loadSuggestions();
	void saveSuggestions();

protected:
	void resizeEvent(QResizeEvent *ev);

private:
	QLabel *mySearchIcon;
	QtWaitingSpinner *myWaitingIcon;
	QSet<QString> mySuggestions;
};

#endif /* __ZLQTSEARCHFIELD_H__ */
