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

#ifndef __ZLQTPROGRESSDIALOG_H__
#define __ZLQTPROGRESSDIALOG_H__

#include <string>

#include <qwidget.h>
#include <qcursor.h>

#include <ZLProgressDialog.h>

class ZLQtWaitMessage;

class ZLQtProgressDialog : public ZLProgressDialog {

public:
	ZLQtProgressDialog(const ZLResourceKey &key);

private:
	void run(ZLRunnable &runnable);
	void setMessage(const std::string &message);

private:
	ZLQtWaitMessage *myWaitMessage;
};

class ZLQtWaitMessage : public QWidget {

public:
	ZLQtWaitMessage(const std::string &message);
	~ZLQtWaitMessage();

	void paintEvent(QPaintEvent *event);

private:
	QCursor myStoredCursor;
	bool myCursorIsStored;
	QString myLabelText;

friend class ZLQtProgressDialog;
};

#endif /* __ZLQTPROGRESSDIALOG_H__ */
