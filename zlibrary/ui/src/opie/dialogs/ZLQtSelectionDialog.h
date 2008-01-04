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

#ifndef __ZLQTSELECTIONDIALOG_H__
#define __ZLQTSELECTIONDIALOG_H__

#include <string>
#include <map>

#include <qlistview.h>

#include "ZLFullScreenDialog.h"
#include <ZLSelectionDialog.h>

class QVBox;
class QLineEdit;

class ZLQtSelectionDialogItem : public QListViewItem {

public:
	ZLQtSelectionDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node);
	ZLTreeNodePtr node() const { return myNode; }

private:
	ZLTreeNodePtr myNode;
};

class ZLQtSelectionDialog : public ZLFullScreenDialog, public ZLSelectionDialog {
	Q_OBJECT

public:
	ZLQtSelectionDialog(const std::string &caption, ZLTreeHandler &handler); 
	~ZLQtSelectionDialog();
	bool run();

private:
	QPixmap &getPixmap(const ZLTreeNodePtr node);

protected:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void exitDialog();
	void updateStateLine();
	void updateList();
	void selectItem(int index);

private slots:
	void runNodeSlot();
	void accept();

private:
	QLineEdit *myStateLine;
	QListView *myListView;
	QVBox *myMainBox;
	std::map<std::string,QPixmap*> myPixmaps;
};

#endif /* __ZLQTSELECTIONDIALOG_H__ */
