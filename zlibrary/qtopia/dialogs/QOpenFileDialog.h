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

#include <qlistview.h>

#include "FullScreenDialog.h"
#include "../../abstract/dialogs/ZLOpenFileDialog.h"

class QVBox;
class ZLDir;

class QOpenFileDialogItem : public QListViewItem {

public:
	QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const QString name, bool dir);
	bool isDir() { return myIsDir; }
	QString name();

private:
	bool myIsDir;
};

class QOpenFileDialog : public FullScreenDialog, public ZLOpenFileDialog {
	Q_OBJECT

public:
	static std::string getOpenFileName(const char *caption);

private:
	QOpenFileDialog(const char *caption); 

	void updateListView();

protected:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

private slots:
	void accept();

private:
	QListView *myListView;
	QVBox *myMainBox;
};

#endif /* __QOPENFILEDIALOG_H__ */
