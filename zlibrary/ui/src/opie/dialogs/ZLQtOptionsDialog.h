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

#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__

#include <vector>

#include <qwidget.h>
#include <qtabwidget.h>

#include <ZLOptionsDialog.h>

#include "ZLFullScreenDialog.h"

class ZLQtOptionsDialog : public ZLFullScreenDialog, public ZLOptionsDialog {
	Q_OBJECT

public:
	ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction);
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

	void resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *event);

private:
	QTabWidget *myTabWidget;
};

class MyQTabWidget : public QTabWidget {
	Q_OBJECT

public:
	MyQTabWidget(QWidget *parent);
	void resizeEvent(QResizeEvent *event);

signals:
	void resized(const QSize &size);
};

#endif /* __ZLQTOPTIONSDIALOG_H__ */
