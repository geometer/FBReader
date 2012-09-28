/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTPREVIEWWIDGET_H__
#define __ZLQTPREVIEWWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include <ZLTreePageNode.h>

class ZLQtPreviewWidget : public QWidget {

public:
	ZLQtPreviewWidget(QWidget *parent = 0);
	void fill(const ZLTreePageNode *node);
	void clear();

private:
	QLabel *myPicLabel;
	QLabel *myTitleLabel;
	QWidget *myActionsWidget;
	QList<QPushButton*> myButtons;
};

class ZLQtButtonAction : public QPushButton {
	Q_OBJECT
public:
	ZLQtButtonAction(shared_ptr<ZLTreeAction> action, QWidget *parent=0);

private Q_SLOTS:
	void onClicked();

private:
	shared_ptr<ZLTreeAction> myAction;
};

#endif /* __ZLQTPREVIEWWIDGET_H__ */
