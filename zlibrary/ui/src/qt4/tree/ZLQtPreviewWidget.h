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
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

#include <ZLTreePageNode.h>

class ZLQtPreviewWidget : public QWidget {

public:
	ZLQtPreviewWidget(QWidget *parent = 0);
	void fill(const ZLTreePageInfo &info);
	void fillCatalog(const ZLTreeTitledNode *node);
	void clear();

private:
	//QHBoxLayout *myLayout;
	QWidget *myWidget;
};

class ZLQtPageWidget : public QWidget {

public:
	ZLQtPageWidget(const ZLTreePageInfo &info, QWidget *parent = 0);

private:
	void createElements();
	void setInfo(const ZLTreePageInfo &info);

private:
	QLabel *myPicLabel;
	QLabel *myTitleLabel;
	QLabel *myAuthorLabel;
	QLabel *myCategoriesLabel;
	QLabel *mySummaryTitleLabel;
	QLabel *mySummaryLabel;
	QWidget *myActionsWidget;
	QScrollArea *mySummaryScrollArea;
};


class ZLQtCatalogPageWidget : public QWidget {

public:
	ZLQtCatalogPageWidget(const ZLTreeTitledNode *node, QWidget *parent = 0);

private:
	void createElements();
	void setInfo(const ZLTreeTitledNode *);

private:
	QLabel *myPicLabel;
	QLabel *myTitleLabel;
	QLabel *mySubtitleLabel;
	QWidget *myActionsWidget;
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
