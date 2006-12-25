/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __ZLQSELECTIONDIALOG_H__
#define __ZLQSELECTIONDIALOG_H__

#include <string>
#include <map>

#include <qlistview.h>

#include "FullScreenDialog.h"
#include <ZLOpenFileDialog.h>

class QVBox;
class QLineEdit;

class ZLQSelectionDialogItem : public QListViewItem {

public:
	ZLQSelectionDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node);
	ZLTreeNodePtr node() const { return myNode; }

private:
	ZLTreeNodePtr myNode;
};

class ZLQSelectionDialog : public FullScreenDialog, public ZLOpenFileDialog {
	Q_OBJECT

public:
	ZLQSelectionDialog(const char *caption, ZLTreeHandler &handler); 
	~ZLQSelectionDialog();
	void run();

private:
	QPixmap &getPixmap(const ZLTreeNodePtr node);

protected:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void exitDialog();
	void update(const std::string &selectedNodeName);

private slots:
	void accept();

private:
	QLineEdit *myStateLine;
	QListView *myListView;
	QVBox *myMainBox;
	std::map<std::string,QPixmap*> myPixmaps;
};

#endif /* __ZLQSELECTIONDIALOG_H__ */
