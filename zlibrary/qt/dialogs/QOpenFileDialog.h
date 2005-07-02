/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __QOPENFILEDIALOG_H__
#define __QOPENFILEDIALOG_H__

#include <string>
#include <map>

#include <qdialog.h>
#include <qlistview.h>

#include "../../desktop/dialogs/ZLDesktopOpenFileDialog.h"

class QVBox;
class QLineEdit;

class QOpenFileDialogItem : public QListViewItem {

public:
	QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node);
	ZLTreeNodePtr node() const { return myNode; }

private:
	ZLTreeNodePtr myNode;
};

class QOpenFileDialog : public QDialog, public ZLDesktopOpenFileDialog {
	Q_OBJECT

public:
	QOpenFileDialog(const char *caption, const ZLTreeHandler &handler); 
	~QOpenFileDialog();
	void run();

private:
	QPixmap &getPixmap(const ZLTreeNodePtr node);

protected:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void setSize(int width, int height) { QDialog::resize(width, height); }
	int width() const { return QDialog::width(); }
	int height() const { return QDialog::height(); }

	void exitDialog();
	void update(const std::string &selectedNodeName);

private slots:
	void accept();

private:
	QLineEdit *myCurrentDirectoryName;
	QListView *myListView;
	QVBox *myMainBox;
	std::map<std::string,QPixmap*> myPixmaps;
};

inline void QOpenFileDialog::run() {
	QDialog::exec();
}

#endif /* __QOPENFILEDIALOG_H__ */
