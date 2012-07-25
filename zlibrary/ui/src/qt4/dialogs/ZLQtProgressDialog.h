/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTWAITMESSAGE_H__
#define __ZLQTWAITMESSAGE_H__

#include <string>

#include <QtGui/QWidget>
#include <QtGui/QCursor>
#include <QtGui/QDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include <QtCore/QRunnable>

#include <ZLProgressDialog.h>

class QLabel;
class QLayout;

class ZLQtWaitDialog : public QDialog {

public:
		ZLQtWaitDialog(const std::string &message, QWidget* parent = 0);

private:
		QLayout* myLayout;
		QLabel* myLabel;
		QProgressBar* myProgressBar;

friend class ZLQtProgressDialog;
};


class ZLQtProgressDialog : public ZLProgressDialog {

public:
	ZLQtProgressDialog(const ZLResourceKey &key, bool network);

private:
	void run(ZLRunnable &runnable);
	void setMessage(const std::string &message);
	void restoreCursor();

private:
	bool myIsNetworkRunnable;
	QWidget* myActiveWindow;
};

class ZLQtRunnableWrapper : public QObject, public QRunnable {
	Q_OBJECT;
public:
	ZLQtRunnableWrapper(ZLRunnable& runnable);
	void run();
Q_SIGNALS:
	void finished();
private:
	ZLRunnable& myRunnable;
};

#endif /* __ZLQTWAITMESSAGE_H__ */
